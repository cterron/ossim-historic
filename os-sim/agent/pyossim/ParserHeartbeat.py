import re, sys, time, socket

import Parser
import util

class ParserHeartbeat(Parser.Parser):

    # heartbeat[7186]: 2006/10/19_11:40:05 ....
    heartbeat_header =  "heartbeat\[\d+\]:\s+" +\
                        "(?P<year>\d+)/(?P<month>\d+)/(?P<day>\d+)_" +\
                        "(?P<hour>\d\d):(?P<minute>\d\d):(?P<second>\d\d)\s+"


    sids = [
        {
            "sid":      1.1,
            "name":     "heartbeat: local node up",

            # heartbeat[7186]: 2006/10/19_11:40:05 info: Local status now set to: 'up'
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Local status now set to: \'up\'",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      1.2,
            "name":     "heartbeat: remote node up",

            # heartbeat[7186]: 2006/10/19_11:45:02 info: Status update for node ossim02.maqueta.cgp: status up
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Status update for node\s+(?P<src_node>\S+): status up",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      2.1,
            "name":     "heartbeat: local node active",

            # heartbeat[7186]: 2006/10/19_11:40:30 info: Local status now set to: 'active'
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Local status now set to: \'active\'",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      2.2,
            "name":     "heartbeat: remote node active",

            # heartbeat[7186]: 2006/10/19_11:45:03 info: Status update for node ossim02.maqueta.cgp: status active
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Status update for node\s+(?P<src_node>\S+): status active",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      3,
            "name":     "heartbeat: node dead",

            # heartbeat[7186]: 2006/10/19_11:40:30 WARN: node ossim02.maqueta.cgp: is dead
            # heartbeat[5754]: 2006/10/19_12:32:56 WARN: node 192.168.0.1: is dead
            "pattern": re.compile (
                    heartbeat_header +\
                    "WARN: node\s+(?P<src_node>\S+):.* is dead",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      4,
            "name":     "heartbeat: link up",

            # heartbeat[7186]: 2006/10/19_11:40:06 info: Link 172.24.8.78:172.24.8.78 up.
            # heartbeat[7186]: 2006/10/19_11:45:02 info: Link ossim02.maqueta.cgp:eth0 up.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Link\s+(?P<src_node>\S+):(?P<interface>\S+)\s+up\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      5,
            "name":     "heartbeat: link dead",

            # heartbeat[5754]: 2006/10/19_12:32:56 info: Link 192.168.0.1:192.168.0.1 dead.
            # heartbeat[7186]: 2006/10/19_12:32:56 info: Link ossim02.maqueta.cgp:eth0 dead.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Link\s+(?P<src_node>\S+):(?P<interface>\S+)\s+dead\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      6,
            "name":     "heartbeat: resources being acquired",

            # heartbeat[7186]: 2006/10/19_11:40:30 info: Resources being acquired from ossim02.maqueta.cgp.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Resources being acquired from\s+(?P<dst_node>\S+)\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      7,
            "name":     "heartbeat: resources acquired",

            # heartbeat[10357]: 2006/10/23_17:37:33 info: Local Resource acquisition completed.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Local Resource acquisition completed\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      8,
            "name":     "heartbeat: no resources to acquire",

            # heartbeat[6535]: 2006/10/19_17:46:34 info: No local resources
            # [/usr/lib/heartbeat/ResourceManager listkeys ossim02.maqueta.cgp] to acquire.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: No local resources \[.* listkeys\s+(?P<dst_node>\S+)\] to acquire\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      9,
            "name":     "heartbeat: standby",

            # heartbeat[7186]: 2006/10/19_12:33:10 info: ossim02.maqueta.cgp wants to go standby [all]
            "pattern": re.compile (
                    heartbeat_header +\
                    "info:\s+(?P<src_node>\S+)\s+wants to go standby \[(?P<resources>.*)\]",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      10.1,
            "name":     "heartbeat: local standby completed",

            # heartbeat[5754]: 2006/10/19_12:33:10 info: Local standby process completed [all].
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Local standby process completed \[(?P<resources>.*)\]\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      10.2,
            "name":     "heartbeat: remote standby completed",

            # heartbeat[7186]: 2006/10/19_12:33:11 info: Standby resource acquisition done [all].
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Standby resource acquisition done \[(?P<resources>.*)\]\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      11.1,
            "name":     "heartbeat: local shutdown",

            # heartbeat[5869]: 2006/10/19_17:46:34 info: Heartbeat shutdown in progress. (5869)
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Heartbeat shutdown in progress\. \(\d+\)",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      11.2,
            "name":     "heartbeat: remote shutdown",

            # heartbeat[5754]: 2006/10/19_17:46:34 info: Received shutdown notice from 'ossim01.maqueta.cgp'.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Received shutdown notice from\s+\'(?P<src_node>\S+)\'\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      12.1,
            "name":     "heartbeat: local shutdown completed",

            # heartbeat[5869]: 2006/10/19_17:46:37 info: ossim01.maqueta.cgp Heartbeat shutdown complete.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info:\s+\S+\s+Heartbeat shutdown complete\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      12.2,
            "name":     "heartbeat: remote shutdown completed",

            # heartbeat[5754]: 2006/10/19_17:46:46 info: Dead node ossim01.maqueta.cgp gave up resources.
            "pattern": re.compile (
                    heartbeat_header +\
                    "info: Dead node\s+(?P<src_node>\S+)\s+gave up resources\.",
                    re.IGNORECASE
                ),
        },
        {
            "sid":      13,
            "name":     "heartbeat: late heartbeat",

            # heartbeat[5816]: 2006/10/25_11:49:58 WARN: Late heartbeat: Node 192.168.0.2: interval 5510 ms
            "pattern": re.compile (
                    heartbeat_header +\
                    "WARN: Late heartbeat: Node\s+(?P<src_node>\S+):.*interval\s+(?P<interval>\d+)\s+ms",
                    re.IGNORECASE
                ),
        },
    ]

    def process(self):

        if self.plugin["source"] == 'common':
            while 1: self.__processHeartbeat()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for Heartbeat...", '!!', 'YELLOW')
            sys.exit()


    def __processHeartbeat(self):
        
        util.debug ('ParserHeartbeat', 'plugin started (hearbeat)...', '--')

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

                for sid in ParserHeartbeat.sids:
                
                    result = sid["pattern"].search(line)
                    if result is not None:
                    
                        date = src_node = dst_node = interface = resources = interval = ""

                        hash = result.groupdict()

                        # sensor
                        sensor = self.plugin["sensor"]

                        # get date
                        datestring = "%s %s %s %s %s %s" % \
                            (hash["year"], hash["month"], hash["day"], hash["hour"], 
                             hash["minute"], hash["second"])
                        date = time.strftime('%Y-%m-%d %H:%M:%S',
                            time.strptime(datestring,
                                "%Y %m %d %H %M %S"))

                        # get variables
                        if hash.has_key("src_node"):
                            try:
                                src_node = socket.gethostbyname(hash["src_node"])
                            except socket.error:
                                src_node = hash["src_node"]
                        else:
                            src_node = sensor
                        if hash.has_key("dst_node"):
                            try:
                                dst_node = socket.gethostbyname(hash["dst_node"])
                            except socket.error:
                                dst_node = hash["dst_node"]
                        if hash.has_key("resources"):
                            resources = hash["resources"]
                        if hash.has_key("interface"):
                            interface = hash["interface"]
                        if hash.has_key("interval"):
                            interval = hash["interval"]


                        self.agent.sendEvent (
                                    type = 'detector',
                                    date       = date,
                                    sensor     = sensor,
                                    interface  = self.plugin["interface"],
                                    plugin_id  = self.plugin["id"],
                                    plugin_sid = int(sid["sid"]),
                                    priority   = '1',
                                    protocol   = '',
                                    src_ip     = src_node,
                                    src_port   = '',
                                    dst_ip     = dst_node,
                                    dst_port   = '',
                                    userdata1  = resources,
                                    userdata2  = interface,
                                    userdata3  = interval,
                                    log        = line)

                        # alert sent
                        break                

        fd.close()

# vim:ts=4 sts=4 tw=79 expandtab:
