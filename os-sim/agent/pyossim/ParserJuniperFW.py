import re
import sys
import time

import Parser
import util

class ParserJuniperFW(Parser.Parser):

    eventCodes = [
    'Permit',
    'Deny',
    'SYN flood',
    'Teardrop attack',
    'Ping of Death',
    'WinNuke attack',
    'IP spoofing',
    'Source Route IP option',
    'Land attack',
    'ICMP flood',
    'UDP flood',
    'Port scan',
    'Address sweep',
    'Malicious URL',
    'Src IP session limit',
    'SYN fragment',
    'No TCP flag',
    'Unknown protocol',
    'Bad IP option',
    'Dst IP session limit',
    'ZIP file blocked',
    'Java applet blocked',
    'EXE file blocked',
    'ActiveX control blocked',
    'ICMP fragment',
    'Large ICMP packet',
    'SYN and FIN bits',
    'FIN but no ACK bit',
    'SYN-ACK-ACK Proxy DoS',
    'Fragmented traffic',
    'DNS:EXPLOIT:CLASS-UNKNOWN attack',
    'DNS:EXPLOIT:EMPTY-UDP-MSG attack',
    'DNS:EXPLOIT:EXPLOIT-BIND9-RT attack',
    'DNS:EXPLOIT:POINTER-LOOP attack',
    'DNS:EXPLOIT:REQUEST-SHORT-MSG attack',
    'DNS:EXPLOIT:TYPE-AXFR attack',
    'DNS:EXPLOIT:TYPE-UNKNOWN attack',
    'DNS:HEADERERROR:INVALID-OPCODE attack',
    'DNS:OVERFLOW:BIN attack',
    'DNS:OVERFLOW:INVALID-LABEL-LEN attack',
    'DNS:OVERFLOW:INVALID-POINTER attack',
    'DNS:OVERFLOW:NAME-TOO-LONG attack',
    'DNS:OVERFLOW:OVERSIZED-UDP-MSG attack',
    'DNS:OVERFLOW:TOO-LONG-TCP-MSG attack',
    'DNS:REPERR:QCLASS-UNEXP attack',
    'DNS:REPERR:REP-MISMATCHING-AN attack',
    'DNS:REPERR:REP-MISMATCHING-QD attack',
    'DNS:REPERR:REP-QTYPE-UNEXPECTED attack',
    'DNS:REPERR:REP-S2C-QUERY attack',
    'DNS:REPERR:REQ-INVALID-HDR-RD attack',
    'DNS:REQERR:REQ-ANSWERS-IN-QUERY attack',
    'DNS:REQERR:REQ-C2S-RESPONSE attack',
    'DOS:NETDEV:IOS-HTTPD-DOS attack',
    'DOS:NETDEV:NETWORK-3COM-DOS attack',
    'FTP:COMMAND:PLATFTP-CD-DOS attack',
    'FTP:COMMAND:SITE-EXEC attack',
    'FTP:DIRECTORY:DOT-DOT attack',
    'FTP:DIRECTORY:DOT-PCT-20-DOT attack',
    'FTP:EXPLOIT:BOUNCE-ATTACK attack',
    'FTP:EXPLOIT:ILLEGAL-PORT attack',
    'FTP:EXPLOIT:SYNTAX-ERROR attack',
    'FTP:MS-FTP:ASTERISK attack',
    'FTP:MS-FTP:STAT-GLOB attack',
    'FTP:OVERFLOW:BSD-FTPD-MKD-OF attack',
    'FTP:OVERFLOW:FREEBSD-FTPD-GLOB attack',
    'FTP:OVERFLOW:LINE_TOO_LONG attack',
    'FTP:OVERFLOW:OPENBSD-X86 attack',
    'FTP:OVERFLOW:PASS_TOO_LONG attack',
    'FTP:OVERFLOW:PATH-LINUX-X86-1 attack',
    'FTP:OVERFLOW:PATH-LINUX-X86-2 attack',
    'FTP:OVERFLOW:PATH-LINUX-X86-3 attack',
    'FTP:OVERFLOW:PATH-TOO-LONG attack',
    'FTP:OVERFLOW:SITESTRING-2-LONG attack',
    'FTP:OVERFLOW:USERNAME-2-LONG attack',
    'FTP:OVERFLOW:WFTPD-MKD-OVERFLOW attack',
    'FTP:OVERFLOW:WUBSD-SE-RACE attack',
    'FTP:PABLO-FTP:FORMAT-STRING attack',
    'FTP:PASSWORD:4DGIFTS attack',
    'FTP:PASSWORD:H0TB0X attack',
    'FTP:PASSWORD:LRKR0X attack',
    'FTP:PASSWORD:SATORI attack',
    'FTP:PASSWORD:WH00T attack',
    'FTP:PROFTP:LOGXFR-OF1 attack',
    'FTP:PROFTP:MKD-OVERFLOW attack',
    'FTP:PROFTP:OVERFLOW1 attack',
    'FTP:PROFTP:PPC-FS1 attack',
    'FTP:PROFTP:PPC-FS2 attack',
    'FTP:PROFTP:SIZE-DOS2 attack',
    'FTP:PROFTP:USER-DOS attack',
    'FTP:REQERR:LOGIN-FAILED attack',
    'FTP:REQERR:REQ-INVALID-CMD-SEQ attack',
    'FTP:REQERR:REQ-MISSING-ARGS attack',
    'FTP:REQERR:REQ-NESTED-REQUEST attack',
    'FTP:REQERR:REQ-UNKNOWN-CMD attack',
    'FTP:RPLERR:REP-NESTED-REPLY attack',
    'FTP:WS-FTP:CPWD attack',
    'FTP:WU-FTP:DELE-OF attack',
    'FTP:WU-FTP:FTPD-BSD-X86 attack',
    'FTP:WU-FTP:GLOBARG attack',
    'FTP:WU-FTP:IREPLY-FS attack',
    'FTP:WU-FTP:LINUX-OF attack',
    'FTP:WU-FTP:REALPATH-OF attack',
    'FTP:WU-FTP:REALPATH-OF2 attack',
    'HTTP:APACHE:NOSEJOB attack',
    'HTTP:APACHE:PHP-INVALID-HDR attack',
    'HTTP:APACHE:SCALP attack',
    'HTTP:BIGBROTHER:DIR-TRAVERSAL attack',
    'HTTP:CGI:ALTAVISTA-TRAVERSAL attack',
    'HTTP:CGI:APPLE-QT-FILEDISC1 attack',
    'HTTP:CGI:BNB-SURVEY-REMOTE-EXEC attack',
    'HTTP:CGI:BUGZILLA-SEMICOLON attack',
    'HTTP:CGI:DCFORUM-AZ-EXEC attack',
    'HTTP:CGI:FORMMAIL-ENV-VAR attack',
    'HTTP:CGI:HASSAN-DIR-TRAVERSAL attack',
    'HTTP:CGI:HTDIG-INCLUSION attack',
    'HTTP:CGI:HYPERSEEK-DIR-TRAVERSL attack',
    'HTTP:CGI:INFOSRCH-REMOTE-EXEC attack',
    'HTTP:CGI:MOREOVER-CACHE-FEED attack',
    'HTTP:CGI:TECHNOTE-PRINT-DSCLSR attack',
    'HTTP:CGI:W3-MSQL-CGI attack',
    'HTTP:CGI:WEBPALS-EXEC attack',
    'HTTP:CGI:WEBSPEED-WSMADMIN attack',
    'HTTP:CGI:WEBSPIRS-FILE-DISCLSR attack',
    'HTTP:CGI:YABB-DIR-TRAVERSAL attack',
    'HTTP:CHKP-FW1-PROXY attack',
    'HTTP:CISCO:IOS-ADMIN-ACCESS attack',
    'HTTP:CISCO:VOIP:PORT-INFO-DOS attack',
    'HTTP:CISCO:VOIP:STREAM-ID-DOS attack',
    'HTTP:COLDFUSION:EXPRCALC-OPNFIL attack',
    'HTTP:DIR:TRAVERSE-DIRECTORY attack',
    'HTTP:EXPLOIT:AMBIG-CONTENT-LEN attack',
    'HTTP:EXPLOIT:BLAZIX-JSPVIEW attack',
    'HTTP:FRONTPAGE:ADMIN.PWD-REQ attack',
    'HTTP:FRONTPAGE:FOURDOTS attack',
    'HTTP:FRONTPAGE:SERVICE.PWD-REQ attack',
    'HTTP:HOSTCTRL:BROWSE-ASP attack',
    'HTTP:IIS:AD-SERVER-CONFIG attack',
    'HTTP:IIS:ASP-CODEBROWSER-EXAIR attack',
    'HTTP:IIS:BAT-& attack',
    'HTTP:IIS:COMMAND-EXEC attack',
    'HTTP:IIS:COMMAND-EXEC-2 attack',
    'HTTP:IIS:DATA-DISCLOSURE attack',
    'HTTP:IIS:DOUBLE-ENCODE attack',
    'HTTP:IIS:FACTO-CMS-SQL-INJ attack',
    'HTTP:IIS:HEADER-HOST-DOS attack',
    'HTTP:IIS:ISAPI-IDQ-OVERFLOW attack',
    'HTTP:IIS:ISAPI-PRINTER-OVERFLOW attack',
    'HTTP:IIS:MDAC-RDS attack',
    'HTTP:IIS:MDAC-RDS-2 attack',
    'HTTP:IIS:NEWDSN-FILE-CREATION attack',
    'HTTP:IIS:OUTLOOK-WEB-DOS attack',
    'HTTP:IIS:WEBDAV:MALFORMED-REQ1 attack',
    'HTTP:IIS:WEBDAV:MALFORMED-REQ2 attack',
    'HTTP:INFO-LEAK:VIGNETTE-LEAK attack',
    'HTTP:INFO-LEAK:WEB-INF-DOT attack',
    'HTTP:IRIX:CGI-BIN-WRAP attack',
    'HTTP:MISC:HP-PROCURVE-RESET attack',
    'HTTP:MISC:HTACCESS-ATTEMPT attack',
    'HTTP:NOVELL:NETWARE-CONVERT.BAS attack',
    'HTTP:OREILLY:WIN-C-SMPLE-OVFLOW attack',
    'HTTP:OVERFLOW:CHUNK-LEN-OFLOW attack',
    'HTTP:OVERFLOW:CHUNK-OVERFLOW attack',
    'HTTP:OVERFLOW:CONTENT-OVERFLOW attack',
    'HTTP:OVERFLOW:HEADER attack',
    'HTTP:OVERFLOW:INV-CHUNK-LEN attack',
    'HTTP:OVERFLOW:PI3WEB-SLASH-OF attack',
    'HTTP:OVERFLOW:URL-OVERFLOW attack',
    'HTTP:PHP:ACHIEVO-EXEC attack',
    'HTTP:PHP:DFORUM-PHP-INC attack',
    'HTTP:PHP:DOTPROJECT-USERCOOKIE attack',
    'HTTP:PHP:FI-DIR-TRAVERSAL attack',
    'HTTP:PHP:GALLERY-MAL-INCLUDE attack',
    'HTTP:PHP:MANTIS-ARB-EXEC1 attack',
    'HTTP:PHP:MANTIS-ARB-EXEC2 attack',
    'HTTP:PHP:MLOG-SCREEN attack',
    'HTTP:PHP:PHORUM:ADMIN-PW-CHG attack',
    'HTTP:PHP:PHORUM:READ-ACCESS attack',
    'HTTP:PHP:PHORUM:REMOTE-EXEC attack',
    'HTTP:PHP:PHPLIB-REMOTE-EXE attack',
    'HTTP:PHP:PHPNUKE:MODULES-DOS attack',
    'HTTP:PHP:PHPWEB-REMOTE-FILE attack',
    'HTTP:PHP:PMACHINE-INCLUDE attack',
    'HTTP:PHP:POSTNUKE-SQL-EXEC attack',
    'HTTP:PHP:REDHAT-PIRANHA-PASSWD attack',
    'HTTP:PHP:VBULL-CAL-EXEC attack',
    'HTTP:PHP:WOLTAB-SQL-INJ attack',
    'HTTP:PHP:YABBSE-PKG-EXEC attack',
    'HTTP:PHP:YABBSE-SSI-INCLUDE attack',
    'HTTP:PHP:ZENTRACK-CMD-EXEC attack',
    'HTTP:PKG:ALLAIRE-JRUN-DOS attack',
    'HTTP:PKG:DB4WEB-FILE-ACCESS-LIN attack',
    'HTTP:PKG:EWAVE-SERVLET-DOS attack',
    'HTTP:PKG:MOUNTAIN-ORDR-DSCLSR attack',
    'HTTP:PKG:WEBGAIS-REMOTE-EXEC attack',
    'HTTP:REQERR:REQ-INVALID-FORMAT attack',
    'HTTP:REQERR:REQ-LONG-UTF8CODE attack',
    'HTTP:REQERR:REQ-MALFORMED-URL attack',
    'HTTP:TOMCAT:JSP-AS-HTML attack',
    'HTTP:TOMCAT:SERVLET-DEVICE-DOS attack',
    'HTTP:WASD:CONF-ACCESS attack',
    'HTTP:WASD:DIR-TRAV attack',
    'HTTP:WEBLOGIC:URL-REVEAL-SRC attack',
    'HTTP:WEBLOGIC:WEBROOT attack',
    'HTTP:WEBPLUS:DIR-TRAVERSAL attack',
    'HTTP:WIN-CMD:WIN-RGUEST attack',
    'HTTP:WIN-CMD:WIN-WGUEST attack',
    'IMAP:OVERFLOW:COMMAND attack',
    'IMAP:OVERFLOW:FLAG attack',
    'IMAP:OVERFLOW:IMAP4-LSUB-OF attack',
    'IMAP:OVERFLOW:LINE attack',
    'IMAP:OVERFLOW:MAILBOX attack',
    'IMAP:OVERFLOW:PASS attack',
    'IMAP:OVERFLOW:REFERENCE attack',
    'IMAP:OVERFLOW:TAG attack',
    'IMAP:OVERFLOW:USER attack',
    'IMAP:REQERR:LOGIN-FAILED attack',
    'IMAP:REQERR:REQ-INVALID-STATE attack',
    'IMAP:REQERR:REQ-INVALID-TAG attack',
    'IMAP:REQERR:REQ-UNEXPECTED-ARG attack',
    'POP3:DOS:MDAEMON-POP-DOS attack',
    'POP3:OVERFLOW:APOP attack',
    'POP3:OVERFLOW:COMMAND attack',
    'POP3:OVERFLOW:LINE attack',
    'POP3:OVERFLOW:PASS attack',
    'POP3:OVERFLOW:QPOP-OF1 attack',
    'POP3:OVERFLOW:QPOP-OF2 attack',
    'POP3:OVERFLOW:QPOP-OF3 attack',
    'POP3:OVERFLOW:QPOP-OF4 attack',
    'POP3:OVERFLOW:USER attack',
    'POP3:REQERR:LOGIN-FAILED attack',
    'POP3:REQERR:REQ-INVALID-STATE attack',
    'POP3:REQERR:REQ-MESSAGE-NUMBER attack',
    'POP3:REQERR:REQ-NESTED-REQUEST attack',
    'POP3:REQERR:REQ-SYNTAX-ERROR attack',
    'POP3:REQERR:REQ-UNKNOWN-CMD attack',
    'SHELLCODE:AIX:NOOP-PKT attack',
    'SHELLCODE:BSDX86:GEN-1-PKT attack',
    'SHELLCODE:BSDX86:GEN-2-PKT attack',
    'SHELLCODE:DIGITAL:NOOP-PKT attack',
    'SHELLCODE:HP-UX:HP-NOOP-1-PKT attack',
    'SHELLCODE:HP-UX:HP-NOOP-2-PKT attack',
    'SMTP:COMMAND:DEBUG attack',
    'SMTP:COMMAND:ETRN attack',
    'SMTP:COMMAND:EXPN attack',
    'SMTP:COMMAND:SMTP-VRFY-CMD attack',
    'SMTP:COMMAND:TURN attack',
    'SMTP:COMMAND:VRFY attack',
    'SMTP:COMMAND:WIZ attack',
    'SMTP:EMAIL:HEADER-FROM-PIPE attack',
    'SMTP:EMAIL:HEADER-TO-PIPE attack',
    'SMTP:EMAIL:MAIL-FROM-PIPE attack',
    'SMTP:EMAIL:RCPT-TO-DECODE attack',
    'SMTP:EMAIL:RCPT-TO-PIPE attack',
    'SMTP:EMAIL:REPLY-TO-PIPE attack',
    'SMTP:EXCHANGE:DOS attack',
    'SMTP:MAJORDOMO:COMMAND-EXEC attack',
    'SMTP:MSSQL-WORM-EMAIL attack',
    'SMTP:OVERFLOW:COMMAND-LINE attack',
    'SMTP:OVERFLOW:EMAIL-ADDRESS attack',
    'SMTP:OVERFLOW:EMAIL-DOMAIN attack',
    'SMTP:OVERFLOW:EMAIL-USERNAME attack',
    'SMTP:OVERFLOW:OUTLOOK-CERT-OF attack',
    'SMTP:OVERFLOW:REPLY-LINE attack',
    'SMTP:OVERFLOW:SENDMAIL-CMT-OF1 attack',
    'SMTP:OVERFLOW:SENDMAIL-CMT-OF2 attack',
    'SMTP:OVERFLOW:TOO-MANY-RCPT attack',
    'SMTP:REPERR:REP-INVALID-REPLY attack',
    'SMTP:REPERR:REP-NESTED-REPLY attack',
    'SMTP:REQERR:REQ-INVALID-CMD-SEQ attack',
    'SMTP:REQERR:REQ-NESTED-REQUEST attack',
    'SMTP:REQERR:REQ-SYNTAX-ERROR attack',
    'SMTP:REQERR:REQ-UNKNOWN-CMD attack',
    'SMTP:RESPONSE:PIPE-FAILED attack',
    'SMTP:SENDMAIL:ADDR-PRESCAN-ATK attack',
    'SMTP:SENDMAIL:SENDMAIL-FF-OF attack',
    'TROJAN:QAZ:TCP25-CALLING-HOME attack',
    'WORM:CODERED-2:CMD-BACKDOOR attack',
    'WORM:CODERED-2:INFECT-ATTEMPT attack',
    'WORM:CODERED-2:ROOT-BACKDOOR attack',
    'WORM:NIMDA:BIN-255-CMD attack',
    'WORM:NIMDA:MSADC-ROOT attack',
    'WORM:NIMDA:SCRIPTS-C11C-CMD attack',
    'WORM:NIMDA:SCRIPTS-CMD attack',
    'WORM:NIMDA:SCRIPTS-ROOT attack',
    'Unknown',    ]

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for Juniper FW...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')

        pattern_type = '\[.*\]system\-(\w+\-\d+)'
        
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
                result = re.findall(str(pattern_type), line)
                try:
                    self.eventFuncs[result[0]](self, line)
                except IndexError:
                    continue
                except KeyError:
                    continue
        fd.close()

    # Function to handle traffic (Permit, Deny)
    def traffic(self, line):

        pattern_traffic = '(\w+)\s+(\d{1,2})\s(\d\d:\d\d:\d\d)\s\d+\.\d+\.\d+\.\d+\s.+:\sNetScreen\sdevice\_id\=.+\s+\[.+\]system\-\w+\-\d+\(traffic\):.+proto=(\d+)\s.+action=(\w+)\s.+src=(\d+\.\d+\.\d+\.\d+)\sdst=(\d+\.\d+\.\d+\.\d+)\ssrc_port=(\d+)\sdst_port=(\d+)'

        result = re.findall(str(pattern_traffic), line)

        try:
            (month, day, datetime, proto, action,
            src, dst, s_port, d_port) = result[0]

            year = time.strftime('%Y', time.localtime(time.time()))
            datestring = "%s %s %s %s" % (year, month, day, datetime)
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                time.strptime(datestring,
                "%Y %b %d %H:%M:%S"))
            try:
                plugin_sid = self.eventCodes.index(action)+1
            except ValueError:
                plugin_sid = len(self.eventCodes)
                util.debug (__name__, 'Unknown event ' + action, '**', 'YELLOW')

            if proto == 6:
                proto = "tcp"
            elif proto == 17:
                proto = "udp"
            elif proto == 1:
                proto = "icmp"

            self.agent.sendEvent  (type = 'detector',
                date        = date,
                sensor        = self.plugin["sensor"],
                interface    = self.plugin["interface"],
                plugin_id    = self.plugin["id"],
                plugin_sid    = plugin_sid,
                priority    = 1,
                protocol    = proto,
                src_ip        = src,
                src_port    = s_port,
                dst_ip        = dst,
                dst_port    = d_port,
                log        = line)

        except IndexError:
            pass

    # Function to handle attacks messages
    def attacks(self, line):

        pattern = '(\w+)\s+(\d{1,2})\s(\d\d:\d\d:\d\d)\s\d+\.\d+\.\d+\.\d+\s.+:\sNetScreen\sdevice\_id\=.+\s+\[.+\]system\-(\w+)\-(\d+):\s(.+)\!\sFrom\s(\d+\.\d+\.\d+\.\d+):(\d+)\sto\s(\d+\.\d+\.\d+\.\d+):(\d+)\,\sproto\s(\w+).+'

        result = re.findall(str(pattern), line)

        try: 
            (month, day, datetime, prio, nsid, message,
            src, s_port, dst, d_port, proto) = result[0]

            year = time.strftime('%Y', time.localtime(time.time()))
            datestring = "%s %s %s %s" % (year, month, day, datetime)
            date = time.strftime('%Y-%m-%d %H:%M:%S', time.strptime(datestring, "%Y %b %d %H:%M:%S"))

        # Set priority: alert = 4, critical = 3, warning = 2
            if prio == 'alert' or prio == 'emergency':
                priority = 4
            elif prio == 'critical':
                priority = 3
            elif prio == 'warning':
                priority = 2
            else:
                priority = 1

            try:
                plugin_sid = self.eventCodes.index(message)+1
            except ValueError:
                plugin_sid = len(self.eventCodes)
                util.debug (__name__, 'Unknown event ' + message, '**', 'YELLOW')

            if proto == 6:
                proto = "tcp"
            elif proto == 17:
                proto = "udp"
            elif proto == 1:
                proto = "icmp"

            self.agent.sendEvent  (type = 'detector',
                  date       = date,
                  sensor     = self.plugin["sensor"],
                  interface  = self.plugin["interface"],
                  plugin_id  = self.plugin["id"],
                  plugin_sid = plugin_sid,
                  priority   = priority,
                  protocol   = proto,
                  src_ip     = src,
                  src_port   = s_port,
                  dst_ip     = dst,
                  dst_port   = d_port,
                  log        = line)

        except IndexError: 
            pass

    eventFuncs = {
    'notification-00257' : traffic,
    'emergency-00005' : attacks,
    'emergency-00006' : attacks,
    'emergency-00007' : attacks,
    'alert-00004' : attacks,
    'alert-00008' : attacks,
    'alert-00009' : attacks,
    'alert-00010' : attacks,
    'alert-00011' : attacks,
    'alert-00012' : attacks,
    'alert-00016' : attacks,
    'alert-00017' : attacks,
    'critical-00032' : attacks,
    'critical-00033' : attacks,
    'critical-00412' : attacks,
    'critical-00413' : attacks,
    'critical-00414' : attacks,
    'critical-00415' : attacks,
    'critical-00430' : attacks,
    'critical-00431' : attacks,
    'critical-00432' : attacks,
    'critical-00433' : attacks,
    'critical-00434' : attacks,
    'critical-00435' : attacks,
    'critical-00436' : attacks,
    'critical-00437' : attacks,
    'critical-00438' : attacks,
    'critical-00439' : attacks,
    'critical-00440' : attacks,
    'critical-00601' : attacks,
    'error-00601' : attacks,
    'warning-00601' : attacks,
    'notification-00601' : attacks,
    'information-00601' : attacks, }

