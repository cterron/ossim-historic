import re
import sys
import time

import Parser
import util

class ParserIIS(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()

        elif self.plugin["source"] == 'W3C':
            while 1: self.__processW3C()

        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for IIS...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        start_time = time.time()

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
                    (year, month, day, hour, minute, second, source, user, 
                     to, port, method, document, result) = result[0]

                    datestring = "%s %s %s %s %s %s" % \
                        (year, month, day, hour, minute, second)
                    
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))

                    # TODO: adjust priority depending of the result ?
                    
                    self.agent.sendAlert  (type     = 'detector',
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
                                     dst_port   = port,
                                     data       = line,
                                     log        = line)
                    
                except IndexError: 
                    pass
        fd.close()


    def __processW3C(self):

        util.debug ('ParserIIS', 'plugin started (W3C)...', '--')

        start_time = time.time()

        pattern = re.compile(
                "(\d{4}\-\d{2}\-\d{2} \d{2}:\d{2}:\d{2}) "  +\
                "(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}) "     +\
                "\S+ \S+ \- (\d+) \- "                      +\
                "(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}) "     +\
                ".*? (\d+) \d+ \d+"
            )

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

                    (date, src, dport, dst, sid) = result.groups()

                    self.agent.sendAlert (
                            type       = 'detector',
                            date       = date,
                            sensor     = self.plugin["sensor"],
                            interface  = self.plugin["interface"],
                            plugin_id  = self.plugin["id"],
                            plugin_sid = sid,
                            priority   = 1,
                            protocol   = 'TCP',
                            src_ip     = src,
                            src_port   = '',
                            dst_ip     = dst,
                            dst_port   = dport,
                            log        = line)

        fd.close()

