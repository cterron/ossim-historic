import xml.sax, socket, time

import Config
import Parser
import Server
import Scheduler
import Watchdog
import MonitorList
import util


class Agent:
    
    def __init__(self):
        self.serverIp = ''
        self.listenPort = 0
        self.watchdog_enable = True
        self.watchdog_interval = 60
        self.logdir = ''
        self.plugins = {}
        self.conn = None
        self.sequence = 0
        self.mlist = MonitorList.MonitorList()
#        self.my_ip = socket.gethostbyname(socket.gethostname())

    def parseConfig(self, config_file):
        """parse config file with the SAX API"""

        # init parser and set XML Handler
        configParser = xml.sax.make_parser()
        configHandler = Config.ConfigHandler()
        configParser.setContentHandler(configHandler)

        # let's go parse!
        configParser.parse(config_file)

        # store data
        (self.serverIp, self.listenPort) = configHandler.get_conn()
        if configHandler.get_watchdog_enable() in ['yes', 'true']:
            self.watchdog_enable = True
        else: 
            self.watchdog_enable = False
        self.watchdog_interval = configHandler.get_watchdog_interval()
        self.logdir = configHandler.get_logdir()
        self.plugins = configHandler.get_plugins()


    def connect(self):
        """Connect to server"""
        
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        data = ""
        try:
            self.conn.connect((self.serverIp, self.listenPort)) 
            self.sequence = 1
            msg = 'connect id="%s" type="sensor"\n' % (self.sequence)
            self.conn.send(msg)
            util.debug (__name__,  "Waiting for server...", '->', 'YELLOW')
            data = self.conn.recv(1024)
        except socket.error, e:
            util.debug (__name__, 
                'Error connecting to server (' + self.serverIp + \
                ', ' + str(self.listenPort) + ') ... ' + str(e),
                '!!', 'RED')
            return None

        if data == 'ok id="' + str(self.sequence) + '"\n':
            util.debug (__name__, "Server connected\n", '<-', 'GREEN')
            return self.conn
        else:
            util.debug (__name__, "Bad response: " + str(data), '!!', 'RED')
            return None

    def append_plugins(self):
        util.debug (__name__, "Apending plugins...", '=>', 'YELLOW')
        for key, plugin in self.plugins.iteritems():
            self.sequence += 1
            if plugin["enable"] == 'yes':
                msg = 'session-append-plugin ' + \
                      'id="%d" plugin_id="%s" ' % \
                        (self.sequence, plugin["id"]) + \
                      'enabled="true" ' + \
                      'state="start"\n'
            else:
                msg = 'session-append-plugin ' + \
                      'id="%d" plugin_id="%s" ' % \
                        (self.sequence, plugin["id"]) + \
                      'enabled="false" ' + \
                      'state="stop"\n'
            self.conn.send(msg)

    def reconnect(self):
        "Reset the current connection by closing and reopening it"
        self.close()
        while 1:
            
            # check if another thread has reconnected...
            print util.debug(__name__, "Connection: " + str(self.conn), 
                             'WW', 'YELLOW')
            if self.conn is not None:
                return self.conn

            # new connection
            conn = self.connect()
            if conn is not None:
                self.append_plugins()
                break
            else:
                util.debug (__name__, 
                    'Can\'t connect, retrying in 10 seconds\n',
                    '!!', 'RED')
                time.sleep(10)
            
        return conn

    def watchdog(self):
        if self.watchdog_enable:
            watchdog = Watchdog.Watchdog(self)
            watchdog.start()

    def server(self):
        server = Server.Server(self)
        server.start()

    def scheduler(self):
        scheduler = Scheduler.Scheduler(self)
        scheduler.start()


    def parser(self):

        import ParserSnort, \
            ParserApache, \
            ParserIIS, \
            ParserIptables, \
            ParserFW1, \
            ParserRealSecure, \
            ParserRRD, \
            ParserCA, \
            ParserCisco, \
            ParserP0f, \
            ParserArpwatch, \
            ParserPrelude, \
            ParserCiscoPIX, \
            ParserCiscoIDS, \
            ParserPads, \
            ParserNTsyslog, \
            ParserOsiris, \
            ParserSyslog

        parsers = {
            "1001":     ParserSnort.ParserSnort,
            "1501":     ParserApache.ParserApache,
            "1502":     ParserIIS.ParserIIS,
            "1503":     ParserIptables.ParserIptables,
            "1504":     ParserFW1.ParserFW1,
            "1506":     ParserRealSecure.ParserRealSecure,
            "1507":     ParserRRD.ParserRRD,
            "1508":     ParserRRD.ParserRRD,
            "1509":     ParserCA.ParserCA,
            "1510":     ParserCisco.ParserCisco,        # TODO: more db sids
            "1511":     ParserP0f.ParserP0f,
            "1512":     ParserArpwatch.ParserArpwatch,
            "1513":     ParserPrelude.ParserPrelude,
            "1514":     ParserCiscoPIX.ParserCiscoPIX,  # TODO: more db sids
            "1515":     ParserCiscoIDS.ParserCiscoIDS,
            "1516":     ParserPads.ParserPads,
            "1517":     ParserNTsyslog.ParserNTsyslog,
            "4001":     ParserOsiris.ParserOsiris,
            "4002":     ParserSyslog.ParserSyslog,
        }

        #
        #  multithreading at this point !!!!
        #
        for key, plugin in self.plugins.iteritems():
            if plugin["type"] == 'detector':
                try:
                    parser = parsers[plugin["id"]](self, plugin)
                    parser.start()
                except KeyError:
                    util.debug (__name__, 
                                "Plugin %s (%s) is not implemented..." % \
                                (plugin["process"], plugin["id"]),
                                '!!', 'RED')
 

    def close(self):
        if self.conn is not None:
            self.conn.close()
            self.conn = None


    def sendMessage(self, message) :
        while 1:
            if self.conn is not None:
                
                try:
                    self.conn.send(message + '\n')
                    util.debug (__name__, message + '\n',
                                '=>', 'CYAN')
                    break
                except socket.error, e:
                    util.debug (__name__, 
                        'Error sending data: %s, retrying in 10 seconds\n' % \
                        (e), '!!', 'RED')
                    time.sleep(10)
                    self.conn = self.reconnect()
                
            else:
                util.debug (__name__, 'Trying to connect in 10 seconds\n',
                            '**', 'YELLOW')
                time.sleep(10)
                self.conn = self.reconnect()
        

    def sendAlert(self, type, date, sensor, interface, 
                  plugin_id, plugin_sid, priority, protocol, 
                  src_ip, src_port, dst_ip, dst_port, log = "",
                  snort_cid="", snort_sid="",
                  data="", condition="", value=""):

        message = 'alert '
        if type:        message +=  'type="'        + str(type)         + '" '
        if date:        message +=  'date="'        + str(date)         + '" '
        if plugin_id:   message +=  'plugin_id="'   + str(plugin_id)    + '" '
        if plugin_sid:  message +=  'plugin_sid="'  + str(plugin_sid)   + '" '
        if sensor:      message +=  'sensor="'      + str(sensor)       + '" '
        if interface:   message +=  'interface="'   + str(interface)    + '" '
        if priority:    message +=  'priority="'    + str(priority)     + '" '
        if protocol:    message +=  'protocol="'    + str(protocol)     + '" '
        if src_ip:      message +=  'src_ip="'      + str(src_ip)       + '" '
        if src_port:    message +=  'src_port="'    + str(src_port)     + '" '
        if dst_ip:      message +=  'dst_ip="'      + str(dst_ip)       + '" '
        if dst_port:    message +=  'dst_port="'    + str(dst_port)     + '" '
        if data:        message +=  'data="'        + str(data)         + '" '
        if log:
            message +=  'log="' + log.rstrip().replace("\"", "\\\"") + '" '

        # snort specific
        if snort_cid:   message +=  'snort_cid="'   + str(snort_cid)    + '" '
        if snort_sid:   message +=  'snort_sid="'   + str(snort_sid)    + '" '

        # Monitors specific
        if condition:   message +=  'condition="'   + str(condition)    + '" '
        if value:       message +=  'value="'       + str(value)        + '" '

        if priority > 0 :
            self.sendMessage(message)


    def sendOsChange(self, host, os, date, plugin_id, plugin_sid, log):
        
        message = 'host-os-new ' +\
            'host="'        + str(host)         + '" ' +\
            'os="'          + str(os)           + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '
    
        self.sendMessage(message)
            
    def sendMacChange(self, host, mac, vendor, date, 
                      plugin_id, plugin_sid, log):
        
        message = 'host-mac-new ' +\
            'host="'        + str(host)         + '" ' +\
            'mac="'         + str(mac)          + '" ' +\
            'vendor="'      + str(vendor)       + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '
            
        self.sendMessage(message)

    def sendService(self, host, port, proto, service, application, date,
                    plugin_id, plugin_sid, log):

        message = 'host-service-new ' +\
            'host="'        + str(host)         + '" ' +\
            'port="'        + str(port)         + '" ' +\
            'protocol="'    + str(proto)        + '" ' +\
            'service="'     + str(service)      + '" ' +\
            'application="' + str(application)  + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '

        self.sendMessage(message)
