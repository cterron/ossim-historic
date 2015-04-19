"""
Wrapper arround list object with mutual exclusion in append/remove methods
"""

from Logger import Logger
logger = Logger.logger

import mutex

class MonitorList(list):

    MAX_SIZE = 5000

    def __init__(self):
        self.mutex = mutex.mutex()
        list.__init__(self)

    def appendRule(self, item):
        "append with mutual exclusion"

        logger.debug(
            "Appending monitor object, list has %d elements" % (len(self)+1))
        self.mutex.lock(self.append, item)
        self.mutex.unlock()

    def removeRule(self, item):
        "remove with mutual exclusion"

        logger.debug(
            "Removing monitor object, list has %d elements" % (len(self)-1))
        self.mutex.lock(self.remove, item)
        self.mutex.unlock()
        if hasattr(item, 'close'):
            item.close()
        del item


if __name__ == "__main__":

    m = MonitorList()
    m.appendRule("a")
    print m
    m.appendRule("b")
    print m
    m.removeRule(m[0])
    print m

