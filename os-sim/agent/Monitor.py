import util, time, sys

class Monitor:

    # MUST be called in child class
    def __init__(self, agent, data, itime):
        
        self.agent  = agent  # agent pointer
        self.data   = data   # watch rule info
        self.itime  = itime  # insert time

        self.plugins  = agent.plugins

        self.vfirst = None
        self.vlast  = None

    # MUST be overriden in child class
    def get_value(self, data):
        pass

    # object free if needed
    def close(self):
        pass

    # MAY be overriden in child class
    def evaluate(self):

        self.vlast = self.get_value(self.data)
        if self.vlast is None:
            util.debug (__name__, "no data for %s" % self.data["from"],
                        '!!', 'YELLOW')
            return True

        if self.eval_condition(cond  = self.data["condition"], 
                               arg1  = self.vfirst,
                               arg2  = self.vlast,
                               value = int(self.data["value"])):
            self.sendAlert()
            return True

        else:
            return False


    def process(self):

        try:
            util.debug(__name__, "Processing watch-rule (id=%s sid=%s)" % \
                       (self.data["plugin_id"], self.data["plugin_sid"]), 
                       "<-", "YELLOW")

            # obtain first value to compare
            if self.vfirst is None:
                if self.data["absolute"] in ["yes", "true"]:
                    self.vfirst = 0
                else:
                    try:
                        self.vfirst = int(self.get_value(self.data))
                        if self.vfirst is None: self.vfirst = 0
                    except TypeError:
                        self.vfirst = 0

            # get actual time
            atime = int(time.time())

            if self.data["interval"] == '':
                self.evaluate()
                return True # finished

            elif (self.itime + int(self.data["interval"]) > atime):
                util.debug (__name__, 
                            "Timeout at %d seconds" % \
                            (self.itime + int(self.data["interval"]) - atime), 
                            "<-", "YELLOW")
                return self.evaluate()

            else: 
                # out of time
                self.evaluate()
                return True # finished

        except Exception, e:
            util.debug (__name__, e, '!!', 'RED')
            print >> sys.stderr, __name__, ": Unexpected exception:", e
            return True


    def eval_condition(self, cond, arg1, arg2, value):

        if cond == "eq":
            return (int(arg2) == int(arg1) + int(value))
        elif cond == "ne":
            return (int(arg2) != int(arg1) + int(value))
        elif cond == "gt":
            return (int(arg2) > int(arg1) + int(value))
        elif cond == "ge":
            return (int(arg2) >= int(arg1) + int(value))
        elif cond == "le":
            return (int(arg2) <= int(arg1) + int(value))
        elif cond == "lt":
            return (int(arg2) < int(arg1) + int(value))
        else:
            return False


    def sendAlert(self):
        plugin_id = self.data["plugin_id"]

        sensor = self.plugins[plugin_id]['sensor']
        interface = self.plugins[plugin_id]['interface']
        date = time.strftime('%Y-%m-%d %H:%M:%S', 
                             time.localtime(time.time()))
        
        util.debug(__name__, "watch-rule matched!", "=>", "GREEN")

        self.agent.sendAlert  (type         = 'monitor', 
                               date         = date, 
                               sensor       = sensor, 
                               interface    = interface,
                               plugin_id    = self.data["plugin_id"], 
                               plugin_sid   = self.data["plugin_sid"],
                               priority     = '', 
                               protocol     = 'tcp', 
                               src_ip       = self.data["from"],
                               src_port     = self.data["port_from"], 
                               dst_ip       = self.data["to"], 
                               dst_port     = self.data["port_to"],
                               condition    = self.data["condition"],
                               value        = self.data["value"])

