import sys, time, re, os, threading

import util

class Watchdog(threading.Thread):

    def __init__(self, agent):
        self.agent = agent
        self.plugins = agent.plugins
        self.watchdog_interval = agent.watchdog_interval
        threading.Thread.__init__(self)

    def run(self):

        util.debug (__name__, "monitor started", '--')
        while 1:
            
            for id, plugin  in self.plugins.iteritems():

                # unkown process to monitoring
                if not plugin["process"]:

                    # debug
                    msg = 'plugin %s has an unknown state' % (id)
                    util.debug_watchlog (msg, '--', 'YELLOW')

                    # unknown plugin state
                    msg = "plugin-unknown plugin_id=\"%s\"\n" % id
                    self.agent.sendMessage(msg)


                # process is alive
                elif util.pidof(plugin["process"]) is not None:
                    
                    # debug
                    msg = 'plugin %s (%s) is running' % (id, plugin["process"])
                    util.debug_watchlog (msg, '--', 'GREEN')
                    
                    # plugin started
                    msg = "plugin-start plugin_id=\"%s\"\n" % id
                    self.agent.sendMessage(msg)


                # restart daemon
                elif plugin["start"] == 'yes':
                       
                    # debug
                    msg = 'plugin %s (%s) is not running' % \
                        (id, plugin["process"])
                    util.debug_watchlog (msg, '--', 'RED')

                    # startup command
                    cmd = plugin["startup"]
                    os.system(cmd)
                    
                    # debug
                    msg = 'starting service %s (%s): %s' % \
                        (id, plugin["process"], cmd)
                    util.debug_watchlog (msg, '->', 'YELLOW')

                    # notify result to server
                    time.sleep(1)
                    if util.pidof(plugin["process"]) is not None:
                        msg = "plugin-start plugin_id=\"%s\"\n" % id
                        util.debug_watchlog('service %s (%s) started' % \
                            (id, plugin["process"]), '<-', 'GREEN')
                    else:
                        msg = "plugin-stop plugin_id=\"%s\"\n" % id
                        util.debug_watchlog('error starting service %s' % \
                            (plugin["process"]), '!!', 'RED')
                    self.agent.sendMessage(msg)
                        
                else:
                    # debug
                    msg = 'plugin %s (%s) is not running' % \
                        (id, plugin["process"])
                    util.debug_watchlog (msg, '--', 'YELLOW')

                    msg = "plugin-stop plugin_id=\"%s\"\n" % id
                    self.agent.sendMessage(msg)

            time.sleep(float(self.watchdog_interval))

