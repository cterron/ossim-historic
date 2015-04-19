import sys
import time

import Parser
import util

try:
    import MySQLdb
except ImportError, e:
    util.debug (__name__, "Import error: %s" % (e), "**", "RED")

class ParserCA(Parser.Parser):

    def process(self):

        if self.plugin["source"] == 'database':
            self.__processDatabase()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for CA...", '!!', 'RED')
            sys.exit()

    def __processDatabase(self):
        
        util.debug ('ParserCA', 'plugin started (database)...', '--')
        
        # database connect
        #
        # dbconfig[0] => database
        # dbconfig[1] => host
        # dbconfig[2] => db
        # dbconfig[3] => user
        # dbconfig[4] => pass
        dbconfig = self.plugin['location'].split(':')

        if dbconfig[0] == 'mysql':
            
            try:
                db = MySQLdb.connect(host   = dbconfig[1],
                                     db     = dbconfig[2],
                                     user   = dbconfig[3],
                                     passwd = dbconfig[4])
                st = db.cursor()
            except Exception, e:
                util.debug (__name__, e, "**", "RED")
                sys.exit()
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');
        
            sys.exit()

        # obtain host thresholds
        host_info = {}
        st.execute("""SELECT ip, threshold_c, threshold_a FROM host""")
        res = st.fetchmany()
        for h in res:
            
            if self.plugin["enable"] == 'no':

                # plugin disabled, wait for enabled
                util.debug (__name__, 'plugin disabled', '**', 'YELLOW')
                while self.plugin["enable"] == 'no':
                    time.sleep(1)
                    
                # lets parse again
                util.debug (__name__, 'plugin enabled', '**', 'GREEN')

            try:
                host_info[h[0]] = h
            except IndexError:
                break

        # obtain default thresholds
        st.execute ("""SELECT value as threshold FROM config WHERE conf = 'threshold'""")
        res = st.fetchone()
        default_threshold = int(res[0])

        while 1:
            
            # get actual qualification info
            st.execute("""SELECT host_ip, compromise, attack 
                          FROM host_qualification""")
            res = st.fetchmany()
            
            # host exceeded its threshold?
            for host in res:
                # host[0] -> ip
                # host[1] -> compromise
                # host[2] -> attack
                # host_info[x][0] -> ip
                # host_info[x][1] -> threshold_c
                # host_info[x][2] -> threshold_a
                try:
                    if int(host[1]) > \
                       int(host_info[host[0]][1]):

                        diff = int(host[1]) / \
                            int(host_info[host[0]][1])
                        self.exceeded(host[0], sid = 1, diff = diff)

                    if int(host[2]) > \
                       int(host_info[host[0]][2]):

                        diff = int(host[2]) / \
                            int(host_info[host[0]][2])
                        self.exceeded(host[0], sid = 2, diff = diff)

                except KeyError:
                    if int(host[1]) > default_threshold:

                        diff = int(host[1]) / default_threshold
                        self.exceeded(host[0], sid = 1, diff = diff)

                    if int(host[2]) > default_threshold:

                        diff = int(host[2]) / default_threshold
                        self.exceeded(host[0], sid = 2, diff = diff)

            time.sleep(float(self.plugin["frequency"]))

        db.close()


    def exceeded (self, ip, sid, diff):

        date = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))

        if diff >= 5:   priority = 5
        elif diff >= 4: priority = 4
        elif diff >= 3: priority = 3
        elif diff >= 2: priority = 2
        elif diff >= 1: priority = 1

        self.agent.sendAlert  (type = 'detector',
                         date       = date,
                         sensor     = self.plugin["sensor"],
                         interface  = self.plugin["interface"],
                         plugin_id  = self.plugin["id"],
                         plugin_sid = sid,
                         priority   = priority,
                         protocol   = '',
                         src_ip     = ip,
                         src_port   = '',
                         dst_ip     = '',
                         dst_port   = '')


