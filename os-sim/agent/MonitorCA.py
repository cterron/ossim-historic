import sys
import time

import Monitor
import util

import MySQLdb

class MonitorCA(Monitor.Monitor):

    plugin_id = '2001'

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


    def __get_value(self, st, rule):

        # get plugin_sid
        plugin_sid = rule['plugin_sid']

        # compromise
        if plugin_sid == '1':
            query = """SELECT compromise FROM host_qualification 
                            WHERE host_ip = '%s'""" % rule["from"]
        # attack
        elif plugin_sid == '2':
            query = """SELECT attack FROM host_qualification 
                            WHERE host_ip = '%s'""" % rule["from"]
        else:
            util.debug(__name__, "Unknown plugin_sid: %s" % plugin_sid, "**", "RED")
            util.debug (__name__, 'monitor finished', '--')
            sys.exit()

        st.execute(query)
        res = st.fetchone()
        if res is not None:
            return res[0]
        else:
            return 0


    def __evaluate(self, rule, absolute = ""):

        # database connect
        #
        # dbconfig[0] => database
        # dbconfig[1] => host
        # dbconfig[2] => db
        # dbconfig[3] => user
        # dbconfig[4] => pass
        dbconfig = self.plugins[MonitorCA.plugin_id]['location'].split(':')
        
        if dbconfig[0] == 'mysql':
            
            db = MySQLdb.connect(host   = dbconfig[1],
                                 db     = dbconfig[2],
                                 user   = dbconfig[3],
                                 passwd = dbconfig[4])
            st = db.cursor()
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');
            sys.exit()


        if absolute == 'true':
            vfirst = 0
        else:
            vfirst = int(self.__get_value(st, rule))
            if vfirst is None: vfirst = 0

        pfreq = int(self.plugins[MonitorCA.plugin_id]['frequency'])
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


            util.debug (__name__, "getting CA value...", '<=')
            vlast = self.__get_value(st, rule)
            if vlast is None:
                util.debug (__name__, "no data for %s" % rule["to"],
                            '!!', 'YELLOW')

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

                sensor = self.plugins[MonitorCA.plugin_id]['sensor']
                interface = self.plugins[MonitorCA.plugin_id]['interface']
                date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.localtime(time.time()))

                self.agent.sendAlert  (type         = 'monitor', 
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
                if rule["interval"] == '': # no alert, finished
                    break
            
            if rule["interval"] != '':
                f += pfreq
                if f >= int(rule["interval"]):  # finish if interval exceded
                    break
        
        db.close()


