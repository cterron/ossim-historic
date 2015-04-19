import threading
import rrdtool
import time
import os
import re

import Const, Util
from OssimDB import OssimDB
from OssimConf import OssimConf

class RRDUpdate(threading.Thread):

    def __init__(self):
        self.__conf = OssimConf(Const.CONFIG_FILE)
        self.__db = OssimDB()
        threading.Thread.__init__(self)


    def __get_hosts(self):

        query = "SELECT * FROM host_qualification"
        return self.__db.exec_query(query)


    def __get_nets(self):

        query = "SELECT * FROM net_qualification"
        return self.__db.exec_query(query)


    def __get_users(self):
        
        query = "SELECT * FROM users"
        return self.__db.exec_query(query)

    def __get_incident_users(self):
        
        query = "SELECT in_charge FROM incident_ticket GROUP BY in_charge;"
        return self.__db.exec_query(query)


    
    def get_global_qualification(self, allowed_nets):
        
        compromise = attack = 1

        for host in self.__get_hosts():
            if Util.isIpInNet(host["host_ip"], allowed_nets) or \
               not allowed_nets:
                compromise += int(host["compromise"])
                attack += int(host["attack"])

        if compromise < 1: compromise = 1
        if attack < 1: attack = 1

        return (compromise, attack)

    
    def get_level_qualification(self, user):
        
        compromise = attack = count = 0

        if self.__conf["rrdtool_path"]:
           Const.RRD_BIN = os.path.join(self.__conf["rrdtool_path"], "rrdtool")

        f = os.popen("%s fetch %s AVERAGE -s N-1D -e N" % \
            (Const.RRD_BIN, os.path.join(self.__conf["rrdpath_global"], 
                                         "global_" + user + ".rrd")))
        for line in f.readlines():
            result = re.findall("(\d+):\s+(\S+)\s+(\S+)", line)
            if result != []:
                (date, c, a) = tuple(result[0])
                if c != "nan" and a != "nan":
                    if float(c) <= float(self.__conf["threshold"]):
                        compromise += 1
                    if float(a) <= float(self.__conf["threshold"]):
                        attack += 1
                    count += 1

        f.close()

        if count != 0:
            if compromise != 0:
                compromise = (compromise * 100) / count
            if attack != 0:
                attack = (attack * 100) / count

        return (compromise, attack)

    def get_incidents (self, user):
       
        status = {}
        query = "SELECT count(*) as count, status FROM incident_ticket WHERE in_charge = \"%s\" GROUP BY status" % user
        hash = self.__db.exec_query(query)
        for row in hash: # Should be only one anyway
            status[row["status"]] = row["count"] 
        return status


    def update(self, rrdfile, compromise, attack):

        timestamp = int(time.time())

        try:
            open(rrdfile)
        except IOError:
            print __name__, ": Creating %s.." % (rrdfile)
# This needs some checking, we don't need HWPREDICT here and I got some probs
# on MacosX (def update_simple) so I removed aberrant behaviour detection.
            rrdtool.create(rrdfile,
                           '-b', str(timestamp), '-s300',
                           'DS:ds0:GAUGE:600:0:1000000',
                           'DS:ds1:GAUGE:600:0:1000000',
                           'RRA:AVERAGE:0.5:1:800',
                           'RRA:HWPREDICT:1440:0.1:0.0035:288',
                           'RRA:AVERAGE:0.5:6:800',
                           'RRA:AVERAGE:0.5:24:800',
                           'RRA:AVERAGE:0.5:288:800',
                           'RRA:MAX:0.5:1:800',
                           'RRA:MAX:0.5:6:800',
                           'RRA:MAX:0.5:24:800',
                           'RRA:MAX:0.5:288:800')
        else:
            print __name__, ": Updating %s with values (C=%s, A=%s).." \
                % (rrdfile, compromise, attack)

            # RRDs::update("$dotrrd", "$time:$inlast:$outlast");
	    # It may fail here, I don't know if it only happens on MacosX but this
        # does solve it. (DK 2006/02)
        try:
            rrdtool.update(rrdfile, str(timestamp) + ":" + \
                            str(compromise) + ":" +\
                            str(attack))
        except Exception, e:
            print "Error updating %s: %s" % (rrdfile, e) 

    def update_simple(self, rrdfile, count):

        timestamp = int(time.time())

        try:
            open(rrdfile)
        except IOError:
            print __name__, ": Creating %s.." % (rrdfile)
            rrdtool.create(rrdfile,
                           '-b', str(timestamp), '-s300',
                           'DS:ds0:GAUGE:600:0:1000000',
                           'RRA:AVERAGE:0.5:1:800',
                           'RRA:AVERAGE:0.5:6:800',
                           'RRA:AVERAGE:0.5:24:800',
                           'RRA:AVERAGE:0.5:288:800',
                           'RRA:MAX:0.5:1:800',
                           'RRA:MAX:0.5:6:800',
                           'RRA:MAX:0.5:24:800',
                           'RRA:MAX:0.5:288:800')
        else:
            print __name__, ": Updating %s with value (Count=%s).." \
                % (rrdfile, count)
            try:
                rrdtool.update(rrdfile, str(timestamp) + ":" + \
                            str(count))
            except Exception, e:
                print "Error updating %s: %s" % (rrdfile, e) 



    def run(self):

        self.__db.connect(self.__conf['ossim_host'],
                          self.__conf['ossim_base'],
                          self.__conf['ossim_user'],
                          self.__conf['ossim_pass'])

        while 1:
            
            ### incidents
            try:
                rrdpath = self.__conf["rrdpath_incidents"]
                if not os.path.isdir(rrdpath):
                    os.mkdir(rrdpath, 0755)
                for user in self.__get_incident_users():
                    incidents = self.get_incidents(user["in_charge"])
                    for type in incidents:
                        filename = os.path.join(rrdpath, "incidents_" + user["in_charge"] + "_" +  type + ".rrd")
                        self.update_simple(filename, incidents[type])
            except OSError, e:
                print __name__, e

            ### hosts
            try:
                rrdpath = self.__conf["rrdpath_host"]
                if not os.path.isdir(rrdpath):
                    os.mkdir(rrdpath, 0755)
                for host in self.__get_hosts():
                    filename = os.path.join(rrdpath, host["host_ip"] + ".rrd")
                    self.update(filename, host["compromise"], host["attack"])
            except OSError, e:
                print __name__, e
            
            
            ### nets
            try:
                rrdpath = self.__conf["rrdpath_net"]
                if not os.path.isdir(rrdpath):
                    os.mkdir(rrdpath, 0755)
                for net in self.__get_nets():
                    filename = os.path.join(rrdpath, net["net_name"] + ".rrd")
                    self.update(filename, net["compromise"], net["attack"])
            except OSError, e:
                print __name__, e

            ### global
            try:
                rrdpath = self.__conf["rrdpath_global"]
                if not os.path.isdir(rrdpath):
                    os.mkdir(rrdpath, 0755)
                for user in self.__get_users():

                    # ** FIXME **
                    # allow all nets if user is admin
                    # it's ugly, I know..
                    if user['login'] == 'admin':
                        user['allowed_nets'] = ''

                    filename = os.path.join(rrdpath,
                                            "global_" + user["login"] + ".rrd")
                    (compromise, attack) = \
                        self.get_global_qualification(user["allowed_nets"])
                    self.update(filename, compromise, attack)
            except OSError, e:
                print __name__, e
                    
            ### level
            try:
                rrdpath = self.__conf["rrdpath_level"]
                if not os.path.isdir(rrdpath):
                    os.mkdir(rrdpath, 0755)
                for user in self.__get_users():
                    filename = os.path.join(rrdpath,
                                            "level_" + user["login"] + ".rrd")
                    (compromise, attack) = \
                        self.get_level_qualification(user["login"])
                    self.update(filename, compromise, attack)
            except OSError, e:
                print __name__, e

            time.sleep(float(Const.SLEEP))

        # never reached..
        self.__db.close()


if __name__ == "__main__":

    rrd = RRDUpdate()
    rrd.start()

# vim:ts=4 sts=4 tw=79 expandtab:
