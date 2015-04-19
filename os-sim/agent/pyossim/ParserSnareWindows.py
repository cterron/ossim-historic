import re, sys, time, socket

import Parser
import util

class ParserSnareWindows(Parser.Parser):

    plugin_id = 1518

    def process(self):

        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for SnareWindows...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserSnareWindows', 'plugin started (syslog)...', '--')

        start_time = time.time()

        pattern = re.compile("\S+\s+\S+\s+\S+\s+(\S+).*?MSWinEventLog\s+(\d)\s+\S+\s+\d+\s+\S+\s+(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+(\d+)\s+(\d+)")

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
                result = pattern.search(line)
                if result is not None:

                    (src_ip, priority, 
                     month, day, datetime, year, sid) = result.groups()

                    datestring = "%s %s %s %s" % (year, month, day, datetime)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H:%M:%S"))

                    try:
                        src_ip = socket.gethostbyname(src_ip)
                    except Exception:
                        pass

                    self.agent.sendAlert  (
                                     type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = ParserSnareWindows.plugin_id,
                                     plugin_sid = sid,
                                     priority   = int(priority) + 1,
                                     protocol   = '',
                                     src_ip     = src_ip,
                                     src_port   = '',
                                     dst_ip     = '',
                                     dst_port   = '',
                                     data       = '',
                                     log        = line)

        fd.close()

