import xml.sax
import sys, time

import util
import Parser


class PreludeHandler(xml.sax.handler.ContentHandler):

    def __init__(self, agent, plugin):
        self.inContent = False
        self.theContent = ""

        self.inSource   = False
        self.inTarget   = False
        self.inIpv4     = False

        self.alert  = { 'dst_ip': [] }

        self.agent = agent
        self.sendAlert  = agent.sendAlert
        self.plugin = plugin

        xml.sax.handler.ContentHandler.__init__(self)


    def startElement (self, name, attrs):
        if name == "Alert":
            self.inContent = True
            self.alert['ident'] = attrs.get('ident', "").encode("UTF-8")

        if name == "Source":
            self.inSource = True
        if name == "Target":
            self.inTarget = True
        if name == "Address" and attrs.get('category', None) == "ipv4-addr":
            self.inIpv4 = True
        if name == "Impact":
            if attrs.get('severity') == 'low':
                self.alert["priority"] = 1
            elif attrs.get('severity') == 'medium':
                self.alert["priority"] = 2
            elif attrs.get('severity') == 'high':
                self.alert["priority"] = 3


    def endElement (self, name):
        if self.inContent:

            # date
            if name == "DetectTime":
                (date, gmt) = self.theContent.encode("UTF-8").split(".")
                self.alert["date"] = date

            # src_ip
            if self.inSource:
                if self.inIpv4 and name == "address":
                    self.alert["src_ip"] = self.theContent.encode("UTF-8")

            # dst_ip
            if self.inTarget:
                if self.inIpv4 and name == "address":
                    self.alert["dst_ip"].append(
                        self.theContent.encode("UTF-8"))
                if name == "protocol":
                    self.alert["protocol"] = self.theContent.encode("UTF-8")

            if name == "Source":    self.inSource = False
            if name == "Target":    self.inTarget = False
            if name == "Address" \
                and self.inIpv4:    self.inIpv4   = False


        if name == "Alert":
            self.inContent = False
            self.send()

            self.alert = { 'dst_ip': [] }


    def characters (self, string):
        self.theContent = string


    def send(self):
        for dst_ip in self.alert["dst_ip"]:

            self.sendAlert(
                    type        = "detector",
                    date        = self.alert["date"],
                    sensor      = self.plugin["sensor"],
                    interface   = self.plugin["interface"],
                    plugin_id   = self.plugin["id"],
                    plugin_sid  = 1,
                    priority    = self.alert["priority"],
                    protocol    = self.alert["protocol"],
                    src_ip      = self.alert["src_ip"],
                    src_port    = "",
                    dst_ip      = dst_ip,
                    dst_port    = ""
                )


class ParserPrelude(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for P0f...", '!!', 'RED')
            sys.exit()

    def __processSyslog(self):

        util.debug ('ParserPrelude', 'plugin started (syslog)...', '--')

        start_time = time.time()

        location = self.plugin["location"]
        try:
            fd = open(location, 'r')
        except IOError, e:
            util.debug(__name__, e, '!!', 'RED')
            sys.exit()

        
        # SAX parser
        preludeParser = xml.sax.make_parser()
        preludeHandler = PreludeHandler(self.agent, self.plugin)
        preludeParser.setContentHandler(preludeHandler)
        preludeParser.feed("<IDMEF-MESSAGES>") # Root element


        # Move to the end of file
        fd.seek(0, 2)
            
        while 1:

            if self.plugin["enable"] == 'no':

                # plugin disabled, wait for enabled
                util.debug (__name__, 'plugin disabled', '**', 'YELLOW')
                while self.plugin["enable"] == 'no':
                    time.sleep(1)
                    
                # lets parse again
                util.debug (__name__, 'plugin enabled', '**', 'GREEN')
                fd.seek(0, 2)

            where = fd.tell()
            line = fd.readline()
            if not line: # EOF reached
                time.sleep(1)
                fd.seek(where)

                # restart plugin every hour
                if self.agent.plugin_restart_enable:
                    current_time = time.time()
                    if start_time + \
                       self.agent.plugin_restart_interval < current_time:
                        util.debug(__name__, 
                                   "Restarting plugin..", '->', 'YELLOW')
                        fd.close()
                        start_time = current_time
                        return None

            else:
                preludeParser.feed(line)



