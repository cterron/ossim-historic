import re
import string
import sys
import time

import Parser

class ProcessFW1(Parser.Parser):

    def process(self):
        
        if self.plugin["log_type"] == 'syslog':
            self.__processSyslog()
            
        else:
            print "log type " + self.plugin["log_type"] +\
                  " unknown for  FireWall-1..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing Firewall-1 (syslog)...'
        
        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+logger:.*src:\s(\d+.\d+.\d+.\d+).*s_port:\s(\d+).*dst:\s+(\d+.\d+.\d+.\d+).*service:\s(\w+).*proto:\s+(\w+)'
            
        location = self.plugin["log_location"]
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
                    (month, day, date, originator, 
                    src, s_port, dst, service, proto) = result[0]
                    
                    self.sendMessage(type     = 'fw-1',
                                     date     = month +' '+ day+' '+date,
                                     sensor   = originator,
                                     plugin   = '',
                                     tplugin  = '',
                                     priority = '',
                                     protocol = proto,
                                     src_ip   = src,
                                     src_port = s_port,
                                     dst_ip   = dst,
                                     dst_port = service)

                except IndexError: 
                    pass
        fd.close()

