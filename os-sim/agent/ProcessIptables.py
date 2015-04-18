import re
import string
import sys
import time

import Parser

class ProcessIptables(Parser.Parser):

    def process(self):
        
        if self.plugin["log_type"] == 'syslog':
            self.__processSyslog()
            
        else:
            print "log type " + self.plugin["log_type"] +\
                  " unknown for iptables..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing iptables (syslog)...'
        
        pattern1 = '(\S+)\s+(\d+)\s+(\d\d):(\d\d):\d\d\s+(\S*) (\S*):'
        pattern2 = 'IN=(\S*) OUT=(\S*) \S+ SRC=(\S+) DST=(\S+) LEN=(\d+) \S+ \S+ TTL=(\d+) .* PROTO=(\S*) SPT=(\d*) DPT=(\d*) WINDOW=(\d*)'

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
                result1 = re.findall(str(pattern1), line)
                result2 = re.findall(str(pattern2), line)
                try: 
                    (monthmmm, day, hour, minute, server, 
                     sourcewpid) = result1[0]
                    (in_, out, source, destination, size, ttl, protocol,
                    sourceport, destport, window) = result2[0]

                    self.sendMessage(type     = 'iptables',
                                     date     = monthmmm+' '+day+' '+\
                                                hour+':'+minute,
                                     sensor   = 'server',
                                     plugin   = 'to_define',
                                     tplugin  = '',
                                     priority = 1,
                                     protocol = protocol,
                                     src_ip   = source,
                                     src_port = sourceport,
                                     dst_ip   = destination,
                                     dst_port = destport)
                    
                except IndexError: 
                    pass
        fd.close()

