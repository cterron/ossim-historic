from Monitor import Monitor

from Logger import Logger
logger = Logger.logger

import httplib

class MonitorHTTP(Monitor):

    # connect to monitor
    def open(self):

        self.conn = None
        location = self.plugin.get("config", "location")
        self.conn = httplib.HTTPConnection(location)
        return self.conn


    # get data from monitor
    def get_data(self, rule_name):

        if self.conn is None:
            return None

        data = ''
        query = self.queries[rule_name]

        logger.debug("Sending query to monitor: %s" % (query))
        self.conn.request("GET", query)
        response = self.conn.getresponse()
        if response.status == 200:
            data = response.read()
            logger.debug("Received data from monitor: %s" % (data))
        else:
            logger.warning("Error receiving from monitor: %s" % (response.reason))

        return data


    # close monitor connection
    def close(self):
        self.conn.close()

