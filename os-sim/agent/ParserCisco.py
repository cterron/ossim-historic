import re
import sys
import time

import Parser
import util

class ParserCisco(Parser.Parser):

    Cisco_sids = {
        'RCMD-4-RSHPORTATTEMPT': '1',   # Attempted to connect to RSHELL
        'CLEAR-5-COUNTERS': '2',        # Clear counter on all interfaces
        'LINEPROTO-5-UPDOWN': '3',      # Line protocol changed state
    }

    def process(self):

        if self.plugin["source"] == 'common':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Cisco...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserCisco', 'plugin started (syslog)...', '--')
        
        pattern = '\S+\s+\S+\s+\S+ (\S+) \S+: (\S+)\s+(\S+)\s+(\d+):(\d+):(\d+):\s*%([^:]*).*?(\d+\.\d+\.\d+\.\d+)?'
            
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
                    (sensor, monthmmm, day, hour, minute, second, \
                     sid, src) = result[0]
                    
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s %s %s" % \
                        (year, monthmmm, day, hour, minute, second)
                    
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))

                    self.agent.sendMessage(type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = ParserCisco.Cisco_sids[sid],
                                     priority   = 1,
                                     protocol   = '',
                                     src_ip     = src,
                                     src_port   = '',
                                     dst_ip     = sensor,
                                     dst_port   = '')

                except IndexError: 
                    pass
                except KeyError:
                    util.debug (__name__, 'Unknown plugin sid (%s)' %
                                sid, '**', 'RED')
        fd.close()

