import mutex, time

import util
import Monitor
import MonitorNtopHostinfo
import MonitorNtopSession

class MonitorNtop(Monitor.Monitor):

    plugin_id = '2005'

    def __init__(self, agent, data, itime):
        self.m = mutex.mutex()
        Monitor.Monitor.__init__(self, agent, data, itime)


    def get_value(self, rule):
     
        #
        # Host Info Monitor
        #
        if int(rule["plugin_sid"]) < 245:

            # search url to connect ntop
            url = 'http://' + \
                self.plugins[MonitorNtop.plugin_id]['location'] + \
                '/dumpData.html?language=python'

            # lock
            while not self.m.testandset(): 
                time.sleep(0.1)
            value = MonitorNtopHostinfo.get_value(rule, url)
            self.m.unlock()
            # end lock
            
            return value
        
        #
        # Session Monitor
        #
        else:
            url = 'http://' + \
                self.plugins[MonitorNtop.plugin_id]['location'] + \
                 '/' + rule["from"] + '.html'
            
            # lock
            while not self.m.testandset():
                time.sleep(0.1)
            value = MonitorNtopSession.get_value(rule, url)
            self.m.unlock()
            # end lock
            
            return value


