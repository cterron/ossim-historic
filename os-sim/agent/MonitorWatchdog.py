import sys
import time
import re
import os

import Monitor
import util

class MonitorWatchdog(Monitor.Monitor):

    def run(self):

        util.debug (__name__, "monitor started", '--')
        while 1:
            
            for id, plugin  in self.plugins.iteritems():
                
                if util.pidof(plugin["name"]) is not None:
                    util.debug (__name__, 
                                'plugin %s (%s) is running' % \
                                (id, plugin["name"]), '->', 'GREEN')
                    self.agent.conn.send("plugin-start plugin_id=\"%s\"\n" % id)
                else:
                    util.debug (__name__,
                                'plugin %s (%s) is not running' % \
                                (id, plugin["name"]), '->', 'RED')
                    
                    # restart daemon
                    if plugin["start"] == 'yes' and \
                       util.pidof(plugin["name"]) is None:
                        cmd = plugin["startup"]
                        util.debug (__name__, 'startup command: ' + 
                                    cmd, '->', 'YELLOW')
                        os.system(cmd)
                        if util.pidof(plugin["name"]) is not None:
                            self.agent.conn.send("plugin-start plugin_id=\"%s\"\n" % id)
                        else:
                            self.agent.conn.send("plugin-stop plugin_id=\"%s\"\n" % id)
                            
                    else:
                        self.agent.conn.send("plugin-stop plugin_id=\"%s\"\n"\
                        % id)

            # check every 5 mins
            time.sleep(300)

