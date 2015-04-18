import xml.sax
import string
import util


class ConfigHandler(xml.sax.handler.ContentHandler):

    serverIp = ''
    serverPort = ''
    plugins = {}

    def __init__(self):
        self.plugin_id = 0
        self.inContent = 0
        self.theContent = ""
        xml.sax.handler.ContentHandler.__init__(self)

    def startElement (self, name, attrs):

        if name in ['serverip', 'serverport', 'source', 'location', \
                    'interface', 'sensor', 'startup', 'shutdown', 'logdir'] :
            self.inContent = 1

        if name == 'plugin':

            plugin = {'id' : '', \
                      'process' : '', \
                      'type' : '', \
                      'start': '', \
                      'enable' : '', \
                      'startup' : '', \
                      'shutdown' : '', \
                      'source' : '', \
                      'interface' : '', \
                      'sensor' : '', \
                      'location' : '', \
                      'frequency': ''}
            plugin["id"] = util.normalizeWhitespace(attrs.get('id', None))
            self.plugin_id = plugin["id"]
            plugin["process"] = util.normalizeWhitespace(attrs.get('process', None))
            plugin["type"] = util.normalizeWhitespace(attrs.get('type', None))
            plugin["start"] = util.normalizeWhitespace(attrs.get('start', None))
            plugin["enable"] = util.normalizeWhitespace(attrs.get('enable', None))
            ConfigHandler.plugins[plugin["id"]] = plugin
            
        elif name == 'watchdog':
            ConfigHandler.watchdog_enable = \
                util.normalizeWhitespace(attrs.get('enable', None))
            ConfigHandler.watchdog_interval = \
                util.normalizeWhitespace(attrs.get('interval', None))

        
    def endElement (self, name):

        if self.inContent:
            self.theContent = util.normalizeWhitespace(self.theContent)
        if name == 'serverip':
            ConfigHandler.serverIp = self.theContent.encode("UTF-8")
        if name == 'serverport':
            ConfigHandler.serverPort = int(self.theContent.encode("UTF-8"))
        if name == 'logdir':
            ConfigHandler.logdir = self.theContent.encode("UTF-8")
        if name == 'startup':
            ConfigHandler.plugins[self.plugin_id]["startup"] = \
                self.theContent.encode("UTF-8")
        if name == 'shutdown':
            ConfigHandler.plugins[self.plugin_id]["shutdown"] = \
                self.theContent.encode("UTF-8")
        if name == 'source':
            ConfigHandler.plugins[self.plugin_id]["source"] = \
                self.theContent.encode("UTF-8")
        if name == 'location':
            ConfigHandler.plugins[self.plugin_id]["location"] = \
                self.theContent.encode("UTF-8")
        if name == 'interface':
            ConfigHandler.plugins[self.plugin_id]["interface"] = \
                self.theContent.encode("UTF-8")
        if name == 'sensor':
            ConfigHandler.plugins[self.plugin_id]["sensor"] = \
                self.theContent.encode("UTF-8")
        if name == 'frequency':
            ConfigHandler.plugins[self.plugin_id]["frequency"] = \
                self.theContent.encode("UTF-8")
 
    def characters (self, string):
        
        # why sax parser break lines when an entity appears?
        # is it a python bug?
        if string == '&':
            self.theContent += string
        
        elif self.inContent:
            self.theContent = string


