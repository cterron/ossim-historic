import re
import sys
import time

import Parser
import util

class ParserIptables(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for iptables...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')
        
        start_time = time.time()

        pattern1 = '(\S+)\s+(\d+)\s+(\d\d):(\d\d):(\d\d)\s+(\S*) (\S*):'
        pattern2 = '(\S+)\s+IN=(\S*) OUT=(\S*) \S+ SRC=(\S+) DST=(\S+) LEN=(\d+) \S+ \S+ TTL=(\d+) .*? PROTO=(\S*) SPT=(\d*) DPT=(\d*)'

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
                result1 = re.findall(str(pattern1), line)
                result2 = re.findall(str(pattern2), line)
                try: 
                    (monthmmm, day, hour, minute, second, server, 
                     sourcewpid) = result1[0]
                    (action, in_, out, source, destination, size, ttl, protocol,
                     sourceport, destport) = result2[0]

                    # get date
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s %s %s" % \
                        (year, monthmmm, day, hour, minute, second)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))
                    
                    # action (ACCEPT | REJECT | DROP | DENY) -> plugin_sid
                    if action == 'ACCEPT':
                        plugin_sid = 1
                    elif action == 'REJECT':
                        plugin_sid = 2
                    elif action == 'DROP' or action == 'DENY':
                        plugin_sid = 3
                    elif action == 'Inbound':
                        plugin_sid = 4
                    elif action == 'Outbound':
                        plugin_sid = 5
                    else:
                        plugin_sid = 1 # ??

                    self.agent.sendEvent  (type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = self.plugin["id"],
                                     plugin_sid = plugin_sid,
                                     priority   = 1,
                                     protocol   = protocol,
                                     src_ip     = source,
                                     src_port   = sourceport,
                                     dst_ip     = destination,
                                     dst_port   = destport,
                                     log        = line)
                    
                except IndexError: 
                    pass
        fd.close()

