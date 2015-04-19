import threading, re, socket, string, time

import Plugin
import util

class Server(threading.Thread):
    
    def __init__(self, agent):
        self.agent  = agent

        # link agent atributes and methods to use them. why???
        self.sequence  = agent.sequence
        self.plugins   = agent.plugins
        self.mlist     = agent.mlist

        threading.Thread.__init__(self)


    def split_data(self, data):
       

        pattern = {
            'main':         'plugin_id="([^"]*)"\s+' + \
                            'plugin_sid="([^"]*)"\s+' + \
                            'condition="([^"]*)"\s+' + \
                            'value="([^"]*)"',
            'interval':     ' interval="([^"]*)"',
            'from':         ' from="([^"]*)"',
            'to':           ' to="([^"]*)"',
            'absolute':     ' absolute="([^"]*)"',
            'port_from':    ' port_from="([^"]*)"',
            'port_to':      ' port_to="([^"]*)"'
        }

        result = {}
        for atr in ["main", "interval", "from", "to", 
                    "absolute", "port_from", "port_to"]:
            result[atr] = re.findall(str(pattern[atr]), data)

        # mandatory arguments
        try:
            (plugin_id, plugin_sid, condition, value) = result["main"][0]
            info = {"plugin_id" : plugin_id, 
                    "plugin_sid" : plugin_sid, 
                    "condition" : condition,
                    "value" : value, 
                    "port_from" : '',
                    "port_to" : '',
                    "interval" : '',
                    "from" : '',
                    "to" : '',
                    "absolute": ''}
        except IndexError:
            return None

        # optional arguments
        for op in ["interval", "from", "to", "absolute", 
                   "port_from", "port_to"]:

            if result[op] != []:
                info[op] = result[op][0]
        
        return info


    def recv_line(self, conn):
        char = data = ''
        while 1:
            try:
                char = conn.recv(1)
                data += char
                if char == '\n': break;
            except socket.error:
                util.debug (__name__,  'Error receiving from server', 
                            '!!', 'RED')
                time.sleep(10)
                conn = self.agent.reconnect()
            except AttributeError:
                util.debug (__name__,  'Error receiving from server', 
                            '!!', 'RED')
                time.sleep(10)
                conn = self.agent.reconnect()
                
        return data


    def run(self):
        
        while 1:
            

            try:
            
                data = self.recv_line(self.agent.conn)
#               TODO: CHECK FOR ERROR MESSAGES

                if data.__contains__('watch-rule') and \
                   len(self.mlist) < self.mlist.MAX_SIZE:
        
                    util.debug(__name__, 
                               "Watch-rule received: [%s]  (bufsize=%d)" % \
                               (string.strip(data), len(self.mlist)), 
                               "<-", "GREEN")

                    rule_info = self.split_data(data)
                    
                    # ntop monitor
                    if data.__contains__('plugin_id="2005"') and \
                      self.plugins['2005']["enable"] in ['yes', 'true']:
                            
                        from MonitorNtop import MonitorNtop
                        self.mlist.appendRule( \
                            MonitorNtop(agent = self.agent, 
                                        data  = rule_info,
                                        itime = int(time.time())))

                    # CA monitor
                    elif data.__contains__('plugin_id="2001"') and \
                      self.plugins['2001']["enable"] in ['yes', 'true']:
                            
                        from MonitorCA import MonitorCA
                        self.mlist.appendRule( \
                            MonitorCA(agent = self.agent, 
                                      data  = rule_info,
                                      itime = int(time.time())))

                    # OpenNMS monitor
                    elif data.__contains__('plugin_id="2004"') and \
                      self.plugins['2004']["enable"] in ['yes', 'true']:
                            
                        from MonitorOpennms import MonitorOpennms
                        self.mlist.appendRule( \
                            MonitorOpennms(agent = self.agent, 
                                           data  = rule_info,
                                           itime = int(time.time())))

                    # tcptrack monitor
                    if data.__contains__('plugin_id="2006"') and \
                      self.plugins['2006']["enable"] in ['yes', 'true']:

                        from MonitorTcptrack import MonitorTcptrack
                        self.mlist.appendRule( \
                            MonitorTcptrack(agent = self.agent,
                                            data  = rule_info,
                                            itime = int(time.time())))

                elif data.__contains__('plugin-start') or \
                   data.__contains__('plugin-stop') or \
                   data.__contains__('plugin-enabled') or \
                   data.__contains__('plugin-disabled'):

                    # Plugin monitor
                    mp = Plugin.Plugin(self.agent, data)
                    mp.start()

            except Exception, e:
                util.debug (__name__, e, '!!', 'RED')
                print >> sys.stderr, __name__, "Unexpected exception:", e


