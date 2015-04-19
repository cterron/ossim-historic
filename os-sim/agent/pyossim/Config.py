import xml.sax
import util


class ConfigHandler(xml.sax.handler.ContentHandler):

    def __init__(self):
        self.plugin_id = 0
        self.inContent = 0
        self.theContent = ""

        self._serverIp = ''
        self._serverPort = ''
        self._plugins = {}
        self._watchdog_enable = True
        self._watchdog_interval = 60
        self._plugin_restart_enable = True
        self._plugin_restart_interval = 600
        self._logdir = ''
        
        xml.sax.handler.ContentHandler.__init__(self)

    def startElement (self, name, attrs):

        self.theContent = ""

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
            self._plugins[plugin["id"]] = plugin
            
        elif name == 'watchdog':
            self._watchdog_enable = \
                util.normalizeWhitespace(attrs.get('enable', True))
            self._watchdog_interval = \
                util.normalizeWhitespace(attrs.get('interval', 60))

        elif name == 'plugin-restart':
            self._plugin_restart_enable = \
                util.normalizeWhitespace(attrs.get('enable', True))
            self._plugin_restart_interval = \
                util.normalizeWhitespace(attrs.get('interval', 600))

        
    def endElement (self, name):

        if self.inContent:
            self.theContent = util.normalizeWhitespace(self.theContent)
        if name == 'serverip':
            self._serverIp = self.theContent.encode("UTF-8")
        if name == 'serverport':
            self._serverPort = int(self.theContent.encode("UTF-8"))
        if name == 'logdir':
            self._logdir = self.theContent.encode("UTF-8")
        if name == 'startup':
            self._plugins[self.plugin_id]["startup"] = \
                self.theContent.encode("UTF-8")
        if name == 'shutdown':
            self._plugins[self.plugin_id]["shutdown"] = \
                self.theContent.encode("UTF-8")
        if name == 'source':
            self._plugins[self.plugin_id]["source"] = \
                self.theContent.encode("UTF-8")
        if name == 'location':
            self._plugins[self.plugin_id]["location"] = \
                self.theContent.encode("UTF-8")
        if name == 'interface':
            self._plugins[self.plugin_id]["interface"] = \
                self.theContent.encode("UTF-8")
        if name == 'sensor':
            self._plugins[self.plugin_id]["sensor"] = \
                self.theContent.encode("UTF-8")
        if name == 'frequency':
            self._plugins[self.plugin_id]["frequency"] = \
                self.theContent.encode("UTF-8")
 
    def characters (self, string):
       
        if self.inContent:
            self.theContent += string



    def get_conn (self):
        return (self._serverIp, self._serverPort)

    def get_plugins (self):
        return self._plugins

    def get_logdir (self):
        return self._logdir

    def get_watchdog_enable(self):
        return self._watchdog_enable
        
    def get_watchdog_interval(self):
        return self._watchdog_interval

    def get_plugin_restart_enable(self):
        return self._plugin_restart_enable

    def get_plugin_restart_interval(self):
        return int(self._plugin_restart_interval)
