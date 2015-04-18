import threading
import re
import socket
import time

import util

class Monitor(threading.Thread):
    
    def __init__(self, agent, data = ''):
        self.agent  = agent

        # link agent atributes and methods to use them. why???
        self.sequence  = agent.sequence
        self.plugins   = agent.plugins
        self.data      = data

        threading.Thread.__init__(self)


    def split_data(self, data):
        
        pattern = 'plugin_id="([^"]*)"\s+plugin_sid="([^"]*)"\s+condition="([^"]*)"\s+value="([^"]*)"\s+port_from="([^"]*)"\s+port_to="([^"]*)"'
        patternInterval = 'interval="([^"]*)"'
        patternFrom = ' from="([^"]*)"'
        patternTo = ' to="([^"]*)"'
        patternAbsolute = ' absolute="([^"]*)"'

        result = re.findall(str(pattern), data)
        resultInterval = re.findall(str(patternInterval), data)
        resultFrom = re.findall(str(patternFrom), data)
        resultTo = re.findall(str(patternTo), data)
        resultAbsolute = re.findall(str(patternAbsolute), data)

        try:
            (plugin_id, plugin_sid, condition, value, 
             port_from, port_to) = result[0]
            info = {"plugin_id" : plugin_id, 
                    "plugin_sid" : plugin_sid, 
                    "condition" : condition,
                    "value" : value, 
                    "port_from" : port_from,
                    "port_to" : port_to,
                    "interval" : '',
                    "from" : '',
                    "to" : '',
                    "absolute": ''}
        except IndexError:
            return None

        try:
            info["interval"] = resultInterval[0]
        except IndexError:
            pass
        
        try:
            info["from"] = resultFrom[0]
        except IndexError:
            pass
        
        try:
            info["to"] = resultTo[0]
        except IndexError:
            pass

        try:
            info["absolute"] = resultAbsolute[0]
        except IndexError:
            pass
        
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
                util.debug (__name__, 'Error receiving from server',
                            '!!', 'RED')
                time.sleep(10)
                conn = self.agent.reconnect()
                

        return data

    def run(self):
        while 1:
            data = self.recv_line(self.agent.conn)
            if not data: break;
            
#            print " * server said: " + str(data) # debug
#            TODO: CHECK FOR ERROR MESSAGES

            if data.__contains__('watch-rule'):

                # ntop
                if data.__contains__('plugin_id="2005"'):
                    from MonitorNtop import MonitorNtop
                    ntop = MonitorNtop(self.agent, data)
                    ntop.start()

