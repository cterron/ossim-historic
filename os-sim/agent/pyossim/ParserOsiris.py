import re
import sys
import time

import Parser
import util

class ParserOsiris(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'syslog':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] + " unknown for Osiris...", '!!', 'RED')
            sys.exit()

    def __processOsirisLog(self, line, date, sensor, sid, target, location, count):
        util.debug ('ParserOsiris', 'examining ' + target + ' (' + count + ' events)...', '--')


        # [213][gestalt][cmp][/sw/bin/gtk-config][ctime][Tue Apr 20 19:20:41 2004,Sun Jul 25 02:59:00 2004]
        event_pattern = '^\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\]'
        event_pattern_ext = '^\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\]\[([^\]]*)\](.*)'

        file_pattern = '(.*)logs/\d+'
        host_pattern = 'host\s+=\s+(\d+.\d+.\d+.\d+)'

        try:
            result = re.findall(str(file_pattern), location);
            try:
                (conffile) = result[0]
            except IndexError:
                return 
            try:
                fd = open(conffile + "host.conf", 'r')
            except IOError, e:
                util.debug(__name__, e, '!!', 'RED')
                self.agent.sendAlert  (type = 'detector',
                    date       = date,
                    sensor     = self.plugin["sensor"],
                    interface  = self.plugin["interface"],
                    plugin_id  = self.plugin["id"],
                    plugin_sid = 10000,
                    priority   = 1,
                    protocol   = '',
                    src_ip     = target,
                    src_port   = '',
                    dst_ip     = '',
                    dst_port   = '',
                    log = line)
                return

        except IndexError: 
            return

        while 1:
            line = fd.readline()
            if not line:
                break
            else:
                result = re.findall(str(host_pattern), line)
                try:
                    src = result[0]
                    break
                except IndexError:
                    fd.close()
                    return
        fd.close()

        # Please be more specific ;)
        if src == "127.0.0.1":
            src = self.plugin["sensor"]

        try:
            fd = open(location, 'r')
        except IOError, e:
            util.debug(__name__, e, '!!', 'RED')
            self.agent.sendAlert  (type = 'detector',
                date       = date,
                sensor     = self.plugin["sensor"],
                interface  = self.plugin["interface"],
                plugin_id  = self.plugin["id"],
                plugin_sid = 10000,
                priority   = 1,
                protocol   = '',
                src_ip     = target,
                src_port   = '',
                dst_ip     = '',
                dst_port   = '',
                log = line)
            return

        self.agent.sendAlert  (type = 'detector',
            date       = date,
            sensor     = self.plugin["sensor"],
            interface  = self.plugin["interface"],
            plugin_id  = self.plugin["id"],
            plugin_sid = 10001,
            priority   = 1,
            protocol   = '',
            src_ip     = src,
            src_port   = '',
            dst_ip     = '',
            dst_port   = '',
            log = line)

        while 1:

            line = fd.readline()
            print "log"
            if not line: # EOF reached
                fd.close()
                return
            else:
                result = re.findall(str(event_pattern), line)
                try: 
                    (sid, src_name, type, target, what) = result[0]
                    data = ""
                    if type == "cmp" or type == "missing":
                        result = re.findall(str(event_pattern_ext), line)
                        try:
                            (sid, src_name, type, target, what, data) = result[0]
                        except IndexError:
                            pass

#                    self.agent.sendHidsEvent (host = src,
#                                    hostname = src_name,
#                                    event_type = type,
#                                    target = what,
#                                    extra_data = data,
#                                    sensor = self.plugin["sensor"],
#                                    date = date,
#                                    plugin_id = self.plugin["id"],
#                                    plugin_sid = sid,
#                                    log = line)

                    self.agent.sendHidsEvent (src, src_name, type, target, what, data, self.plugin["sensor"], date, self.plugin["id"], sid, line)



#                    self.agent.sendAlert  (type = 'detector',
#                                     date       = date,
#                                     sensor     = self.plugin["sensor"],
#                                     interface  = self.plugin["interface"],
#                                     plugin_id  = self.plugin["id"],
#                                     plugin_sid = sid,
#                                     priority   = 1,
#                                     protocol   = '',
#                                     src_ip     = src,
#                                     src_port   = '',
#                                     dst_ip     = '',
#                                     dst_port   = '',
#                                     log = line)
                except IndexError: 
                    pass


    def __processSyslog(self):
        
        util.debug ('ParserOsiris', 'plugin started (syslog)...', '--')
        
        # [213][gestalt][cmp][/sw/bin/gtk-config][ctime][Tue Apr 20 19:20:41 2004,Sun Jul 25 02:59:00 2004]
        #line = 'Aug 13 18:31:04 localhost osirismd[15481]: [702][gestalt][info] notifying dk@ossim.net using 127.0.0.1:25, sending contents of log file: /usr/local/osiris/hosts/gestalt/logs/55, containing 87 entries.'
        pattern = '(\w+)\s+(\d{1,2})\s+(\d\d):(\d\d):(\d\d)\s+([\w\-\_]+|\d+.\d+.\d+.\d+)\s+osirismd\[\d+\]:\s+\[(\d+)\]\[([^\]]*)\].*log\sfile:\s+(.*),\s+containing\s(\d+)\sentries.*'
            
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
                result = re.findall(str(pattern), line)
                try: 
                    (monthmm, day, hour, minute, second, sensor, sid, target, src, count) = result[0]

                    
                    year = time.strftime('%Y', time.localtime(time.time()))
                    datestring = "%s %s %s %s %s %s" % \
                        (year, monthmm, day, hour, minute, second)
                    
                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.strptime(datestring, 
                                                       "%Y %b %d %H %M %S"))
                    self.__processOsirisLog(line, date, sensor, sid, target, src, count)

                except IndexError: 
                    pass
                except KeyError:
                    util.debug (__name__, 'Unknown plugin sid (%s)' %
                                sid, '**', 'RED')
        fd.close()

