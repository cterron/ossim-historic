import xml.sax
import string

class data:
    serverIp = ''
    serverPort = ''
    plugins = []

def normalizeWhitespace(text):
    "Remove redundant whitespace from a string"
    return string.join(string.split(text), ' ')

class ConfigHandler(xml.sax.handler.ContentHandler):

    def __init__(self):
        self.inContent = 0
        self.theContent = ""

    def startElement (self, name, attrs):

        if name == 'serverip' or name == 'serverport' or name == 'path' \
          or name == 'type' or name == 'location':
            self.inContent = 1

        if name == 'plugin':
            plugin = {'name' : '', \
                      'enable' : '', \
                      'path' : '', \
                      'log_type' : '', \
                      'log_location' : ''}
            plugin["name"] = normalizeWhitespace(attrs.get('name', None))
            plugin["enable"] = normalizeWhitespace(attrs.get('enable', None))
            data.plugins.append(plugin)
        
        
    def endElement (self, name):

        if self.inContent:
            self.theContent = normalizeWhitespace(self.theContent)
        if name == 'serverip':
            data.serverIp = self.theContent.encode("iso-8859-1")
        if name == 'serverport':
            data.serverPort = int(self.theContent.encode("iso-8859-1"))
        if name == 'path':
            data.plugins[-1]["path"] = \
                self.theContent.encode("iso-8859-1")
        if name == 'type':
            data.plugins[-1]["log_type"] = \
                self.theContent.encode("iso-8859-1")
        if name == 'location':
            data.plugins[-1]["log_location"] = \
                self.theContent.encode("iso-8859-1")
 
    def characters (self, string):
        if self.inContent:
            self.theContent = string


