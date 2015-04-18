import urllib2
import HTMLParser
import sys
import socket

def get_value(rule, url):

    fd = urllib2.urlopen(url)
    parser = NtopSessionParser()
    try:
        parser.feed(fd.read())  # Feed some text to the parser
        parser.close()          # Force processing of all buffered data
    except HTMLParser.HTMLParseError:
        pass
   
    for session in NtopSessionParser.session_data:
        
        if session["client"] == rule["from"] and \
           session["server"] == rule["to"] and \
           session["client_port"] == rule["port_from"] or \
                rule["port_from"] == '' and \
           session["server_port"] == rule["port_to"] or \
                rule["port_to"] == '':
            
            if int(rule["plugin_sid"]) == 246:
                return session["data_sent"]
            elif int(rule["plugin_sid"]) == 247:
                return session["data_recv"]
            elif int(rule["plugin_sid"]) == 248:
                return session["duration"]
   
    # no session opened
    return None


class NtopSessionParser(HTMLParser.HTMLParser):

    session_data = []

    def __init__(self):
        self.start = ''     # start tag
        self.end = ''       # end tag
        self.col = 0        # column in the main <tr></tr>
        self.data = {}      # session data
        HTMLParser.HTMLParser.__init__(self)

    def handle_starttag(self, tag, attrs):
        self.start = tag
        if self.start == 'tr':
            self.col = 0

    def handle_endtag(self, tag):
        self.end = tag


    def handle_data(self, data):

        if self.start == 'td' or self.start == 'a' or self.start == 'img': 
            
            if self.col == 0:
                
                # client
                try:
                    self.data['client'] = socket.gethostbyname(data)
                except socket.gaierror:
                    self.data['client'] = data
                
            elif self.col == 1:
                
                # client_port
                if data.startswith(':'):
                    try:
                        self.data["client_port"] = \
                            socket.getprotobyname(data[1:])
                    except socket.error:
                        self.data["client_port"] = data[1:]
                else:
                    self.data["client_port"] = ''
                    try:
                        self.data["server"] = socket.gethostbyname(data)
                    except socket.gaierror:
                        self.data["server"] = data
                    self.col = self.col + 1
                    
            elif self.col == 2:
                
                # server
                try:
                    self.data["server"] = socket.gethostbyname(data)
                except socket.gaierror:
                    self.data["server"] = data
                
            elif self.col == 3:
                
                # server_port
                if data.startswith(':'):
                    try:
                        self.data["server_port"] = \
                            socket.getprotobyname(data[1:])
                    except socket.error:
                        self.data["server_port"] = data[1:]
                else:
                    self.data["server_port"] = ''
                    self.data["data_sent"] = data
                    self.col = self.col + 1

            elif self.col == 4:
                
                # data_sent
                self.data["data_sent"] = data
                
            elif self.col == 5:
                
                # data_recv
                if data == 'KB' or data == 'MB':
                    self.col = self.col -1
                    self.data["data_sent"] = \
                        float(self.data["data_sent"]) * 1024
                else:
                    self.data["data_recv"] = data
                    
            elif self.col == 6:
                
                # active since
                if data == 'KB' or data == 'MB':
                    self.col = self.col -1
                    self.data["data_recv"] = \
                        float(self.data["data_sent"]) * 1024
                else:
                   self.data["active_since_date"] = data

            elif self.col == 7:

                # active since hour
                self.data["active_since_hour"] = data
                
            elif self.col == 8:
                
                # last seen
                self.data["last_seen_date"] = data
                
            elif self.col == 9:

                # last seen hour
                self.data["last_seen_hour"] = data
                
            elif self.col == 10:
                
                # duration
                if data.endswith('sec'):
                    self.data["duration"] = data[:-4]
                
                #
                # FIXME!!
                #
                elif data.endswith('Days'):
                    self.data["duration"] = int(data[:-5]) * 86400;
                elif data.endswith('Day'):
                    self.data["duration"] = int(data[:-4]) * 86400;
                    
                else:
                    dt = data.split(':')
                    seconds = int(dt.pop())
                    try:
                        seconds = seconds + int(dt.pop()) * 60
                        seconds = seconds + int(dt.pop()) * 3600
                    except IndexError:
                        pass
                    self.data["duration"] = str(seconds)
                
            elif self.col == 11:
                
                # inactive
                if data.endswith('secs'):
                    self.data["inactive"] = data[:-4]
                else:
                    self.data["inactive"] = data
                
            elif self.col == 12:
                
                # latency
                if data.endswith('ms'):
                    self.data["latency"] = data[:-3]
                else:
                    self.data["latency"] = data

                # debug
#                print "\nclient: %s" % self.data["client"]
#                print "client_port: %s" % self.data["client_port"]
#                print "server: %s" % self.data["server"]
#                print "server_port: %s" % self.data["server_port"]
#                print "data_sent: %s" % self.data["data_sent"]
#                print "data_recv: %s" % self.data["data_recv"]
#                print "active_since_date: %s" % self.data["active_since_date"]
#                print "active_since_hour: %s" % self.data["active_since_hour"]
#                print "last_seen_date: %s" % self.data["last_seen_date"]
#                print "last_seen_hour: %s" % self.data["last_seen_hour"]
#                print "duration: %s" % self.data["duration"]
#                print "inactive: %s" % self.data["inactive"]
#                print "latency: %s" % self.data["latency"]
                
                NtopSessionParser.session_data.append(self.data);
                self.data = {}
                
            self.col = self.col + 1

 

