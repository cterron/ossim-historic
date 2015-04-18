import re, sys, time

import Parser
import util

class ParserCiscoIDS(Parser.Parser):

    plugin_id = 1515

    def process(self):

        if self.plugin["source"] == 'common':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Cisco IDS...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserCiscoIDS', 'plugin started (syslog)...', '--')
        
        pattern = re.compile("(\d+),\s*(\d+),\s*(\d+)/(\d+)/(\d+),\s*(\d+:\d+:\d+),\s*(\d+)/(\d+)/(\d+),\s*(\d+:\d+:\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(IN|OUT),\s*(IN|OUT),\s*(\d+),\s*(\d+),\s*(\d+),\s*([^,]+),\s*(\d+\.\d+\.\d+\.\d+),\s*(\d+\.\d+\.\d+\.\d+),\s*(\d+),\s*(\d+),\s*(\d+\.\d+\.\d+\.\d+)(?:,\s*([^,]+))(?:,\s*(.*))$")
            
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
                    
                    (type, number, 
                     year_gmt, month_gmt, day_gmt, hour_gmt, 
                     year_local, month_local, day_local, hour_local, 
                     app_id, host_id, org_id, 
                     src_location, dst_location, 
                     alarm_level, sig_id, sig_sid, protocol,
                     src_ip, dst_ip, src_port, dst_port, ext_ip, 
                     event_detail, context_data) = result.groups()

                    date = "%s-%s-%s %s" % \
                        (year_local, month_local, day_local, hour_local)

                    self.agent.sendAlert  (
                                     type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = self.plugin["interface"],
                                     plugin_id  = ParserCiscoIDS.plugin_id,
                                     plugin_sid = sig_id,
                                     priority   = 1,
                                     protocol   = protocol,
                                     src_ip     = src_ip,
                                     src_port   = src_port,
                                     dst_ip     = dst_ip,
                                     dst_port   = dst_port,
                                     data       = event_detail + ": " + \
                                                  context_data,
                                     log        = line)

        fd.close()

