#
# Some maintenance tasks for the Nagios config related to HOST SERVICES
#

from NagiosMisc import nagios_host,nagios_host_service,nagios_host_group_service
from OssimDB import OssimDB
from OssimConf import OssimConf
import time, threading
import Const
import os
import re

looped=0
class DoNagios(threading.Thread):
	_interval=600					# intervals

	def test_create_dir(self,path):
		if not os.path.exists(path):
			os.makedirs(path)

	def __init__(self):
		self._tmp_conf= OssimConf (Const.CONFIG_FILE)
		threading.Thread.__init__(self)
		self.test_create_dir(self._tmp_conf['nagios_cfgs'])
		self.test_create_dir(os.path.join(self._tmp_conf['nagios_cfgs'],"hosts"))
		self.test_create_dir(os.path.join(self._tmp_conf['nagios_cfgs'],"host-services"))
		self.test_create_dir(os.path.join(self._tmp_conf['nagios_cfgs'],"hostgroups"))
		self.test_create_dir(os.path.join(self._tmp_conf['nagios_cfgs'],"hostgroup-services"))

	def run(self):
		global looped
		if looped ==0:
			self.loop()
			looped=1

	def debug(self,msg):
		print __name__, " : ", msg

	def loop(self):
		while True:
			self.debug("Looking for new services to add")
			self.make_nagios_changes()

			#Sleep until the next round
			self.debug("Sleeping until the next round in %ss" % self._interval)
			time.sleep(self._interval)

	def make_nagios_changes(self):
		db=OssimDB()
		db.connect (self._tmp_conf["ossim_host"],
			    self._tmp_conf["ossim_base"],
			    self._tmp_conf["ossim_user"],
			    self._tmp_conf["ossim_pass"])
		query="select port from host_services where (protocol=6 or protocol=0) and nagios=1 group by port"
		services=db.exec_query(query)

		path = os.path.join(self._tmp_conf['nagios_cfgs'], "host-services")
		for fi in os.listdir(path):
			os.remove(os.path.join(path, fi))

		path = os.path.join(self._tmp_conf['nagios_cfgs'], "hostgroup-services")
		for fi in os.listdir(path):
			os.remove(os.path.join(path, fi))
		i=0

		for port in services:
			i+=1
			query="select h.ip, hs.service_type from host_services hs, host_scan h_sc, host h where (hs.protocol=6 or hs.protocol=0) and hs.port=%s and hs.ip=h_sc.host_ip and h_sc.plugin_id=2007 and hs.nagios=1 and h.ip=inet_ntoa(hs.ip) group by h.ip order by h.ip" % port['port']
			hosts=db.exec_query(query)
			list=""
			for host in hosts:
				if list!="":
					list+=","
				list+=host['ip']

			if list!="":
				k=nagios_host_service(list, port['port'], host['service_type'], "check_tcp!%d" % port['port'], "0", self._tmp_conf)
				k.select_command()
				k.write()

				hg=nagios_host_group_service(self.serv_port(port['port']),self.serv_name(port['port']),list,self._tmp_conf)
				hg.write()

		if port in services:
			self.debug("Changes where applied! Reloading Nagios config.")
			self.reload_nagios()

		db.close()

	def reload_nagios(self):
		os.system(self._tmp_conf['nagios_reload_cmd'])


	def port_to_service(self,number):
		f = open("/etc/services")
		#Actually we only look for tcp protocols here
		regexp_line=r'^(?P<serv_name>[^\s]+)\s+%d/tcp.*' % number
		try:
			service=re.compile(regexp_line)
			for line in f:

				serv=service.match(line)
				if serv != None:
					return serv.groups()[0]
		finally:
			f.close()

	def serv_name(self,port):
		return "%s_Servers" % (self.port_to_service(port)) 

	def serv_port(self,port):
		return "port_%d_Servers" % port





