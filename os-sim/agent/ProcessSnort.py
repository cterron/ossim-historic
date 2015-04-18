import re
import string
import sys
import time

import Parser

class ProcessSnort(Parser.Parser):

    def process(self):
        
        if self.plugin["log_type"] == 'syslog':
            self.__processSyslog()
        
        elif self.plugin["log_type"] == 'fast':
            self.__processFast()
            
        else:
            print "log type " + self.plugin["log_type"] +\
                  " unknown for  snort..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing snort (syslog)...'

        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+snort:\s+\[(\d+):(\d+):\d+\].*?{(\w+)}\s+([\d\.]+):?(\d+)?\s+.*\s+([\d\.]+):?(\d+)?'
        pattern2 = '\[Priority:\s+(\d+)\]'
        
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
                result2 = re.findall(str(pattern2), line)
                try:
                    priority = result2[0]
                except IndexError:
                    priority = 1

                try: 
                    (month, day, date, sensor, plugin, tplugin, protocol,
                    src_ip, src_port, dst_ip, dst_port)  = result[0]
                    self.sendMessage(type     = 'snort',
                                     date     = month+' '+day+' '+date,
                                     sensor   = sensor,
                                     plugin   = plugin,
                                     tplugin  = tplugin,
                                     priority = priority,
                                     protocol = protocol,
                                     src_ip   = src_ip,
                                     src_port = src_port,
                                     dst_ip   = dst_ip,
                                     dst_port = dst_port)
 
                except IndexError: 
                    pass
        fd.close()
        
    def __processFast(self):
        
        print 'processing snort (fast)...'
        
        patternl1 = '^(\d+)/(\d+)/\d+-(\d\d:\d\d:\d\d).*{(\w+)}\s+([\d\.]+):?(\d+)?\s+..\s+([\d\.]+):?(\d+)?'
        patternl2 = '\[(\d+):(\d+):\d+\]'
        patternl3 = '\[Priority:\s+(\d+)\]'
            
        location = self.plugin["log_location"]
        fd = open(location, 'r')

        # Move to the end of file
        fd.seek(0, 2)

        while 1:
            where = fd.tell()
            
            line1 = fd.readline()
            line2 = fd.readline()
            line3 = fd.readline()

            if not line1 or not line2 or not line3: # EOF reached
                time.sleep(1)
                fd.seek(where)

            resultl1 = re.findall(str(patternl1), line1)
            resultl2 = re.findall(str(patternl2), line2)
            resultl3 = re.findall(str(patternl3), line3)
            try:
                (month, day, date, protocol, 
                src_ip, src_port, dst_ip, dst_port) = resultl1[0]
                (plugin, tplugin) = resultl2[0]
                (priority) = resultl3[0]
                self.sendMessage(type     = 'snort',
                                 date     = month +' '+ day +' '+ date,
                                 sensor   = '',
                                 plugin   = plugin,
                                 tplugin  = tplugin,
                                 priority = priority,
                                 protocol = protocol,
                                 src_ip   = src_ip,
                                 src_port = src_port,
                                 dst_ip   = dst_ip,
                                 dst_port = dst_port)
 
            except IndexError: 
                pass

            # ignore lines, jump to next delimiter
            while 1:
                line = fd.readline()
                if not line:
                    time.sleep(1)
                    fd.seek(where)
                if line.startswith('-----'): break

        fd.close()
        
        

