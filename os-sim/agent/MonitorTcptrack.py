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
            util.debug(__name__, "Not enougth data", "!!", "YELLOW")
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
            conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            conn.connect((tcptrack_ip, int(tcptrack_port)))
            conn.send(query + "\n")

            # get session data
            #
            data = conn.recv(1024)
            util.debug (__name__, data, "=>", "CYAN")
            sid = int(rule["plugin_sid"]) - 1
            return data.split()[sid]

        except socket.error, e:
            util.debug(__name__, "Socket Error (" + str(e) + ")",
                       "!!", "RED")
            return None

        except Exception, e:
            util.debug (__name__, e, '!!', 'RED')
            print >> sys.stderr, __name__, ": Unexpected exception:", e
            return None

