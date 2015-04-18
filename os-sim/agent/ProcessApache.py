import re
import string
import sys
import time

import Parser

class ProcessApache(Parser.Parser):

    def process(self):
        
        if self.plugin["log_type"] == 'syslog':
            self.__processSyslog()
            
        else:
            print "log type " + self.plugin["log_type"] +\
                  " unknown for Apache..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing Apache (syslog)...'
        
        pattern = '(\S+) (\S+) (\S+) \[(\d\d)\/(\w\w\w)\/\d\d\d\d:(\d\d):(\d\d).+"(.+)" (\d+) (\S+)'
            
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
                    (source, user, authuser, day, monthmmm, hour, minute, 
                     request, result, size) = result[0]

                    # TODO: adjust priority depending of the result ?

                    self.sendMessage(type     = 'apache',
                                     date     = monthmmm +' '+ day,
                                     sensor   = '',
                                     plugin   = 'to_define',
                                     tplugin  = result,
                                     priority = 1,
                                     protocol = 'TCP',
                                     src_ip   = source,
                                     src_port = '',
                                     dst_ip   = '127.0.0.1', # TODO !!
                                     dst_port = 80)          # TODO !!

                except IndexError: 
                    pass
        fd.close()

