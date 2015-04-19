import threading, time

from Logger import Logger
logger = Logger.logger

from MonitorList import MonitorList
from MonitorSocket import MonitorSocket
from MonitorCommand import MonitorCommand
from MonitorDatabase import MonitorDatabase
from MonitorHTTP import MonitorHTTP

class MonitorScheduler(threading.Thread):

    def __init__(self):
        self.monitor_list = MonitorList()
        threading.Thread.__init__(self)

    def new_monitor(self, type, plugin, watch_rule):
        if type in ('socket', 'unix_socket'):
            monitor = MonitorSocket(plugin, watch_rule)
            self.monitor_list.appendRule(monitor)
        elif type == 'database':
            monitor = MonitorDatabase(plugin, watch_rule)
            self.monitor_list.appendRule(monitor)
        elif type in ('log', 'file'):
            monitor = MonitorFile(plugin, watch_rule)
            self.monitor_list.appendRule(monitor)
        elif type == ('command'):
            monitor = MonitorCommand(plugin, watch_rule)
            self.monitor_list.appendRule(monitor)
        elif type == ('http'):
            monitor = MonitorHTTP(plugin, watch_rule)
            self.monitor_list.appendRule(monitor)
         
    def run(self):
        logger.debug("Monitor Scheduler started")
        while 1:
            for monitor in self.monitor_list:
                # get monitor from monitor list
                # process watch-rule and remove from list
                if monitor.process():
                    self.monitor_list.removeRule(monitor)

            # don't overload agent
            time.sleep(2)


# vim:ts=4 sts=4 tw=79 expandtab:


