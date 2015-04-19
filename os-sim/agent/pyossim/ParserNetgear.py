import re, sys, time

import Parser
import util

class ParserNetgear(Parser.Parser):

    def __init__(self, agent, plugin):

        # example log:
        # Wed, 11/03/2004 19:45:46 - All ports forwarded - Source:192.168.1.10,
        # 27005, LAN - Destination:69.31.6.141, 27015, WAN
        self.PATTERN = "(\d{1,2}\/\d{1,2}\/\d{4})\s+" +\
            "(\d{1,2}:\d{2}:\d{2})\s+\-\s+" +\
            "(.*?) \-\s+" +\
            "Source:(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}),\s+(\d+)," +\
            "\s+\S+\s+\-\s+" +\
            "Destination:(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}),\s+(\d+)," +\
            "\s+\S+"

        self.msg2sid = [
            "All ports forwarded",

            "UDP packet forwarded",
            "SMTP forwarded",
            "HTTP forwarded",
            "HTTPS forwarded",
            
            "TCP connection dropped",
            "IP packet dropped",
            "UDP packet dropped",
            "ICMP packet dropped",
            
            "Successful administrator login",
            "Administrator login fail",
        ]

        Parser.Parser.__init__(self, agent, plugin)


    def process(self):

        if self.plugin["source"] == 'syslog':
            self.__processSyslog()

        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Netgear...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):

        util.debug ('ParserNetgear', 'plugin started (syslog)...', '--')

        pattern = re.compile(self.PATTERN)

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

                    (date, time_, msg, src, sport, dst, dport) = \
                        result.groups()
                    date = date.replace("/", "-")

                    try:
                        pass
                        self.agent.sendAlert (
                                type       = 'detector',
                                date       = date + " " + time_,
                                sensor     = self.plugin["sensor"],
                                interface  = self.plugin["interface"],
                                plugin_id  = self.plugin["id"],
                                plugin_sid = self.msg2sid.index(msg) + 1,
                                priority   = 1,
                                protocol   = "",
                                src_ip     = src,
                                src_port   = sport,
                                dst_ip     = dst,
                                dst_port   = dport,
                                log        = line)
                    except ValueError:
                        util.debug(__name__, "Unknown sid %s" % (msg),
                                   "**", "RED")


        fd.close()

