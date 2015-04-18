import threading, time

import util

class Scheduler(threading.Thread):

    def __init__(self, agent):
        self.mlist = agent.mlist
        threading.Thread.__init__(self)
    
    def run(self):

        while 1:
            
            monitor = None
            count = 1
            
            if len(self.mlist) > 0:

                for monitor in self.mlist:
                    
                    util.debug(__name__, 
                               "MonitorList : processing element (%d/%d)..." %\
                               (count, len(self.mlist)), "**", "PURPLE")
                    count += 1
                    
                    # get monitor from monitor list
                    # process watch-rule and remove from list
                    if monitor.process():
                        self.mlist.removeRule(monitor)

            # don't overload agent
            time.sleep(2)

