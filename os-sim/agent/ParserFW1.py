import re
import sys
import time

import Parser
import util

class ParserFW1(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            print "log type " + self.plugin["source"] +\
                  " unknown for  FireWall-1..."
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+logger:.*src:\s(\d+.\d+.\d+.\d+).*s_port:\s(\d+).*dst:\s+(\d+.\d+.\d+.\d+).*service:\s(\w+).*proto:\s+(\w+)'
            
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
                try: 
                    (month, day, datetime, originator, 
                    src, s_port, dst, service, proto) = result[0]
                    
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s" % (year, month, day, datetime)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H:%M:%S"))
                    
                    self.agent.sendMessage(type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = '',
                                     priority   = '',
                                     protocol   = proto,
                                     src_ip     = src,
                                     src_port   = s_port,
                                     dst_ip     = dst,
                                     dst_port   = service)

                except IndexError: 
                    pass
        fd.close()

