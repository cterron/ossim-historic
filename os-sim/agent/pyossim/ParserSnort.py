import re
import sys
import time

import Parser
import util

class ParserSnort(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
        
        elif self.plugin["source"] == 'fast':
            while 1: self.__processFast()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for  snort...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        start_time = time.time()

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
                
                # restart plugin every hour
                if self.agent.plugin_restart_enable:
                    current_time = time.time()
                    if start_time + \
                       self.agent.plugin_restart_interval < current_time:
                        util.debug(__name__, 
                                   "Restarting plugin..", '->', 'YELLOW')
                        fd.close()
                        start_time = current_time
                        return None

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
                                    dst_port    = dst_port,
                                    log         = line)
 
                except IndexError: 
                    pass
        fd.close()
        
        
    def __processFast(self):
        
        util.debug (__name__, 'plugin started (fast)...', '--')
        
        start_time = time.time()
 
        patternl1 = re.compile('^(\d+)/(\d+)-(\d\d:\d\d:\d\d).*{(\w+)}\s+([\d\.]+):?(\d+)?\s+..\s+([\d\.]+):?(\d+)?')
        patternl2 = re.compile('\[(\d+):(\d+):\d+\]')
        patternl3 = re.compile('\[Priority:\s+(\d+)\]')
        patternl4 = re.compile('\[(\d+):(\d+)\]$')
            
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

                # restart plugin every hour
                if self.agent.plugin_restart_enable:
                    current_time = time.time()
                    if start_time + \
                       self.agent.plugin_restart_interval < current_time:
                        util.debug(__name__, 
                                   "Restarting plugin..", '->', 'YELLOW')
                        fd.close()
                        start_time = current_time
                        return None

            else:
                
                result1 = patternl1.search(line)
                result2 = patternl2.search(line)
                result3 = patternl3.search(line)
                result4 = patternl4.search(line)

                if result2 is not None:
                    (plugin, tplugin) = result2.groups() 

                if result3 is not None:
                    priority = result3.groups()[0]
                else:
                    priority = 3

                if result4 is not None:
                    (sid, cid) = result4.groups()
                else:
                    sid = cid = ""
                
                if result1 is not None:

                    (month, day, date, protocol, 
                     src_ip, src_port, dst_ip, dst_port) = result1.groups()

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
                                     snort_sid  = sid,
                                     log        = line)

        fd.close()

