#
#  Sample Skeleton for building OSSIM parsers
#
#  Replace Skel with the name of the tool you want to manage (s/Skel/Tool/g)
#

import re, sys, time

import Parser
import util

class ParserSkel(Parser.Parser):

    #
    # pattern for the following log sample:
    #   2004-11-28 16:45 id=5 src=192.168.2.69:3452 dst=192.168.2.97:21 
    #   proto=tcp priority=5
    #
    PATTERN = "(\d{4}-\d{2}-\d{2} \d{2}:\d{2}) id=(\d+) " +\
        "src=(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+) " +\
        "dst=(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+) " +\
        "proto=(\w+) priority=(\d+)"

    def process(self):

        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()

        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Skel...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):

        util.debug ('ParserSkel', 'plugin started (syslog)...', '--')

        start_time = time.time()

        pattern = re.compile(ParserSkel.PATTERN)

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

                    (date, sid, src, dst, sport, dport, proto, priority) = \
                        result.groups()

                    self.agent.sendAlert (
                            type       = 'detector',
                            date       = date,
                            sensor     = self.plugin["sensor"],
                            interface  = self.plugin["interface"],
                            plugin_id  = self.plugin["id"],
                            plugin_sid = sid,
                            priority   = priority,
                            protocol   = proto,
                            src_ip     = src,
                            src_port   = sport,
                            dst_ip     = dst,
                            dst_port   = dport,
                            log        = line)

        fd.close()

