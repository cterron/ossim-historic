import xml.sax
import Config
import Parser
import socket
import sys

class Agent:
    
    def __init__(self):        
        self.serverIp = ''
        self.plugins = []
        self.listenPort = 0
        self.conn = None

    def parseConfig(self):
        """parse config file with the SAX API"""

        # init parser and set XML Handler
        configParser = xml.sax.make_parser()
        configHandler = Config.ConfigHandler()
        configParser.setContentHandler(configHandler)

        # let's go parse!
        configParser.parse('config.xml')

        # store data
        self.serverIp = Config.data.serverIp
        self.listenPort = Config.data.serverPort
        self.plugins = Config.data.plugins


    def connect(self):
        """Connect to server"""
        
        self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.conn.connect((self.serverIp, self.listenPort))
        except socket.error, e:
            print 'Error connecting to server (' + self.serverIp + \
            ', ' + str(self.listenPort) + ') ... ' + str(e)
            sys.exit()

        self.conn.send('connect\n')
        print "Waiting for server..."
        data = self.conn.recv(1024)
        if data == 'connect-ok\n':
            print "Server connected\n"
        else:
            print "Bad response: " + str(data)
            sys.exit()


    def parser(self):

        for plugin in self.plugins:
            
            #
            #  multithreading at this point !!!!
            #

            if plugin["enable"] == 'yes':
                parser = Parser.Parser(self.conn, plugin)
                parser.start()

    def close(self):
        self.conn.close()


