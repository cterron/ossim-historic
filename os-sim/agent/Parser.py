import sys
import threading

class Parser(threading.Thread):

    def __init__(self, agent, plugin):
        self.agent  = agent

        # link agent atributes and methods to use them. why???
        self.conn      = agent.conn
        self.reconnect = agent.reconnect
        self.my_ip     = agent.my_ip

        self.plugin = plugin
        threading.Thread.__init__(self)

    def run(self):
    
        if self.plugin["id"] == '1001':
            from ParserSnort import ParserSnort
            snort = ParserSnort(self.agent, self.plugin)
            snort.process()
            
        elif self.plugin["id"] == '1504':
            from ParserFW1 import ParserFW1
            fw1 = ParserFW1(self.agent, self.plugin)
            fw1.process()
            
        elif self.plugin["id"] == '1501':
            from ParserApache import ParserApache
            apache = ParserApache(self.agent, self.plugin)
            apache.process()
            
        elif self.plugin["id"] == '1502':
            from ParserIIS import ParserIIS
            iis = ParserIIS(self.agent, self.plugin)
            iis.process()
            
        elif self.plugin["id"] == '1503':
            from ParserIptables import ParserIptables
            iptables = ParserIptables(self.agent, self.plugin)
            iptables.process()

        else:
            print "Plugin " + self.plugin["name"] + " is not implemented..."
            sys.exit()


