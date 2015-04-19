import re, sys, time, socket

import Parser
import util

class ParserPostfix(Parser.Parser):

    sids = [
        {
            #Jul 17 06:26:32 192.168.1.1 postfix/smtpd[13507]: NOQUEUE: reject: RCPT from 156.Red-213-96-86.pooles.rima-tde.net[213.96.86.156]: 450 <test@goo0000gle.com>: Sender address rejected: Domain not found; from=<test@goo0000gle.com> to=<test.3061@google.com> proto=ESMTP helo=<goo0000gle.com>

            "sid":      1,
            "name":     "postfix: relaying denied",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "Relay access denied.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      2,
            "name":     "postfix: sender domain not found",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "Sender address rejected: Domain not found",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      3,
            "name":     "postfix: recipient user unknown",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "Recipient address rejected: User unknown in relay recipient table;",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      4,
            "name":     "postfix: blocked using spamhaus",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "blocked using [\w\-\.]+.spamhaus.org;",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      5,
            "name":     "postfix: blocked using njabl",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "blocked using [\w\-\.]+.njabl.org;",
                    re.IGNORECASE
                ),
        },
        {
            # Keep this the last of the "blocked using" group
            "sid":      5000,
            "name":     "postfix: blocked using unknown list",
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    "(?P<sensor>[^\s]*).*" +\
                    "reject: RCPT from [\w\-\.]+\[(?P<src>[\d\.]+)\]: .*" +\
                    "blocked using ",
                    re.IGNORECASE
                )
        }


    ]

    def process(self):

        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Syslog...", '!!', 'YELLOW')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserPostfix', 'plugin started (syslog)...', '--')

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

                for sid in ParserPostfix.sids:
                
                    date = src = ""
                
                    result = sid["pattern"].search(line)
                    if result is not None:

                        hash = result.groupdict()

                        # get date
                        year = time.strftime('%Y', time.localtime(time.time()))
                        datestring = "%s %s %s %s %s %s" % \
                            (year, hash["month"], hash["day"], hash["hour"], 
                             hash["minute"], hash["second"])
                        date = time.strftime('%Y-%m-%d %H:%M:%S',
                            time.strptime(datestring,
                                "%Y %b %d %H %M %S"))

                        # get src ip
                        try:
                            if hash.has_key("src"):
                                src = socket.gethostbyname(hash["src"])
                        except socket.error:
                            src = hash["src"]

                        # sensor
                        try:
                            if hash.has_key("sensor"):
                                sensor = socket.gethostbyname(hash["sensor"])
                            else:
                                sensor = self.plugin["sensor"]
                        except socket.error:
                            sensor = self.plugin["sensor"]


                        self.agent.sendEvent (
                                    type = 'detector',
                                    date       = date,
                                    sensor     = sensor,
                                    interface  = self.plugin["interface"],
                                    plugin_id  = self.plugin["id"],
                                    plugin_sid = sid["sid"],
                                    priority   = '1',
                                    protocol   = 'tcp',
                                    src_ip     = src,
                                    src_port   = '',
                                    dst_ip     = sensor,
                                    dst_port   = 25, # TODO: self.plugin["port"]
                                    data       = line,
                                    log        = line)

                        # alert sent
                        break                

        fd.close()

