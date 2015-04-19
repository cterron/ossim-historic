import re
import sys
import time
import os

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

        pattern = '<\S+\s+(\S+)\s+(\S+)\s+(\d+):(\d+):(\d+)\s+(\S+)>\s+(\d+\.\d+\.\d+\.\d+):\d+\s+-\s+([^\(]*)'
            
        location = self.plugin["location"]

        # first check if file exists
        if not os.path.exists(location):
            fd = open(location, "w")
            fd.close()

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
                try: 
                    (monthmmm, day, hour,
                     minute, second, year, source, ops) = result[0]

                    ops = util.normalizeWhitespace(ops)
                    prev = os_hash.get(source, '')

                    # os change !
                    if ops != prev and \
                       (not ops.__contains__('UNKNOWN')) and \
                       (not ops.__contains__('NMAP')):

                        os_hash[source] = ops

                        datestring = "%s %s %s %s %s %s" % \
                            (year, monthmmm, day, hour, minute, second)
                        
                        date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                             time.strptime(datestring, 
                                                           "%Y %b %d %H %M %S"))

                        self.agent.sendOsChange (
                             host       = source,
                             os         = ops,
                             date       = date,
                             plugin_id  = self.plugin["id"],
                             plugin_sid = 1,
                             log        = line)

                except IndexError: 
                    pass
        fd.close()

