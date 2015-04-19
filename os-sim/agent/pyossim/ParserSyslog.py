import re, sys, time, socket

import Parser
import util

class ParserSyslog(Parser.Parser):

    sids = [
        {
            "sid":      1,
            "name":     "pam_unix: authentication failure",

            # Feb  7 15:31:41 golgotha login[3867]: (pam_unix) authentication
            # failure; logname=LOGIN uid=0 euid=0 tty=tty1 ruser= rhost=
            # user=dgil
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    ".*?\(pam_unix\) authentication failure" +\
                    ".*?rhost=(?P<src>\S*)" +\
                    ".*?user=(?P<user>\S*)" +\
                    "(?P<sport>)(?P<dport>)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      2,
            "name":     "pam_unix: 2 more authentication failures",
            # Feb  8 10:51:34 golgotha sshd[29755]: (pam_unix) 2 more 
            # authentication failures; logname= uid=0 euid=0 tty=ssh ruser= 
            # rhost=192.168.6.69  user=dgil
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    ".*?\(pam_unix\) 2 more authentication failures" +\
                    ".*?rhost=(?P<src>\S*)" +\
                    ".*?user=(?P<user>\S*)" +\
                    "(?P<sport>)(?P<dport>)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      3,
            "name":     "SSHd: Failed password",

            # Feb  8 10:09:06 golgotha sshd[24472]: Failed password for dgil
            # from ::ffff:192.168.6.69 port 33992 ssh2
            "pattern": re.compile (
                    "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                    "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                    ".*?ssh.*?Failed password for (?P<user>\S+)\s+" + \
                    ".*?(?P<src>\d+.\d+.\d+.\d+)" +\
                    ".*?port\s+(?P<sport>\d+)\s+(?P<dport>\S+)",
                    re.IGNORECASE
                ),
        },
    ]

    def process(self):

        if self.plugin["source"] == 'common':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Syslog...", '!!', 'YELLOW')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserSyslog', 'plugin started (syslog)...', '--')
        
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

                for sid in ParserSyslog.sids:
                
                    date = src = user = sport = dport = ""
                
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

                        # get user
                        if hash.has_key("user"):    user  = hash["user"]
                        
                        # get source and dest ports
                        if hash.has_key("sport"):   sport = hash["sport"]
                        if hash.has_key("dport"):   dport = hash["dport"]
                        if sid["sid"] == 3:  dport = 22 # ssh
                            

                        self.agent.sendAlert (
                                    type = 'detector',
                                    date       = date,
                                    sensor     = self.plugin["sensor"],
                                    interface  = self.plugin["interface"],
                                    plugin_id  = self.plugin["id"],
                                    plugin_sid = sid["sid"],
                                    priority   = '1',
                                    protocol   = '',
                                    src_ip     = src,
                                    src_port   = sport,
                                    data       = user,
                                    dst_ip     = self.plugin["sensor"],
                                    dst_port   = dport,
                                    log        = line)

                        # alert sent
                        break                

        fd.close()

