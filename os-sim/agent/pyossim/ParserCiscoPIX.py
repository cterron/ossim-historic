import re, sys, time

import Parser
import util

class ParserCiscoPIX(Parser.Parser):


    def process(self):

        if self.plugin["source"] == 'common':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Cisco...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserCiscoPIX', 'plugin started (syslog)...', '--')
        
        pattern = '(\S+) (\d+) (\d\d):(\d\d):(\d\d) \S+ %PIX-(\d)-(\d+):'
        
        pattern_from = 'from\s+\(?(\d{1,3}\.\d{1,3}\.\d{1,3}.\d{1,3})\)?'
        pattern_from2 = '(src|for)\s+(inside|outside)\s*:\s*(\d{1,3}\.\d{1,3}\.\d{1,3}.\d{1,3})\/(\d+)'
        
        pattern_to = 'to\s+\(?(\d{1,3}\.\d{1,3}\.\d{1,3}.\d{1,3})\)?'
        pattern_to2 = '(dst|to)\s+(inside|outside)\s*:\s*(\d{1,3}\.\d{1,3}\.\d{1,3}.\d{1,3})\/(\d+)'
            
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
                
                # no match
                if result == []: continue

                (monthmmm, day, hour, minute, second, priority, \
                 sid) = result[0]
                
                year = time.strftime('%Y', time.localtime(time.time()))
                datestring = "%s %s %s %s %s %s" % \
                    (year, monthmmm, day, hour, minute, second)
                
                date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.strptime(datestring, 
                                                   "%Y %b %d %H %M %S"))

                src_ip = dst_ip = src_port = dst_port = ''
                
                # source
                result_from = re.findall(str(pattern_from), line)
                if result_from != []:
                    (src_ip) = result_from[0]
                else:
                    result_from2 = re.findall(str(pattern_from2), line)
                    if result_from2 != []:
                        src_ip = result_from2[0][2]
                        src_port = result_from2[0][3]
                    
                # destination
                result_to = re.findall(str(pattern_to), line)
                if result_to != []:
                    (dst_ip) = result_to[0]
                else:
                    result_to2 = re.findall(str(pattern_to2), line)
                    if result_to2 != []:
                        dst_ip = result_to2[0][2]
                        dst_port = result_to2[0][3]

                self.agent.sendAlert (
                                type = 'detector',
                                 date       = date,
                                 sensor     = self.plugin["sensor"],
                                 interface  = self.plugin["interface"],
                                 plugin_id  = self.plugin["id"],
                                 plugin_sid = sid,
                                 priority   = priority,
                                 protocol   = '',
                                 src_ip     = src_ip,
                                 src_port   = src_port,
                                 dst_ip     = dst_ip,
                                 dst_port   = dst_port,
                                 log        = line)
        fd.close()

