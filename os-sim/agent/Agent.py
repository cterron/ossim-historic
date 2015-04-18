import xml.sax
import socket
import time
import sys
import os

import Config
import Parser
import Monitor
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
        self.serverIp = Config.ConfigHandler.serverIp
        self.listenPort = Config.ConfigHandler.serverPort
        if Config.ConfigHandler.watchdog_enable in ['yes', 'true']:
            self.watchdog_enable = True
        else: 
            self.watchdog_enable = False
        self.watchdog_interval = Config.ConfigHandler.watchdog_interval
        self.logdir = Config.ConfigHandler.logdir
        self.plugins = Config.ConfigHandler.plugins


    def connect(self):
        """Connect to server"""
        
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.conn.connect((self.serverIp, self.listenPort)) 
            self.sequence = 1
            self.conn.send('connect id="%s" type="sensor"\n' % (self.sequence))
            util.debug (__name__,  "Waiting for server...", '->', 'YELLOW')
            data = self.conn.recv(1024)
        except socket.error, e:
            util.debug (__name__, 
                'Error connecting to server (' + self.serverIp + \
                ', ' + str(self.listenPort) + ') ... ' + str(e),
                '!!', 'RED')
            os.remove(os.path.join(util.RUN_DIR, 'ossim_agent.pid'))
            sys.exit()

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

#            if plugin["enable"] == 'yes' and plugin["type"] == 'detector':
            if plugin["type"] == 'detector':
                parser = Parser.Parser(self, plugin)
                parser.start()

    def close(self):
        self.conn.close()


    def sendMessage(self, message) :
        try:
            if self.conn:
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
        

    def sendAlert(self, type, date, sensor, interface, 
                  plugin_id, plugin_sid, priority, protocol, 
                  src_ip, src_port, dst_ip, dst_port, 
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

        # snort specific
        if snort_cid:   message +=  'snort_cid="'   + str(snort_cid)    + '" '
        if snort_sid:   message +=  'snort_sid="'   + str(snort_sid)    + '" '

        # Monitors specific
        if condition:   message +=  'condition="'   + str(condition)    + '" '
        if value:       message +=  'value="'       + str(value)        + '" '

        if priority > 0 :
            self.sendMessage(message)


    def sendOsChange(self, host, os, date, plugin_id, plugin_sid):
        
        message = 'host-os-change ' +\
            'host="'        + str(host)         + '" ' +\
            'os="'          + str(os)           + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" '
            
        self.sendMessage(message)
            
    def sendMacChange(self, host, mac, vendor, date, plugin_id, plugin_sid):
        
        message = 'host-mac-change ' +\
            'host="'        + str(host)         + '" ' +\
            'mac="'         + str(mac)          + '" ' +\
            'vendor="'      + str(vendor)       + '" ' +\
            'date="'        + str(date)         + '" ' +\
            'plugin_id="'   + str(plugin_id)    + '" ' +\
            'plugin_sid="'  + str(plugin_sid)   + '" '
            
        self.sendMessage(message)

