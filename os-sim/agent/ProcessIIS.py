import re
import string
import sys
import time

import Parser

class ProcessIIS(Parser.Parser):

    def process(self):
        
        if self.plugin["log_type"] == 'syslog':
            self.__processSyslog()
            
        else:
            print "log type " + self.plugin["log_type"] +\
                  " unknown for IIS..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing IIS (syslog)...'
        
        pattern = '\d\d\d\d-(\d\d)-(\d\d) (\d\d):(\d\d)\S+ (\S+) (\S+) (\S+) (\d+) (\w+) (\S+) \S+ (\d+)'
            
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
                    (month, day, hour, minute, source, user, 
                     to, port, method, document, result) = result[0]

                    # TODO: adjust priority depending of the result ?
                    
                    self.sendMessage(type     = 'iis',
                                     date     = month+' '+day+' '+\
                                                hour+':'+minute,
                                     sensor   = '',
                                     plugin   = 'to_define',
                                     tplugin  = result,
                                     priority = 1,
                                     protocol = 'TCP',
                                     src_ip   = source,
                                     src_port = '',
                                     dst_ip   = to,
                                     dst_port = port)
                    
                except IndexError: 
                    pass
        fd.close()

