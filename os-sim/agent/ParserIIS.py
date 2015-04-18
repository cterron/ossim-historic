import re
import sys
import time

import Parser
import util

class ParserIIS(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for IIS...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        pattern = '(\d\d\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d):(\d\d)\S+ (\S+) (\S+) (\S+) (\d+) (\w+) (\S+) \S+ (\d+)'
            
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
                    (year, month, day, hour, minute, second, source, user, 
                     to, port, method, document, result) = result[0]

                    datestring = "%s %s %s %s %s %s" % \
                        (year, month, day, hour, minute, second)
                    
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))

                    # TODO: adjust priority depending of the result ?
                    
                    self.agent.sendMessage(type     = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = result,
                                     priority   = 1,
                                     protocol   = 'TCP',
                                     src_ip     = source,
                                     src_port   = '',
                                     dst_ip     = to,
                                     dst_port   = port)
                    
                except IndexError: 
                    pass
        fd.close()

