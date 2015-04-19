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
	def __init__(self, conf, plugin):
		self._conf = conf # Main agente config file
		self._plugin = plugin # Plugin config file
		self._prefix ="" 
		Detector.__init__(self, conf, plugin)
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
						(event["unziplen"],event["gzipdata"]) = ev.strgzip()
						#event["event_type"]=Snort.EVENT_TYPE
						self.send_message(event)
				
				
			else:
				logger.error("Bad config parameter: directory (%s)" % dir)
				sys.exit(-1)
		else:
			logger.error("Unknown link layer")
			sys.exit(-1)
			
			

