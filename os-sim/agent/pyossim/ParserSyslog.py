import re, sys, time, socket

import Parser
import util

class ParserSyslog(Parser.Parser):

    # Feb  7 15:31:41 golgotha ....
    syslog_header =  "(?P<month>\S+)\s+(?P<day>\d+)\s+" +\
                     "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+" +\
                     "(?P<sensor>\S*)"

    sids = [
        {
            "sid":      1,
            "name":     "pam_unix: authentication failure",

            # Feb  7 15:31:41 golgotha login[3867]: (pam_unix) authentication
            # failure; logname=LOGIN uid=0 euid=0 tty=tty1 ruser= rhost=
            # user=dgil
            #
            # Feb  8 10:51:34 golgotha sshd[29755]: (pam_unix) 2 more
            # authentication failures; logname= uid=0 euid=0 tty=ssh ruser=
            # rhost=192.168.6.69  user=dgil
            #
            # Apr 26 15:42:27 linuxclient2 sshd[7055]: pam_unix(sshd:auth):
            # authentication failure; logname= uid=0 euid=0 tty=ssh ruser=
            # rhost=itdept3a.cambridge.news user=dirsearch
            "pattern": re.compile (
                    syslog_header +\
                    ".*?(?P<service>\S+)\[(?P<pid>\d+)\]" +\
                    ".*?authentication failure" +\
                    ".*?uid=(?P<src_uid>\S*)" +\
                    ".*?ruser=(?P<src_user>\S*)" +\
                    ".*?rhost=(?P<src>\S*)" +\
                    ".*?user=(?P<dst_user>\S*)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      2,
            "name":     "PAM_unix: authentication failure",
            # Aug 29 12:46:59 sonda PAM_unix[3350]: authentication failure;
            # root(uid=1000) -> root for su service
            #
            # Aug 29 11:11:03 sonda PAM_unix[3332]: authentication failure;
            # (uid=0) -> test for ssh service
            "pattern": re.compile (
                    syslog_header +\
                    ".*?PAM_unix\[(?P<pid>\d+)\]" +\
                    ".*?authentication failure" +\
                    ".*?\(uid=(?P<src_uid>\S+)\)\s+->\s+(?P<dst_user>\S*)" +\
                    ".*?for\s+(?P<service>\S*)\s+service",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      3,
            "name":     "SSHd: Failed password",

            # Feb  8 10:09:06 golgotha sshd[24472]: Failed password for dgil
            # from ::ffff:192.168.6.69 port 33992 ssh2
            "pattern": re.compile (
                    syslog_header +\
                    ".*?ssh.*?\[(?P<pid>\d+)\]" +\
                    ".*?Failed password for (?P<dst_user>\S+) from" + \
                    ".*?(?P<src>\d+.\d+.\d+.\d+)" +\
                    ".*?port\s+(?P<sport>\d+)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      4,
            "name":     "SSHd: Invalid user",

            # Feb  8 10:09:06 golgotha sshd[24472]: Invalid user dgil
            # from 10.0.0.1
            #
            # Aug 29 14:49:15 sonda sshd[26189]: Failed password
            # for invalid user test from 10.5.4.93 port 55861 ssh2
            #
            # Aug 29 14:49:15 sonda sshd[26189]: Failed none
            # for invalid user test from 10.5.4.93 port 55861 ssh2
            "pattern": re.compile (
                    syslog_header +\
                    ".*?ssh.*?\[(?P<pid>\d+)\]" +\
                    ".*?Invalid user (?P<dst_user>\S+) from" +\
                    ".*?(?P<src>\d+.\d+.\d+.\d+)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      5,
            "name":     "pam_unix: session opened",

            # Aug 30 09:16:56 sonda sshd[9977]: (pam_unix) session opened
            # for user test by (uid=0)
            #
            # Aug 30 09:17:12 sonda su[10021]: (pam_unix) session opened 
            # for user root by (uid=1001)
            #
            # Aug 30 10:51:54 localhost login[4228]: (pam_unix) session opened
            # for user alopezp by (uid=0)
            #
            # Aug 30 11:00:01 sonda CRON[21674]: (pam_unix) session opened
            # for user root by (uid=0)
            #
            # Jan 29 10:15:01 earth crond(pam_unix)[31492]: session opened
            # for user root by (uid=0)
            "pattern": re.compile (
                    syslog_header +\
                    ".*?(?P<service>\S+)\[(?P<pid>\d+)\]" +\
                    ".*?session\s+opened\s+for" +\
                    "\s+user\s+(?P<dst_user>\S+)\s+by" +\
                    ".*?\(uid=(?P<src_uid>\S+)\)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      6,
            "name":     "PAM_unix: session opened",

            # Aug 29 12:46:58 10.5.39.6 PAM_unix[3349]: (su) session opened
            # for user test by root(uid=0)
            #
            # Aug 29 12:46:49 10.5.39.6 PAM_unix[3344]: (ssh) session opened
            # for user root by (uid=0)
            "pattern": re.compile (
                    syslog_header +\
                    ".*?PAM_unix\[(?P<pid>\d+)\]" +\
                    ".*?\((?P<service>\S+)\)session\s+opened\s+for" +\
                    "\s+user\s+(?P<dst_user>\S+)\s+by" +\
                    ".*?\(uid=(?P<src_uid>\S+)\)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      7,
            "name":     "SSHd: Accepted login",

            # Aug 29 11:13:26 10.5.39.6 sshd[3334]: Accepted publickey
            # for root from 10.5.5.147 port 57143 ssh2
            #
            # Aug 29 15:21:33 sonda sshd[26320]: Accepted password
            # for root from 10.5.135.202 port 34379 ssh2
            #
            # Aug 30 09:45:05 sonda sshd[20597]: Accepted keyboard-interactive/pam
            # for root from 10.5.4.233 port 51706 ssh2
            "pattern": re.compile (
                    syslog_header +\
                    ".*?ssh.*?\[(?P<pid>\d+)\]" +\
                    ".*?Accepted\s+\S+\s+for\s+(?P<dst_user>\S+)" + \
                    "\s+from\s+.*?(?P<src>\d+.\d+.\d+.\d+)" + \
                    "\s+port\s+(?P<sport>\d+)",
                    re.IGNORECASE
                ),
        },
    ]

    def process(self):

        if self.plugin["source"] == 'common':
            while 1: self.__processSyslog()
 
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Syslog...", '!!', 'YELLOW')
            sys.exit()


    def __processSyslog(self):

        util.debug ('ParserSyslog', 'plugin started (syslog)...', '--')

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

                for sid in ParserSyslog.sids:

                    result = sid["pattern"].search(line)
                    if result is not None:

                        hash = result.groupdict()

                        date = src = src_uid = src_user = dst_user = sport = dport = service = ""

                        # service
                        if hash.has_key("service"):
                            service = hash["service"].lower()
                            service = service.replace("(pam_unix)","")
                            # ssh and sshd are the same service
                            if service == 'sshd':
                                service = 'ssh'
                            # if pam_unix try next sid
                            elif service == 'pam_unix':
                                # continue 'for' loop to process next pattern
                                continue
                            # ignore task schedulers
                            elif service in ('cron','crond','atd'):
                                # break 'for' loop to process next event
                                break
                        elif sid["sid"] in (3,4,7):
                            service = 'ssh'

                        # if sid["sid"] is modified then ParserSyslog.sids[n]["sid"] is modified too
                        # Therefore local variable sid_id is necessary in order to modify temporary sid
                        sid_id = sid["sid"]

                        # For ssh service
                        if service == 'ssh':
                            # "pam_unix: authentication failure" events is like
                            # "SSHd: Password failed"
                            if sid_id == 1:
                                # set sid_id to 3 (SSHd: Password failed)
                                sid_id = 3

                            # "PAM_unix: authentication failure", "pam_unix: session opened"
                            # and "PAM_unix: session opened" events for ssh service are useless
                            # because they miss rhost info
                            if sid_id in (2,5,6):
                                # break 'for' loop to process next event
                                break

                        # get date
                        year = time.strftime('%Y', time.localtime(time.time()))
                        datestring = "%s %s %s %s %s %s" % \
                            (year, hash["month"], hash["day"], hash["hour"],
                             hash["minute"], hash["second"])
                        date = time.strftime('%Y-%m-%d %H:%M:%S',
                            time.strptime(datestring,
                                "%Y %b %d %H %M %S"))

                        # get users
                        if hash.has_key("dst_user"):    dst_user  = hash["dst_user"]
                        if hash.has_key("src_user"):    src_user  = hash["src_user"]
                        if hash.has_key("src_uid"):     src_uid   = hash["src_uid"]

                        # get source and dest ports
                        if hash.has_key("sport"):   sport = hash["sport"]
                        if hash.has_key("dport"):   dport = hash["dport"]
                        # ssh service could be on another port than 22
                        #if sid_id == 3:  dport = 22 # ssh

                        # sensor
                        try:
                            if hash.has_key("sensor"):
                                sensor = socket.gethostbyname(hash["sensor"])
                            else:
                                sensor = self.plugin["sensor"]
                        except socket.error:
                            sensor = self.plugin["sensor"]

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
                                    plugin_sid = sid_id,
                                    priority   = '1',
                                    protocol   = '',
                                    src_ip     = src,
                                    src_port   = sport,
                                    username   = dst_user,
                                    userdata1  = src_user,
                                    userdata2  = src_uid,
                                    userdata3  = service,
                                    dst_ip     = sensor,
                                    dst_port   = dport,
                                    log        = line)

                        # alert sent
                        break

        fd.close()

# vim:ts=4 sts=4 tw=79 expandtab:
