import re
import string
import sys
import time
import threading


class Parser(threading.Thread):

    def __init__(self, conn, plugin):
        self.conn   = conn
        self.plugin = plugin
        threading.Thread.__init__(self)

    def run(self):
    
        if self.plugin["name"] == 'snort':
            from ProcessSnort import ProcessSnort
            snort = ProcessSnort(self.conn, self.plugin)
            snort.process()
            
        elif self.plugin["name"] == 'fw-1':
            from ProcessFW1 import ProcessFW1
            fw1 = ProcessFW1(self.conn, self.plugin)
            fw1.process()
            
        elif self.plugin["name"] == 'apache':
            from ProcessApache import ProcessApache
            apache = ProcessApache(self.conn, self.plugin)
            apache.process()
            
        elif self.plugin["name"] == 'iis':
            from ProcessIIS import ProcessIIS
            iis = ProcessIIS(self.conn, self.plugin)
            iis.process()
            
        elif self.plugin["name"] == 'iptables':
            from ProcessIptables import ProcessIptables
            iptables = ProcessIptables(self.conn, self.plugin)
            iptables.process()

        else:
            print "Plugin " + self.plugin["name"] + " is not implemented..."
            sys.exit()


    def sendMessage(self, type, date, sensor, plugin, tplugin, priority,
                     protocol, src_ip, src_port, dst_ip, dst_port):

        
        message = 'message [type='      + str(type)      + ']' +\
                          '[date="'     + str(date)      + '"]' +\
                          '[sensor='    + str(sensor)    + ']' +\
                          '[plugin='    + str(plugin)    + ']' +\
                          '[tplugin='   + str(tplugin)   + ']' +\
                          '[priority='  + str(priority)  + ']' +\
                          '[protocol='  + str(protocol)  + ']' +\
                          '[src_ip='    + str(src_ip)    + ']' +\
                          '[src_port='  + str(src_port)  + ']' +\
                          '[dst_ip='    + str(dst_ip)    + ']' +\
                          '[dst_port='  + str(dst_port)  + ']'

        print message + '\n' # debug
        self.conn.send(message + '\n')

