import re
import sys
import time

import Parser
import util

class ParserFW1(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        elif self.plugin["source"] == 'opsec':
            self.__processOpsec()

        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for  FireWall-1...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+logger:.*src:\s(\d+.\d+.\d+.\d+).*s_port:\s(\d+).*dst:\s+(\d+.\d+.\d+.\d+).*service:\s(\w+).*proto:\s+(\w+)'
            
        location = self.plugin["location"]
        try:
            fd = open(location, 'r')
        except IOError, e:
            util.debug(__name__, e, '!!', 'RED')
            sys.exit()
            
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
            else:
                result = re.findall(str(pattern), line)
                try: 
                    (month, day, datetime, originator, 
                    src, s_port, dst, service, proto) = result[0]
                    
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s" % (year, month, day, datetime)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H:%M:%S"))
                    
                    # action (ACCEPT | REJECT | DROP | DENY) -> plugin_sid
                    if action == 'accept':
                        plugin_sid = 1
                    elif action == 'reject':
                        plugin_sid = 2
                    elif action == 'drop' or action == 'deny':
                        plugin_sid = 3
 
                    self.agent.sendAlert  (type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = plugin_sid,
                                     priority   = '',
                                     protocol   = proto,
                                     src_ip     = src,
                                     src_port   = s_port,
                                     dst_ip     = dst,
                                     dst_port   = service)

                except IndexError: 
                    pass
        fd.close()

    def __processOpsec(self):
        
        util.debug (__name__, 'plugin started (opsec)...', '--')
        
        pattern = "loc=.*" +\
            "time=\s*(\d{1,2})(\w+)(\d{4})\s+(\d\d):(\d\d):(\d\d)\|" +\
            "action=([^\|]*)\|orig=([^\|]*)\|"
        patternIface = "\|i/f_name=([^\|]*)\|"
        patternIps = "src=([^\|]*)\|(?:s_port=([^\|]*)\|)?" +\
            "dst=([^\|]*)\|(?:service=([^\|]*)\|)?"
        patternSport = "s_port=([^\|]*)"
        patternProto = "proto=([^\|]*)\|"
        
        location = self.plugin["location"]
        fd = open(location, 'r')
            
        # Move to the end of file
        fd.seek(0, 2)
            
        while 1:
            where = fd.tell()
            line = fd.readline()
            if not line: # EOF reached
                time.sleep(1)
                fd.seek(where)
            else:
                result = re.findall(str(pattern), line)
                resultIface = re.findall(str(patternIface), line)
                resultIps = re.findall(str(patternIps), line)
                resultSport = re.findall(str(patternSport), line)
                resultProto = re.findall(str(patternProto), line)
                
                try: 
                    interface = resultIface[0]
                except IndexError:
                    pass
                
                try:
                    (src, s_port, dst, service) = resultIps[0]
                except IndexError:
                    pass

                try:
                    proto = resultProto[0]
                except IndexError:
                    pass

                try:
                    s_port = resultSport[0]
                except IndexError:
                    pass
                
                try: 
                    (day, month, year, hour, minute, second, action, 
                    originator) = result[0]
                   
                    datestring = "%s-%s-%s %s:%s:%s" % \
                        (year, month, day, hour, minute, second)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y-%b-%d %H:%M:%S"))
                    
                    # action (ACCEPT | REJECT | DROP | DENY) -> plugin_sid
                    if action == 'accept':
                        plugin_sid = 1
                    elif action == 'reject':
                        plugin_sid = 2
                    elif action == 'drop' or action == 'deny':
                        plugin_sid = 3
 
                    self.agent.sendAlert  (type = 'detector',
                                     date       = date,
                                     sensor     = originator,
                                     interface  = interface,
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = plugin_sid,
                                     priority   = '',
                                     protocol   = proto,
                                     src_ip     = src,
                                     src_port   = s_port,
                                     dst_ip     = dst,
                                     dst_port   = service)

                except IndexError: 
                    pass
        fd.close()

