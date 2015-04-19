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
        self.plugin_restart_enable = True
        self.plugin_restart_interval = 600
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
        if configHandler.get_plugin_restart_enable() in ['yes', 'true']:
            self.plugin_restart_enable = True
        else:
            self.plugin_restart_enable = False
        self.plugin_restart_interval = \
            configHandler.get_plugin_restart_interval()
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
            ParserNetgear, \
            ParserNTsyslog, \
            ParserSnareWindows, \
            ParserOsiris, \
            ParserSyslog, \
            ParserPostfix, \
            ParserNetscreen, \
            ParserJuniperFW 
#            ParserIntruShield

        parsers = {
            "1001":     ParserSnort.ParserSnort,
#            "1003":     ParserIntruShield.ParserIntruShield,
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
            "1518":     ParserSnareWindows.ParserSnareWindows,
            "1519":     ParserNetgear.ParserNetgear,    # TODO: more db sids
            "1520":     ParserNetscreen.ParserNetscreen,
            "1521":     ParserPostfix.ParserPostfix,
            "1522":     ParserJuniperFW.ParserJuniperFW,
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
        

    def sanitize(self, string):
        return str(string).rstrip().replace("\"", "\\\"").replace("'", "")

    def sendEvent(self, type, date, sensor, interface, 
                  plugin_id, plugin_sid, priority, protocol, 
                  src_ip, src_port, dst_ip, dst_port, log = "",
                  snort_cid="", snort_sid="",
                  data="", condition="", value=""):

        s = self.sanitize

        message = 'event '
        if type:        message +=  'type="'        + s(type)         + '" '
        if date:        message +=  'date="'        + s(date)         + '" '
        if plugin_id:   message +=  'plugin_id="'   + s(plugin_id)    + '" '
        if plugin_sid:  message +=  'plugin_sid="'  + s(plugin_sid)   + '" '
        if sensor:      message +=  'sensor="'      + s(sensor)       + '" '
        if interface:   message +=  'interface="'   + s(interface)    + '" '
        if priority:    message +=  'priority="'    + s(priority)     + '" '
        if protocol:    message +=  'protocol="'    + s(protocol)     + '" '
        if src_ip:      message +=  'src_ip="'      + s(src_ip)       + '" '
        if src_port:    message +=  'src_port="'    + s(src_port)     + '" '
        if dst_ip:      message +=  'dst_ip="'      + s(dst_ip)       + '" '
        if dst_port:    message +=  'dst_port="'    + s(dst_port)     + '" '
        if data:        message +=  'data="'        + s(data)         + '" '
        if log:         message +=  'log="'         + s(log)          + '" '

        # snort specific
        if snort_cid:   message +=  'snort_cid="'   + s(snort_cid)    + '" '
        if snort_sid:   message +=  'snort_sid="'   + s(snort_sid)    + '" '

        # Monitors specific
        if condition:   message +=  'condition="'   + s(condition)    + '" '
        if value:       message +=  'value="'       + s(value)        + '" '

        if priority > 0 :
            self.sendMessage(message)


    def sendOsChange(self, host, os, date, sensor, iface, plugin_id, plugin_sid, log):
        
        message = 'host-os-event ' +\
            'host="'        + str(host)         + '" ' +\
            'os="'          + str(os)           + '" ' +\
            'sensor="'      + str(sensor)       + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'interface="'   + str(iface)        + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '
    
        self.sendMessage(message)
            
    def sendMacEvent(self, host,  iface, mac, vendor, date,
                    sensor, plugin_id, plugin_sid, log):
        
        message = 'host-mac-event ' +\
            'host="'        + str(host)         + '" ' +\
            'interface="'   + str(iface)        + '" ' +\
            'mac="'         + str(mac)          + '" ' +\
            'vendor="'      + str(vendor)       + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'sensor="'      + str(sensor)       + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '
            
        self.sendMessage(message)

    def sendService(self, host, sensor, iface, port, proto, service, application, date,
                    plugin_id, plugin_sid, log):

        message = 'host-service-event ' +\
            'host="'        + str(host)         + '" ' +\
            'sensor="'      + str(sensor)       + '" ' +\
            'interface="'   + str(iface)        + '" ' +\
            'port="'        + str(port)         + '" ' +\
            'protocol="'    + str(proto)        + '" ' +\
            'service="'     + str(service)      + '" ' +\
            'application="' + str(application)  + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '

        self.sendMessage(message)

    def sendHidsEvent(self, host, hostname, event_type, target, what, extra_data, sensor, date, plugin_id, plugin_sid, log):

        message = 'host-ids-event ' +\
            'host="'        + str(host)         + '" ' +\
            'hostname="'    + str(hostname)     + '" ' +\
            'event_type="'  + str(event_type)   + '" ' +\
            'target="'      + str(target)       + '" ' +\
            'what="'        + str(what)         + '" ' +\
            'extra_data="'  + str(extra_data)   + '" ' +\
            'sensor="'      + str(sensor)       + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" ' +\
            'log="' + log.rstrip().replace("\"", "\\\"") + '" '

        self.sendMessage(message)
