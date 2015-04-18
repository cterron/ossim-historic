import sys
import threading

import util

class Parser(threading.Thread):

    def __init__(self, agent, plugin):
        self.agent  = agent
        self.plugin = plugin

        self.conn      = agent.conn
        self.reconnect = agent.reconnect

        threading.Thread.__init__(self)


    # must be overriden in child class
    def process(self):
        pass

    def run(self):
        self.process()

