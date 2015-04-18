import sys
import time

import Monitor
import util

from pyPgSQL import PgSQL

class MonitorOpennms(Monitor.Monitor):

    plugin_id = '2004'

    def run(self):
    
        util.debug (__name__, "monitor started", '--')
        rule = self.split_data(self.data)
        util.debug (__name__, "request received... (%s)" % str(rule), 
                    '<=', 'GREEN')
        
        if rule is not None:
            self.__evaluate(rule = rule)
                
        util.debug (__name__, 'monitor finished', '--')


    def __get_value(self, st, rule):

        # get plugin_sid
        plugin_sid = rule['plugin_sid']
      
        # service available?
        if plugin_sid == '1':
            
            query = """SELECT qualifier FROM ifservices 
                            WHERE ipaddr = '%s' AND qualifier = '%s' 
                                AND status = 'A""" % \
                        (rule["from"], rule["port_from"])
                
        # service down?
        elif plugin_sid == '2':
            
            query = """SELECT qualifier FROM ifservices 
                            WHERE ipaddr = '%s' AND qualifier = '%s' 
                                AND status = 'D'""" % \
                        (rule["from"], rule["port_from"])

        else:
            util.debug(__name__, "Unknown plugin_sid: %s" % plugin_sid, 
                       "**", "RED")
            util.debug (__name__, 'monitor finished', '--')
            sys.exit()

            
        st.execute(query)
        res = st.fetchone()
            
        if res is not None: 
            return True
        else:
            return False                


    def __evaluate(self, rule):

        # database connect
        #
        # dbconfig[0] => database
        # dbconfig[1] => host
        # dbconfig[2] => db
        # dbconfig[3] => user
        # dbconfig[4] => pass
        dbconfig = self.plugins[MonitorOpennms.plugin_id]['location'].split(':')
        
        if dbconfig[0] == 'pgsql':

            if dbconfig[4]:
                db = PgSQL.connect (host     = dbconfig[1],
                                    database = dbconfig[2],
                                    user     = dbconfig[3],
                                    password = dbconfig[4])
            else:
                db = PgSQL.connect (host     = dbconfig[1],
                                    database = dbconfig[2],
                                    user     = dbconfig[3])
            st = db.cursor()        
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');
            sys.exit()

        pfreq = int(self.plugins[MonitorOpennms.plugin_id]['frequency'])
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


            util.debug (__name__, "getting Opennms value...", '<=')
            value = self.__get_value(st, rule)

            if value:

                sensor = self.plugins[MonitorOpennms.plugin_id]['sensor']
                interface = self.plugins[MonitorOpennms.plugin_id]['interface']
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
                                       dst_ip       = '',
                                       dst_port     = '',
                                       condition    = '',
                                       value        = '')
                
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


