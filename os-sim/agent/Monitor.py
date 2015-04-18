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
        self.watchdog_enable = agent.watchdog_enable
        self.watchdog_interval = agent.watchdog_interval

        threading.Thread.__init__(self)


    def split_data(self, data):
        
        pattern = 'plugin_id="([^"]*)"\s+plugin_sid="([^"]*)"\s+condition="([^"]*)"\s+value="([^"]*)"'
        patternInterval = 'interval="([^"]*)"'
        patternFrom = ' from="([^"]*)"'
        patternTo = ' to="([^"]*)"'
        patternAbsolute = ' absolute="([^"]*)"'
        patternPortFrom = ' port_from="([^"]*)"'
        patternPortTo = ' port_to="([^"]*)"'

        result = re.findall(str(pattern), data)
        resultInterval = re.findall(str(patternInterval), data)
        resultFrom = re.findall(str(patternFrom), data)
        resultTo = re.findall(str(patternTo), data)
        resultAbsolute = re.findall(str(patternAbsolute), data)
        resultPortFrom = re.findall(str(patternPortFrom), data)
        resultPortTo = re.findall(str(patternPortTo), data)

        try:
            (plugin_id, plugin_sid, condition, value) = result[0]
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
            
        try:
            info["port_from"] = resultPortFrom[0]
        except IndexError:
            pass
            
        try:
            info["port_to"] = resultPortTo[0]
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
       
        # watchdog monitor
        if self.agent.watchdog_enable:
            from MonitorWatchdog import MonitorWatchdog
            watchdog = MonitorWatchdog(self.agent)
            watchdog.start()
        
        while 1:
            data = self.recv_line(self.agent.conn)
            if not data: break;
            
#            print " * server said: " + str(data) # debug
#            TODO: CHECK FOR ERROR MESSAGES

            try:
                if data.__contains__('watch-rule'):

                    # ntop monitor
                    if data.__contains__('plugin_id="2005"'):
                        if self.plugins['2005']["enable"] == 'yes':
                            from MonitorNtop import MonitorNtop
                            ntop = MonitorNtop(self.agent, data)
                            ntop.start()
                        else:
                            util.debug (__name__, 'plugin NTOP is disabled',
                                        '**', 'RED');
                        
                    # C & A levels monitor
                    elif data.__contains__('plugin_id="2001"'):
                        if self.plugins['2001']["enable"] == 'yes':
                            from MonitorCA import MonitorCA
                            ca = MonitorCA(self.agent, data)
                            ca.start()
                        else:
                            util.debug (__name__, 'plugin CA is disabled',
                                        '**', 'RED');

                    # OpenNMS monitor
                    elif data.__contains__('plugin_id="2004"'):
                        if self.plugins['2004']["enable"] == 'yes':
                            from MonitorOpennms import MonitorOpennms
                            opennms = MonitorOpennms(self.agent, data)
                            opennms.start()
                        else:
                            util.debug (__name__, 'plugin Opennms is disabled',
                                        '**', 'RED');


                elif data.__contains__('plugin-start') or \
                   data.__contains__('plugin-stop') or \
                   data.__contains__('plugin-enabled') or \
                   data.__contains__('plugin-disabled'):

                    # Plugin monitor
                    from MonitorPlugin import MonitorPlugin
                    mp = MonitorPlugin(self.agent, data)
                    mp.start()

            except Exception, e:
                util.debug (__name__, e, '!!', 'RED')


