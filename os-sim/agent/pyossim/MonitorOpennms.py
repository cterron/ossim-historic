import util
import Monitor

try:
    from pyPgSQL import PgSQL
except ImportError:
    util.debug(__name__, 
        "You need to install pyPgSQL in order to use OpenNMS Monitor\n" + \
        "\tLook at: http://pypgsql.sourceforge.net/ ", "!!", "RED")

class MonitorOpennms(Monitor.Monitor):

    plugin_id = '2004'

    def __init__(self, agent, data, itime):
        self.db = None  # Database connection
        self.st = None  # Database cursor
        Monitor.Monitor.__init__(self, agent, data, itime)

    def get_value(self, rule):

        if self.db is None or self.st is None:
            self.connect()

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

            
        self.st.execute(query)
        res = self.st.fetchone()
            
        if res is not None: 
            return 1
        else:
            return 0


    def eval_condition(self, cond, arg1, arg2, value):
        
        if cond == "eq":
            if int(arg2) == value:
                return True
            else:
                return False
        else:
            return False


    def connect(self):
        
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
                self.db = PgSQL.connect (host     = dbconfig[1],
                                         database = dbconfig[2],
                                         user     = dbconfig[3],
                                         password = dbconfig[4])
            else:
                self.db = PgSQL.connect (host     = dbconfig[1],
                                         database = dbconfig[2],
                                         user     = dbconfig[3])
            self.st = self.db.cursor()
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');


    def close(self):
       self.db.close() 

