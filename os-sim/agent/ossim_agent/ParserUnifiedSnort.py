#
# This file monitors 
#
import os,sys
from Detector import Detector
from Logger import Logger
from ParserSnort import ParserSnort
from Event import Snort
logger = Logger.logger

class ParserUnifiedSnort(Detector):
        """This class read the events from a directory, following all the events"""
        def __init__(self, conf, plugin, conn):
                self._conf = conf # Main agente config file
                self._plugin = plugin # Plugin config file
                self.conn = conn
                self._prefix ="" 
                Detector.__init__(self, conf, plugin, self.conn)
        def process(self):
                self._dir = self._plugin.get("config","directory")
                self._linklayer = self._plugin.get("config","linklayer")
                if self._linklayer in ['ethernet','cookedlinux']:
                        if os.path.isdir(self._dir):
                                self._prefix =  self._plugin.get("config","prefix")
                                if self._prefix <>"":
                                        snort = ParserSnort(linklayer=self._linklayer)
                                        snort.init_log_dir(self._dir,self._prefix)
                                        while 1:
                                                ev = snort.get_snort_event()
                                                event = Snort()
                                                event["event_type"] = Snort.EVENT_TYPE
                                                if event['interface'] is None:
                                                        event["interface"] = self._plugin.get("config","interface")
                                                (event["unziplen"],event["gzipdata"]) = ev.strgzip()
                                                if event['plugin_id'] is None:
                                                        event['plugin_id'] = self._plugin.get("config", "plugin_id")
                                                if event['type'] is None:
                                                        event['type'] = self._plugin.get("config", "type")
                                                self.send_message(event)
                                
                                
                        else:
                                logger.error("Bad config parameter: directory (%s)" % dir)
                                sys.exit(-1)
                else:
                        logger.error("Unknown link layer")
                        sys.exit(-1)
                        
                        
# vim:ts=4 sts=4 tw=79 expandtab:

