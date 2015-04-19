import re, sys, time, socket

import Parser
import util

class ParserSyslog(Parser.Parser):

    sids = [
        {
            "sid":      1,
            "name":     "pam_unix: authentication failure",
            "pattern":  "(\S+) (\d+) (\d\d):(\d\d):(\d\d) \S+ " + \
                        "\S+\[(\d+)\]: " + \
                        "\(pam_unix\) authentication failure; " + \
                        "logname=\S*\s+uid=\d*\s+euid=\d*\s+tty=\S*\s+" + \
                        "ruser=\S*\s+rhost=(\S*)\s+user=(\S*)"
        },
        {
            "sid":      2,
            "name":     "SSHd: Failed password",
            "pattern":  "(\S+) (\d+) (\d\d):(\d\d):(\d\d) \S+ " + \
                        "sshd\[(\d+)\]: Failed password \S+ " + \
                        "(\S+) \S+ \S+:(\d+.\d+.\d+.\d+) \S+ (\d+) \S+"
        },
        {
            "sid":      3,
            "name":     "Telnetd: Authentication failure",
            "pattern":  "(\S+) (\d+) (\d\d):(\d\d):(\d\d) \S+ " + \
                        "login\[(\d+)\]: FAILED LOGIN \(\d+\) \S+ " + \
                        "\Wpts\S+ \S+ \W(\S+)\W \S+ \W(\w+)\S+"
        },
        {
            "sid":      4,
            "name":     "Proftp: Login failed",
            "pattern":  "(\S+) (\d+) (\d\d):(\d\d):(\d\d) \S+ " + \
                        "proftpd\[(\d+)\]: " + \
                        "\S+ \(\S+\[(\d+.\d+.\d+.\d+)\]\) - USER (\S+) " + \
                        "\(Login failed\): Incorrect password\."
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

                month = day = hour = minute = second = ""
                user = source_ip = source_port = dst_ip = dst_port = ""

                for sid in ParserSyslog.sids:
                    
                    result = re.findall(str(sid["pattern"]), line)
                    if result != []:
                        
                        # ssh
                        if sid["sid"] == 2:
                            (month, day, hour, minute, second, process,
                             user, source_ip, source_port) = result[0]
                            dst_port = 22
                        
                        elif sid["sid"] in (1, 3, 4):
                            (month, day, hour, minute, second, process, 
                             host_name, user) = result[0]

                            if host_name:
                                try:
                                    source_ip = socket.gethostbyname(host_name)
                                except socket.error:
                                    source_ip = host_name
                        
                        else:
                            continue

                        year = time.strftime('%Y', time.localtime(time.time()))
                    
                        datestring = "%s %s %s %s %s %s" % \
                            (year, month, day, hour, minute, second)
                    
                        date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))

                        self.agent.sendAlert (
                                    type = 'detector',
                                    date       = date,
                                    sensor     = self.plugin["sensor"],
                                    interface  = self.plugin["interface"],
                                    plugin_id  = self.plugin["id"],
                                    plugin_sid = sid["sid"],
                                    priority   = '1',
                                    protocol   = '',
                                    src_ip     = source_ip,
                                    src_port   = source_port,
                                    data       = user, 
                                    dst_ip     = self.plugin["sensor"],
                                    dst_port   = dst_port,
                                    log        = line)

                        # alert sent
                        break                

        fd.close()

