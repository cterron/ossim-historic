import sys
import time

import Parser
import util

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
            import _mysql
            
            db=_mysql.connect(host   = dbconfig[1],
                              db     = dbconfig[2],
                              user   = dbconfig[3],
                              passwd = dbconfig[4])
 
        else:
            util.debug (__name__, 'database %s not supported' % (dbconfig[0]),
                        '--', 'RED');
        
            sys.exit()

        # obtain host thresholds
        host_info = {}
        db.query("""SELECT ip, threshold_c, threshold_a FROM host""")
        r = db.store_result()
        while 1:
            
            if self.plugin["enable"] == 'no':

                # plugin disabled, wait for enabled
                util.debug (__name__, 'plugin disabled', '**', 'RED')
                while self.plugin["enable"] == 'no':
                    time.sleep(1)
                    
                # lets parse again
                util.debug (__name__, 'plugin enabled', '**', 'GREEN')

            try:
                h = r.fetch_row(maxrows = 1, how = 1)
                host_info[h[0]["ip"]] = h[0]
            except IndexError:
                break

        # obtain default thresholds
        db.query ("""SELECT * FROM conf""")
        r = db.store_result()
        conf = r.fetch_row(maxrows = 1, how = 1)
        default_threshold = int(conf[0]["threshold"])

        while 1:
            
            # get actual qualification info
            db.query("""SELECT * FROM host_qualification""")
            r = db.store_result()
            result = r.fetch_row(maxrows = 0, how = 1)
            
            # host exceeded its threshold?
            for host in result:
                try:
                    if int(host["compromise"]) > \
                       int(host_info[host["host_ip"]]["threshold_c"]):

                        diff = int(host["compromise"]) / \
                            int(host_info[host["host_ip"]]["threshold_c"])
                        self.exceeded(host, sid = 1, diff = diff)

                    if int(host["attack"]) > \
                       int(host_info[host["host_ip"]]["threshold_a"]):

                        diff = int(host["attack"]) / \
                            int(host_info[host["host_ip"]]["threshold_a"])
                        self.exceeded(host, sid = 2, diff = diff)

                except KeyError:
                    if int(host["compromise"]) > default_threshold:

                        diff = int(host["compromise"]) / default_threshold
                        self.exceeded(host, sid = 1, diff = diff)

                    if int(host["attack"]) > default_threshold:

                        diff = int(host["attack"]) / default_threshold
                        self.exceeded(host, sid = 2, diff = diff)

            time.sleep(float(self.plugin["frequency"]))

        db.close()


    def exceeded (self, host, sid, diff):

        date = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))

        if diff >= 5:   priority = 5
        elif diff >= 4: priority = 4
        elif diff >= 3: priority = 3
        elif diff >= 2: priority = 2
        elif diff >= 1: priority = 1

        self.agent.sendMessage(type = 'detector',
                         date       = date,
                         sensor     = self.plugin["sensor"],
                         interface  = self.plugin["interface"],
                         plugin_id  = self.plugin["id"],
                         plugin_sid = sid,
                         priority   = priority,
                         protocol   = '',
                         src_ip     = host["host_ip"],
                         src_port   = '',
                         dst_ip     = '',
                         dst_port   = '')

