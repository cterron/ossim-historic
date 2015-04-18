import xml.sax
import socket
import time

import Config
import Parser
import Monitor
import util

class Agent:
    
    def __init__(self):
        self.serverIp = ''
        self.plugins = []
        self.listenPort = 0
        self.conn = None
        self.sequence = 0
        self.my_ip = socket.gethostbyname(socket.gethostname())

    def parseConfig(self, config_file):
        """parse config file with the SAX API"""

        # init parser and set XML Handler
        configParser = xml.sax.make_parser()
        configHandler = Config.ConfigHandler()
        configParser.setContentHandler(configHandler)

        # let's go parse!
        configParser.parse(config_file)

        # store data
        self.serverIp = Config.ConfigHandler.serverIp
        self.listenPort = Config.ConfigHandler.serverPort
        self.plugins = Config.ConfigHandler.plugins


    def connect(self):
        """Connect to server"""
        
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.conn.connect((self.serverIp, self.listenPort))
        except socket.error, e:
            util.debug (__name__, 
                'Error connecting to server (' + self.serverIp + \
                ', ' + str(self.listenPort) + ') ... ' + str(e),
                '!!', 'RED')
            return None
        
        self.sequence = 1
        self.conn.send('connect id="%s"\n' % (self.sequence))
        util.debug (__name__,  "Waiting for server...", '->', 'YELLOW')
        data = self.conn.recv(1024)
        if data == 'ok id="' + str(self.sequence) + '"\n':
            util.debug (__name__, "Server connected\n", '<-', 'GREEN')
            return self.conn
        else:
            util.debug (__name__, "Bad response: " + str(data), '!!', 'RED')
            return None

    def append_plugins(self):
        util.debug (__name__, "Apending plugins...", '=>', 'YELLOW')
        for key, plugin in self.plugins.iteritems():
            if plugin["enable"] == 'yes':
                self.sequence += 1
                self.conn.send('session-append-plugin id="%d" plugin-id="%s"\n' %\
                    (self.sequence, plugin["id"]))

    def reconnect(self):
        "Reset the current connection by closing and reopening it"
        self.close()
        while 1:
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


    def monitor(self):
        monitor = Monitor.Monitor(self)
        monitor.start()

    def parser(self):

        for key, plugin in self.plugins.iteritems():

            #
            #  multithreading at this point !!!!
            #

            if plugin["enable"] == 'yes' and plugin["type"] == 'detector':
                parser = Parser.Parser(self, plugin)
                parser.start()

    def close(self):
        self.conn.close()

    def sendMessage(self, type, date, sensor, interface, 
                    plugin_id, plugin_sid, priority, protocol, 
                    src_ip, src_port, dst_ip, dst_port, 
                    condition="", value=""):

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

        # Monitors specific
        if condition:   message +=  'condition="'   + str(condition)    + '" '
        if value:       message +=  'value="'       + str(value)        + '" '

        try:
            if self.conn:
                if priority > 0 :
                    self.conn.send(message + '\n')
                    util.debug (__name__, message + '\n',
                                '=>', 'CYAN')
            else:
                util.debug (__name__, 'Trying to connect in 10 seconds\n',
                            '**', 'YELLOW')
                time.sleep(10)
                self.conn = self.reconnect()
        except socket.error, e:
            util.debug (__name__, 
                'Error sending data: %s, retrying in 10 seconds\n' % (e),
                '!!', 'RED')
            time.sleep(10)
            self.conn = self.reconnect()


