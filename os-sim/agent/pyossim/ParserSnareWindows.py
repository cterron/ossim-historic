import re, sys, time

import Parser
import util

class ParserSnareWindows(Parser.Parser):

    plugin_id = 1518

    def process(self):

        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for SnareWindows...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserSnareWindows', 'plugin started (syslog)...', '--')

        pattern = re.compile("\w+\s+\d{1,2}\s+\d\d:\d\d:\d\d\s+(\d+\.\d+\.\d+\.\d+)\s+\S+\s+MSWinEventLog\s+(\d)\s+\S+\s+\d+\s+\S+\s+(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+(\d+)\s+(\d+)")

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
                result = pattern.search(line)
                if result is not None:

                    (ip, priority, month, day, datetime, year, sid) = \
                        result.groups()

                    datestring = "%s %s %s %s" % (year, month, day, datetime)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H:%M:%S"))

                    self.agent.sendAlert  (
                                     type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = ParserSnareWindows.plugin_id,
                                     plugin_sid = sid,
                                     priority   = int(priority) + 1,
                                     protocol   = '',
                                     src_ip     = ip,
                                     src_port   = '',
                                     dst_ip     = '',
                                     dst_port   = '',
                                     data       = '',
                                     log        = line)

        fd.close()

