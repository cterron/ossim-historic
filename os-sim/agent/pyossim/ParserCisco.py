import re
import sys
import time

import Parser
import util

class ParserCisco(Parser.Parser):

    #
    # example: 
    # %LINK-5-CHANGED: Interface Serial3/3, ...
    #       ^
    # severity-level:
    # {0 | emergencies} System is unusable
    # {1 | alerts}Immediate action needed
    # {2 | critical}Critical conditions
    # {3 | errors}Error conditions
    # {4 | warnings}Warning conditions
    # {5 | notifications}Normal but significant conditions
    # {6 | informational}Informational messages
    # {7 | debugging} Debugging messages 
    #

    Cisco_sids = {
        'RCMD-4-RSHPORTATTEMPT': '1',   # Attempted to connect to RSHELL
        'CLEAR-5-COUNTERS':      '2',   # Clear counter on all interfaces
        'LINEPROTO-5-UPDOWN':    '3',   # Line protocol changed state
    }
    

    def process(self):

        if self.plugin["source"] == 'common':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Cisco...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserCisco', 'plugin started (syslog)...', '--')

        start_time = time.time()
        
        pattern = re.compile('(\w+)\s+(\d{1,2})\s+(\d\d:\d\d:\d\d)\s+(\S+)\s+\S+:[^%]*%(\w+-(\d)-\w+)')

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

                    (month, day, datetime, sensor, sid, severity) = \
                        result.groups()
        
                    # date
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s" % (year, month, day, datetime)
                    date = time.strftime('%Y-%m-%d %H:%M:%S',
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H:%M:%S"))
                    # priority
                    priority = 1
                    if severity in ("0", "1", "2"):
                        priority = 3
                    elif severity in ("3", "4"):
                        priority = 2

                    # src_ip and dst_ip

                    try:
                        self.agent.sendEvent (
                            type       = 'detector',
                            date       = date,
                            sensor     = self.plugin["sensor"],
                            interface  = self.plugin["interface"],
                            plugin_id  = self.plugin["id"],
                            plugin_sid = ParserCisco.Cisco_sids[sid],
                            priority   = priority,
                            protocol   = '',
                            src_ip     = '',
                            src_port   = '',
                            dst_ip     = '',
                            dst_port   = '',
                            log        = line)
                    except KeyError:
                        util.debug (__name__, 'Unknown plugin sid (%s)' %
                                    sid, '**', 'YELLOW')


        fd.close()

