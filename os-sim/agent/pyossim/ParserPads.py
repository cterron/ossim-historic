import re
import sys
import time
import os

import Parser
import util

class ParserPads(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'csv':
            while 1: self.__processCSV()

        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Pads...", '!!', 'RED')
            sys.exit()

    def __processCSV(self):
        
        util.debug ('ParserPads', 'plugin started (csv)...', '--')

        start_time = time.time()

        pattern = '^([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),(\d+)$'
            
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
                    (source, port, proto, service, application, epoch_time) = result[0]
                    epoch_time=float(epoch_time)

                    application = util.normalizeWhitespace(application)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                    time.gmtime(epoch_time))

                    self.agent.sendService (
                        host        = source,
                        port        = port,
                        proto       = proto,
                        service     = service,
                        application = application,
                        date        = date,
                        plugin_id   = self.plugin["id"],
                        plugin_sid  = 1,
                        log         = line)

                except IndexError: 
                    pass
        fd.close()
