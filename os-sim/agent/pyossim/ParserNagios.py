import re, sys, time, socket

import Parser
import util

class ParserNagios(Parser.Parser):

    # Feb  7 15:31:41 golgotha ....
    header_nagios =  "\[(?P<time>\d+)\]"
    header_syslog = "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*)"

    sids = [
        {
            "sid":      1,
            "name":     "nagios: host alert - hard down",
            # [1170053208] HOST ALERT:
            # hostname;DOWN;HARD;10;CRITICAL - Plugin timed out
            # after 10 seconds
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?DOWN;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      2,
            "name":     "nagios: host alert - hard up",
            # [1169977338] HOST ALERT: hostname;UP;HARD;1;PING OK -
            # Packet loss = 0%, RTA = 10.52 ms
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?UP;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      3,
            "name":     "nagios: host alert - hard unreachable",
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?UNREACHABLE;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      4,
            "name":     "nagios: host alert - soft down",
            # [1170053118] HOST ALERT:
            # hostname;DOWN;SOFT;1;CRITICAL - Plugin timed out
            # after 10 seconds
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?DOWN;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      5,
            "name":     "nagios: host alert - soft up",
            # [1170102444] HOST ALERT: hostname;UP;SOFT;2;PING OK -
            # Packet loss = 90%, RTA = 2004.46 ms
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?UP;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      6,
            "name":     "nagios: host alert - soft unreachable",
            "pattern":
                    ".*?HOST ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?UNREACHABLE;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      7,
            "name":     "nagios: service alert - hard critical",
            # [1169971419] SERVICE ALERT: hostname;PING
            # Sondas;CRITICAL;HARD;10;PING CRITICAL - Packet loss = 0%, RTA =
            # 958.63 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?CRITICAL;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      8,
            "name":     "nagios: service alert - hard ok",
            # [1169962908] SERVICE ALERT: hostname;PING
            # Sondas;OK;HARD;10;PING OK - Packet loss = 0%, RTA = 23.89 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?OK;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      9,
            "name":     "nagios: service alert - hard unknown",
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?UNKNOWN;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      10,
            "name":     "nagios: service alert - hard warning",
            # [1169962728] SERVICE ALERT: hostname;PING
            # Sondas;WARNING;HARD;10;PING WARNING - Packet loss = 0%, RTA =
            # 154.87 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?WARNING;" +\
                    ".*?HARD;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      11,
            "name":     "nagios: service alert - soft critical",
            # [1169971419] SERVICE ALERT: hostname;PING
            # Sondas;CRITICAL;SOFT;1;PING CRITICAL - Packet loss = 0%, RTA =
            # 958.63 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?CRITICAL;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      12,
            "name":     "nagios: service alert - soft ok",
            # [1169962908] SERVICE ALERT: hostname;PING
            # Sondas;OK;SOFT;4;PING OK - Packet loss = 0%, RTA = 23.89 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?OK;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      13,
            "name":     "nagios: service alert - soft unknown",
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?UNKNOWN;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
        {
            "sid":      14,
            "name":     "nagios: service alert - soft warning",
            # [1169962728] SERVICE ALERT: hostname;PING
            # Sondas;WARNING;SOFT;1;PING WARNING - Packet loss = 0%, RTA =
            # 154.87 ms
            "pattern":
                    ".*?SERVICE ALERT:" +\
                    ".*?(?P<src>\S+);" +\
                    ".*?(?P<service>[^;]+);" +\
                    ".*?WARNING;" +\
                    ".*?SOFT;" +\
                    ".*?(?P<tries>\d+);"
        },
    ]

    def process(self):

        if self.plugin["source"] == 'nagios':
            while 1: self.__processCommon(ParserNagios.header_nagios)
        elif self.plugin["source"] == 'syslog':
            while 1: self.__processCommon(ParserNagios.header_syslog)
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Nagios...", '!!', 'YELLOW')
            sys.exit()


    def __processCommon(self, header):

        util.debug ('ParserNagios', 'plugin started (Nagios2)...', '--')

        start_time = time.time()

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

                for sid in ParserNagios.sids:

                    regexp = re.compile (header + sid["pattern"], re.IGNORECASE)
                    result = regexp.search(line)
                    if result is not None:

                        hash = result.groupdict()

                        date = sensor = src = service = tries = ""

                        # get date
                        if hash.has_key("month") and hash.has_key("day") and \
                           hash.has_key("hour") and hash.has_key("minute") and \
                           hash.has_key("second"):
                            year = time.strftime('%Y', time.localtime(time.time()))
                            datestring = "%s %s %s %s %s %s" % \
                                (year, hash["month"], hash["day"], hash["hour"], 
                                 hash["minute"], hash["second"])
                            date = time.strftime('%Y-%m-%d %H:%M:%S',
                                time.strptime(datestring,
                                    "%Y %b %d %H %M %S"))
                        else:
                            date = time.strftime(
                                   '%Y-%m-%d %H:%M:%S',
                                   time.localtime(float(hash["time"]))
                                   )


                        # sensor
                        try:
                            if hash.has_key("sensor"):
                                sensor = socket.gethostbyname(hash["sensor"])
                            else:
                                sensor = self.plugin["sensor"]
                        except socket.error:
                            sensor = self.plugin["sensor"]

                        # service
                        if hash.has_key("service"):
                            if hash["service"]:
                                service = hash["service"].lower()
                        # tries
                        if hash.has_key("tries"):
                            if hash["tries"]:
                                tries = hash["tries"]

                        # get src ip
                        try:
                            if hash.has_key("src"):
                                if hash["src"]:
                                    src = socket.gethostbyname(hash["src"])
                                    if src == '127.0.0.1':
                                        src = sensor
                                else:
                                    src = sensor
                            else:
                                src = sensor
                        except socket.error:
                            src = hash["src"]


                        self.agent.sendEvent (
                                    type = 'detector',
                                    date       = date,
                                    sensor     = sensor,
                                    interface  = self.plugin["interface"],
                                    plugin_id  = self.plugin["id"],
                                    plugin_sid = sid["sid"],
                                    priority   = '1',
                                    protocol   = '',
                                    src_ip     = src,
                                    src_port   = '',
                                    dst_ip     = '',
                                    dst_port   = '',
                                    userdata1  = service,
                                    userdata2  = tries,
                                    log        = line)

                        # alert sent
                        break

        fd.close()

# vim:ts=4 sts=4 tw=79 expandtab:
