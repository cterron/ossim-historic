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
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for  snort...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')

        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+snort:\s+\[(\d+):(\d+):\d+\].*?{(\w+)}\s+([\d\.]+):?(\d+)?\s+.*\s+([\d\.]+):?(\d+)?'
        pattern2 = '\[Priority:\s+(\d+)\]'
        
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
                result2 = re.findall(str(pattern2), line)
                try:
                    priority = result2[0]
                except IndexError:
                    priority = 3

                try: 
                    (month, day, datetime, sensor, plugin, tplugin, protocol,
                    src_ip, src_port, dst_ip, dst_port)  = result[0]

                    # provisional! 
                    # TODO: read date from log
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.localtime(time.time()))
                    
                    self.agent.sendAlert  (
                                    type        = 'detector',
                                    date        = date,
                                    sensor      = sensor,
                                    plugin_id   = int(plugin) + 1000,
                                    plugin_sid  = tplugin,
                                    priority    = abs(4 - int(priority)),
                                    protocol    = protocol,
                                    src_ip      = src_ip,
                                    src_port    = src_port,
                                    dst_ip      = dst_ip,
                                    dst_port    = dst_port)
 
                except IndexError: 
                    pass
        fd.close()
        
        
    def __processFast(self):
        
        util.debug (__name__, 'plugin started (fast)...', '--')
 
        patternl1 = '^(\d+)/(\d+)-(\d\d:\d\d:\d\d).*{(\w+)}\s+([\d\.]+):?(\d+)?\s+..\s+([\d\.]+):?(\d+)?'
        patternl2 = '\[(\d+):(\d+):\d+\]'
        patternl3 = '\[Priority:\s+(\d+)\]'
        patternl4 = '\[(\d+):(\d+)\]$'
            
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

                result1 = re.findall(str(patternl1), line)
                result2 = re.findall(str(patternl2), line)
                result3 = re.findall(str(patternl3), line)
                result4 = re.findall(str(patternl4), line)
                
                if result3 != []:
                    priority = result3[0]
                else:
                    priority = 3

                if result4 != []:
                    (sid, cid) = result4[0]
                else:
                    sid = cid = ""
                
                try:
                    (month, day, date, protocol, 
                     src_ip, src_port, dst_ip, dst_port) = result1[0]
                    (plugin, tplugin) = result2[0]
                    year = time.strftime('%Y', time.localtime(time.time()))
                    date = year + '-' + month + '-' + day + ' ' + date
                    self.agent.sendAlert  (type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = int(plugin) + 1000,
                                     plugin_sid = tplugin,
                                     priority   = abs(4 - int(priority)),
                                     protocol   = protocol,
                                     src_ip     = src_ip,
                                     src_port   = src_port,
                                     dst_ip     = dst_ip,
                                     dst_port   = dst_port,
                                     snort_cid  = cid,
                                     snort_sid  = sid)
     
                except IndexError: 
                    pass

        fd.close()

