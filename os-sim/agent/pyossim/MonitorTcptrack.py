import sys, socket

import Monitor
import util

class MonitorTcptrack(Monitor.Monitor):

    plugin_id = '2006'

    def get_value(self, rule):

        # get config
        #
        location = self.plugins[MonitorTcptrack.plugin_id]['location']
        config = location.split(':')
        (tcptrack_ip, tcptrack_port) = config

        
        # src_ip, src_port, dst_ip and dst_port are mandatory
        #
        if not (rule["from"] and rule["port_from"] and \
                rule["to"] and rule["port_to"]):
            util.debug(__name__, "Bad request: " +\
                "from:port_from to:port_to are mandatory", 
                "!!", "YELLOW")
            return None


        # query to tcptrack:
        # src_ip:src_port dst_ip:dst_port
        #
        query = "%s:%s %s:%s" % (rule["from"], rule["port_from"], 
                                 rule["to"], rule["port_to"])
        util.debug(__name__, query, "=>", "GREEN")


        try:

            # connect to tcptrack and send query
            #
            try:
                conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                conn.connect((tcptrack_ip, int(tcptrack_port)))
            except socket.error:
                util.debug(__name__, "Connection refused (%s:%s)" %\
                    (tcptrack_ip, tcptrack_port), "**", "RED")
                return None
            conn.send(query + "\n")

            # get session data
            #
            data = conn.recv(1024)
            conn.shutdown(2)
            conn.close()
            util.debug (__name__, data, "=>", "CYAN")

            # obtain tcptrack sid from array index
            # 1: Data Sent
            # 2: Data Recv
            # 3: Session Duration
            #
            sid = int(rule["plugin_sid"]) - 1
            value = data.split()[sid]
            if int(value) >= 0:
                return value
            else:
                return None # Error

        except socket.error, e:
            util.debug(__name__, "Socket Error (" + str(e) + ")",
                       "!!", "RED")
            conn.shutdown(2)
            conn.close()
            return None

        except Exception, e:
            util.debug (__name__, e, '!!', 'RED')
            conn.shutdown(2)
            conn.close()
            print >> sys.stderr, __name__, ": Unexpected exception:", e
            return None

