import re
import sys
import time
import os

import Parser
import util

class ParserArpwatch(Parser.Parser):

    def process(self):
        
        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__, "log type " + self.plugin["source"] +\
                        " unknown for  arpwatch...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug (__name__, 'plugin started (syslog)...', '--')

        start_time = time.time()

        location = self.plugin["location"]

        # first check if file exists
        if not os.path.exists(location):
            fd = open(location, "w")
            fd.close()

        try:
            fd = open(location, 'r')
        except IOError, e:
            util.debug(__name__, e, '!!', 'RED')
            sys.exit()

        # Move to the end of file
        fd.seek(0, 2)

        sensor = ip = addr = vendor = timestamp = ""

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
                
                result_ip = re.findall('ip address: (\S+)', line)
                result_iface = re.findall('interface: (\S+)', line)
                result_addr = re.findall('^\s*ethernet address: (.*)', line)
                result_vendor = re.findall('^\s*ethernet vendor: (.*)', line)
                result_timestamp = re.findall('^\s*timestamp: ([^\+|\-]*)', line)

                if result_ip != []:
                    ip = iface = addr = vendor = date = ""
                    ip = result_ip[0]
                    lines = line.rstrip()
                elif result_iface != []:
                    iface = result_iface[0]
                    lines += line.rstrip()
                elif result_addr != []:
                    addr = result_addr[0]
                    lines += line.rstrip()
                elif result_vendor != []:
                    vendor = result_vendor[0]
                    lines += line.rstrip()
                elif result_timestamp != []:
                    # timestamp
                    # Monday, March 15, 2004 15:39:19 +0000
                    timestamp = \
                        time.strptime(util.normalizeWhitespace(result_timestamp[0]), 
                                  "%A, %B %d, %Y %H:%M:%S")
                    date = time.strftime('%Y-%m-%d %H:%M:%S', timestamp)
                    lines += line.rstrip()

                    self.agent.sendMacEvent (
                         host       = ip,
                         iface      = iface,
                         mac        = addr,
                         vendor     = vendor,
                         date       = date,
                         sensor     = self.plugin["sensor"],
                         plugin_id  = self.plugin["id"],
                         plugin_sid = 1,
                         log        = lines)

        fd.close()
