import sys
import time
import re
import os

import Monitor
import util

class MonitorPlugin(Monitor.Monitor):

    def run(self):
    
        util.debug (__name__, "monitor started", '--')
        util.debug (__name__, "request received... %s" % self.data, 
                    '<=', 'GREEN')
        
        pattern = '(\S+) plugin_id="([^"]*)"'
        result = re.findall(pattern, self.data)
        try:
            (command, plugin_id) = result[0]
            if command == 'plugin-start':
                
                # start daemon
                cmd = self.plugins[plugin_id]["startup"]
                util.debug (__name__, 'startup command: ' + cmd, '->', 'GREEN')
                os.system(cmd)
                
                # notify server about the change
                if util.pidof(self.plugins[plugin_id]["name"]) is not None:
                    self.agent.conn.send('plugin-start plugin_id="%s"\n' % \
                                          plugin_id)
                    self.plugins[plugin_id]["start"] = 'yes'
                
            elif command == 'plugin-stop':
                
                # stop daemon
                cmd = self.plugins[plugin_id]["shutdown"]
                util.debug (__name__, 'shutdown command: ' + cmd, '<-', 'RED')
                os.system(cmd)
                
                # notify server about the change
                if util.pidof(self.plugins[plugin_id]["name"]) is None:
                    self.agent.conn.send('plugin-stop plugin_id="%s"\n' % \
                                          plugin_id)
                    self.plugins[plugin_id]["start"] = 'no'

                
            elif command == 'plugin-enabled':
                
                self.agent.conn.send('plugin-enabled plugin_id="%s"\n' % \
                                     plugin_id)
                self.plugins[plugin_id]["enable"] = 'yes'

            elif command == 'plugin-disabled':
                
                self.agent.conn.send('plugin-disabled plugin_id="%s"\n' % \
                                     plugin_id)
                self.plugins[plugin_id]["enable"] = 'no'

        except IndexError:
            pass
        
        util.debug (__name__, 'monitor finished', '--')



