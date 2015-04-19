import sys
import MySQLdb

import util
import Monitor

class MonitorCA(Monitor.Monitor):

    plugin_id = '2001'

    def __init__(self, agent, data, itime):
        self.db = None  # Database connection
        self.st = None  # Database cursor
        Monitor.Monitor.__init__(self, agent, data, itime)

    def get_value(self, rule):

        if self.db is None or self.st is None:
            self.connect()

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

        self.st.execute(query)
        res = self.st.fetchone()
        if res is not None:
            return res[0]
        else:
            return 0


    def connect(self):
        
        # database connect
        #
        # dbconfig[0] => database
        # dbconfig[1] => host
        # dbconfig[2] => db
        # dbconfig[3] => user
        # dbconfig[4] => pass
        dbconfig = self.plugins[MonitorCA.plugin_id]['location'].split(':')
        
        if dbconfig[0] == 'mysql':
            
            self.db = MySQLdb.connect(host   = dbconfig[1],
                                      db     = dbconfig[2],
                                      user   = dbconfig[3],
                                      passwd = dbconfig[4])
            self.st = self.db.cursor()
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');


    def close(self):
       self.db.close() 
