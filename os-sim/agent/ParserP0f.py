import re
import sys
import time

import Parser
import util

class ParserP0f(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for P0f...", '!!', 'RED')
            sys.exit()

    def __processSyslog(self):
        
        util.debug ('ParserP0f', 'plugin started (syslog)...', '--')

        os_hash = {}

        pattern = '<\S+ (\S+) (\S+) (\d+):(\d+):(\d+) (\S+)> (\d+\.\d+\.\d+\.\d+):\d+ - ([^\(]*)'
            
        location = self.plugin["location"]
        fd = open(location, 'r')
            
        # Move to the end of file
        fd.seek(0, 2)
            
        while 1:

            if self.plugin["enable"] == 'no':

                # plugin disabled, wait for enabled
                util.debug (__name__, 'plugin disabled', '**', 'RED')
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
                try: 
                    (monthmmm, day, hour,
                     minute, second, year, source, os) = result[0]

                    os = util.normalizeWhitespace(os)
                    prev = ''
                    
                    try:
                        prev = os_hash[source]
                    except KeyError:
                        os_hash[source] = os

                    # os change !
                    if os != prev and \
                       (not os.__contains__('UNKNOWN')) and \
                       (not os.__contains__('NMAP')):

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
                                         plugin_sid = 1,
                                         priority   = 1,
                                         protocol   = 'TCP',
                                         src_ip     = source,
                                         src_port   = '',
                                         dst_ip     = '',
                                         dst_port   = '',
                                         data       = os)

                except IndexError: 
                    pass
        fd.close()

