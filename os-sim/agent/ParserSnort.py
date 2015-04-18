import re
import sys
import time

import Parser
import util

class ParserSnort(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
        
        elif self.plugin["source"] == 'fast':
            self.__processFast()
            
        else:
            print "log type " + self.plugin["source"] +\
                  " unknown for  snort..."
            sys.exit()


    def __processSyslog(self):
        
        print 'processing snort (syslog)...'
        util.debug (__name__, 'plugin started (syslog)...', '--')

        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+snort:\s+\[(\d+):(\d+):\d+\].*?{(\w+)}\s+([\d\.]+):?(\d+)?\s+.*\s+([\d\.]+):?(\d+)?'
        pattern2 = '\[Priority:\s+(\d+)\]'
        
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
                result2 = re.findall(str(pattern2), line)
                try:
                    priority = result2[0]
                except IndexError:
                    priority = 1

                try: 
                    (month, day, datetime, sensor, plugin, tplugin, protocol,
                    src_ip, src_port, dst_ip, dst_port)  = result[0]

                    # provisional! 
                    # TODO: read date from log
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.localtime(time.time()))
                    
                    self.agent.sendMessage(
                                    type        = 'detector',
                                    date        = date,
                                    sensor      = sensor,
                                    plugin_id   = int(plugin) + 1000,
                                    plugin_sid  = tplugin,
                                    priority    = priority,
                                    protocol    = protocol,
                                    src_ip      = src_ip,
                                    src_port    = src_port,
                                    dst_ip      = dst_ip,
                                    dst_port    = dst_port)
 
                except IndexError: 
                    pass
        fd.close()
        
        
    def __processFast(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
 
        patternl1 = '^(\d+)/(\d+)-(\d\d:\d\d:\d\d).*{(\w+)}\s+([\d\.]+):?(\d+)?\s+..\s+([\d\.]+):?(\d+)?'
        patternl2 = '\[(\d+):(\d+):\d+\]'
        patternl3 = '\[Priority:\s+(\d+)\]'
            
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

                result1 = re.findall(str(patternl1), line)
                result2 = re.findall(str(patternl2), line)
                result3 = re.findall(str(patternl3), line)
                try:
                    priority = result3[0]
                except IndexError:
                    priority = 1
                try:
                    (month, day, date, protocol, 
                     src_ip, src_port, dst_ip, dst_port) = result1[0]
                    (plugin, tplugin) = result2[0]
                    year = time.strftime('%Y', time.localtime(time.time()))
                    date = year + '-' + month + '-' + day + ' ' + date
                    self.agent.sendMessage(type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = int(plugin) + 1000,
                                     plugin_sid = tplugin,
                                     priority   = priority,
                                     protocol   = protocol,
                                     src_ip     = src_ip,
                                     src_port   = src_port,
                                     dst_ip     = dst_ip,
                                     dst_port   = dst_port)
     
                except IndexError: 
                    pass

        fd.close()

