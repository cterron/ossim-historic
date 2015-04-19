import re, sys, time, socket
import string

import Parser
import util

class ParserNetscreen(Parser.Parser):


    def process(self):

        if self.plugin["source"] == 'syslog':
            while 1: self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Syslog...", '!!', 'YELLOW')
            sys.exit()


    def __processSyslog(self):
       
	# This pattern is for syslog output from the Netscreen Manager, NOT netscreen devices.  
	# They are different formats.

	# NSM uses a CSV output with the following format 
	#
	# Log Day Id, Log Record Id, Time Received (UTC), Time Generated (UTC), Device Domain, Device Domain Version, Device Name, Category, Sub-Category, Src Zone, Src Intf, Src Addr, Src Port, NAT Src Addr, NAT Src Port, Dst Zone, Dst Intf, Dst Addr, Dst Port, NAT Dst Addr, NAT Dst Port, Protocol, Policy Domain, Policy Domain Version, Policy, Rulebase, Rule Number, Action, Severity, Is Alert, Details, User, App, URI, Elapsed Secs, Bytes In, Bytes Out, Bytes Total, Packets In, Packets Out, Packets Total, Repeat Count, Has Packet Data, Var Data Enum
 
        pattern = "([\w:\(\)\.\s\/]+),\s"

        util.debug (__name__, 'plugin started (syslog)...', '--')

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

                result = re.findall(str(pattern), line)
                if result is not None:

                    dstring1 = result[3]
                    src = result[11]
                    sport = result[12]
                    dst = result[17]
                    dport = result[18]
                    proto = result[21]
                    action = result[27]
                    sbytes = result[35]
                    rbytes = result[36]

                    datestring = string.replace(dstring1, '/', '-') 

                    # action (accepted | pckt dropped) -> plugin_sid
                    if action == 'accepted':
                        plugin_sid = 1
                    elif action == 'pckt dropped':
                        plugin_sid = 2

                    self.agent.sendEvent (
                            type       = 'detector',
                            date       = datestring,
                            sensor     = self.plugin["sensor"],
                            interface  = self.plugin["interface"],
                            plugin_id  = self.plugin["id"],
                            plugin_sid = plugin_sid,
                            priority   = '',
                            protocol   = proto,
                            src_ip     = src,
                            src_port   = sport,
                            dst_ip     = dst,
                            dst_port   = dport,
                            log        = line)

                        # alert sent

        fd.close()
