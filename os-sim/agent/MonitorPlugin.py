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
                util.debug_watchlog('starting plugin %s (%s) from web: %s' %\
                    (plugin_id, self.plugins[plugin_id]["process"], cmd), 
                    '->', 'YELLOW')
                os.system(cmd)
                
                # notify server about the change
                time.sleep(1)
                if util.pidof(self.plugins[plugin_id]["process"]) is not None:
                    self.agent.sendMessage('plugin-start plugin_id="%s"\n' % \
                                            plugin_id)
                    self.plugins[plugin_id]["start"] = 'yes'
                    
                    util.debug_watchlog('plugin %s (%s) started' % \
                        (plugin_id, self.plugins[plugin_id]["process"]),
                        '<-', 'GREEN')
                else:
                    util.debug_watchlog('error starting plugin %s' % \
                        (self.plugins[plugin_id]["process"]), '!!', 'RED')

                
            elif command == 'plugin-stop':
                
                # stop daemon
                cmd = self.plugins[plugin_id]["shutdown"]
                util.debug_watchlog('stopping plugin %s (%s) from web: %s' %\
                    (plugin_id, self.plugins[plugin_id]["process"], cmd),
                    '->', 'YELLOW')
                os.system(cmd)
                
                # notify server about the change
                if util.pidof(self.plugins[plugin_id]["process"]) is None:
                    self.agent.sendMessage('plugin-stop plugin_id="%s"\n' % \
                                          plugin_id)
                    self.plugins[plugin_id]["start"] = 'no'
                    
                    util.debug_watchlog('plugin %s (%s) stopped' % \
                        (plugin_id, self.plugins[plugin_id]["process"]),
                        '<-', 'RED')
                else:
                    util.debug_watchlog('error stopping plugin %s' % \
                        (self.plugins[plugin_id]["process"]), '!!', 'RED')
                    
                
            elif command == 'plugin-enabled':
                
                self.agent.sendMessage('plugin-enabled plugin_id="%s"\n' % \
                                     plugin_id)
                self.plugins[plugin_id]["enable"] = 'yes'

            elif command == 'plugin-disabled':
                
                self.agent.sendMessage('plugin-disabled plugin_id="%s"\n' % \
                                     plugin_id)
                self.plugins[plugin_id]["enable"] = 'no'

        except IndexError:
            pass
        
        util.debug (__name__, 'monitor finished', '--')



