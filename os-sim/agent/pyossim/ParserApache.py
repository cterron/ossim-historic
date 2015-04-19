import re
import sys
import time

import Parser
import util

class ParserApache(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'common':
            while 1: self.__processCommon()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Apache...", '!!', 'YELLOW')
            sys.exit()


    def __processCommon(self):
        
        util.debug ('ParserApache', 'plugin started (common)...', '--')

        start_time = time.time()
        
        pattern = '(\S+) (\S+) (\S+) \[(\d\d)\/(\w\w\w)\/(\d\d\d\d):(\d\d):(\d\d):(\d\d).+"(.+)" (\d+) (\S+)'
            
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
                try: 
                    (source, user, authuser, day, monthmmm, year, hour,
                     minute, second, request, code, size) = result[0]

                    datestring = "%s %s %s %s %s %s" % \
                        (year, monthmmm, day, hour, minute, second)
                    
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))

                    # TODO: adjust priority depending of the result ?
                    self.agent.sendEvent (type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = code,
                                     priority   = 1,
                                     protocol   = 'TCP',
                                     src_ip     = source,
                                     src_port   = '',
                                     dst_ip     = '127.0.0.1', # TODO !!
                                     dst_port   = 80,          # TODO !!
                                     log        = line)

                except IndexError: 
                    pass
        fd.close()

