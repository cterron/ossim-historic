import re
import sys
import time

import Parser
import util

class ParserArpwatch(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for  arpwatch...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')

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
                try:
                    # ip address
                    result = re.findall('ip address: (\S+)', line)
                    ip = result[0]
                    
                    # ethernet address
                    line = fd.readline()
                    result = re.findall('ethernet address: (.*)', line)
                    addr = result[0]
                    
                    # ethernet vendor
                    line = fd.readline()
                    result = re.findall('ethernet vendor: (.*)', line)
                    vendor = result[0]
                    
                    # timestamp
                    # Monday, March 15, 2004 15:39:19 +0000
                    line = fd.readline()
                    result = re.findall('timestamp: ([^\+|\-]*)', line)
                    timestamp = \
                        time.strptime(util.normalizeWhitespace(result[0]), 
                                      "%A, %B %d, %Y %H:%M:%S")
                    date = time.strftime('%Y-%m-%d %H:%M:%S', timestamp)

                                     
                    self.agent.sendMacChange (
                         host       = ip,
                         mac        = addr,
                         vendor     = vendor,
                         date       = date,
                         plugin_id  = self.plugin["id"],
                         plugin_sid = 1,
                         log        = line)

                    
                except IndexError:
                    pass

        fd.close()

