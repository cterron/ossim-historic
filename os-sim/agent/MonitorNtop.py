import time
import sys

import Monitor
import MonitorNtopHostinfo
import MonitorNtopSession
import util

class MonitorNtop(Monitor.Monitor):

    plugin_id = '2005'

    def run(self):
    
        util.debug (__name__, "monitor started", '--')
        rule = self.split_data(self.data)
        util.debug (__name__, "request received... (%s)" % str(rule), 
                    '<=', 'GREEN')
        
        if rule is not None:
   
            # absolute
            if rule["absolute"] == 'true' or \
               rule["absolute"] == 'yes' or \
               rule["absolute"] == '1':
                self.__evaluate(rule = rule, absolute = 'true')

            # relative
            else:
                self.__evaluate(rule = rule)
                
        util.debug (__name__, 'monitor finished', '--')


    def __get_value(self, rule):
     
        #
        # Host Info Monitor
        #
        if int(rule["plugin_sid"]) < 245:

            # search url to connect ntop
            url = 'http://' + \
                self.plugins[MonitorNtop.plugin_id]['location'] + \
                '/dumpData.html?language=xml'

            return MonitorNtopHostinfo.get_value(rule, url)
        
        #
        # Session Monitor
        #
        else:
            url = 'http://' + \
                self.plugins[MonitorNtop.plugin_id]['location'] + \
                '/NetNetstat.html'
            return MonitorNtopSession.get_value(rule, url)


    def __evaluate(self, rule, absolute = ""):

        if absolute == 'true':
            vfirst = 0
        else:
            try:
                vfirst = int(self.__get_value(rule))
                if vfirst is None: vfirst = 0
            except TypeError:
                vfirst = 0

        pfreq = int(self.plugins[MonitorNtop.plugin_id]['frequency'])
        f = 0

        while 1:

            if rule["interval"] != '':

                #  calculate time to sleep
                if int(rule["interval"]) < pfreq:
                    util.debug (__name__, 
                                "waiting %d secs..." % int(rule["interval"]),
                                '**')
                    time.sleep(float(rule["interval"]))
                else:
                    if int(rule["interval"]) < f + pfreq:
                        util.debug (__name__,
                            "waiting %d secs..." % (int(rule["interval"])-f),
                            '**')
                        time.sleep(int(rule["interval"]) - f)
                    else:
                        util.debug (__name__, "waiting %d secs..." % pfreq,
                                    '**')
                        time.sleep(pfreq)


            util.debug (__name__, "getting ntop value...", '<=')
            vlast = self.__get_value(rule)
            if vlast is None:
                util.debug (__name__, "no data for %s" % rule["to"],
                            '!!', 'YELLOW')
                break

            if ((rule["condition"] == 'eq') and \
                 (vlast == vfirst + int(rule["value"])) or \
                (rule["condition"] == 'ne') and \
                 (vlast != vfirst + int(rule["value"])) or \
                (rule["condition"] == 'gt') and \
                 (vlast > vfirst + int(rule["value"])) or \
                (rule["condition"] == 'ge') and \
                 (vlast >= vfirst + int(rule["value"])) or \
                (rule["condition"] == 'le') and \
                 (vlast <= vfirst + int(rule["value"])) or \
                (rule["condition"] == 'lt') and \
                 (vlast < vfirst + int(rule["value"]))):

                sensor = self.plugins[MonitorNtop.plugin_id]['sensor']
                interface = self.plugins[MonitorNtop.plugin_id]['interface']
                date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.localtime(time.time()))

                self.agent.sendMessage(type         = 'monitor', 
                                       date         = date, 
                                       sensor       = sensor, 
                                       interface    = interface,
                                       plugin_id    = rule["plugin_id"], 
                                       plugin_sid   = rule["plugin_sid"],
                                       priority     = '', 
                                       protocol     = 'tcp', 
                                       src_ip       = rule["from"],
                                       src_port     = rule["port_from"], 
                                       dst_ip       = rule["to"], 
                                       dst_port     = rule["port_to"],
                                       condition    = rule["condition"],
                                       value        = rule["value"])
                
                break # alert sent, finished
                
            else:
                util.debug (__name__, 'No alert', '--', 'GREEN')
            
            f += pfreq
            if f >= int(rule["interval"]):  # finish if interval exceded
                break
 


