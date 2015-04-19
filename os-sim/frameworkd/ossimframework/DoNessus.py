import os, sys, time, re, datetime, stat, tarfile, threading, pwd

from sets import Set
from optparse import OptionParser
from stat import *

import Framework
from OssimConf import OssimConf
from OssimDB import OssimDB
import Const

class DoNessus (threading.Thread) :

    def __init__ (self) :
        self.__conf = None      # ossim configuration values (ossim.conf)
        self.__conn = None      # cursor to ossim database
        self.__nessus_user = None
        self.__nessus_pass = None
        self.__nessus_host = None
        self.__nessus_port = None
        self.__nessusrc = None
        self.__dirnames = {}
        self.__filenames = {}
        self.__linknames = {}
        self.__set_debug = True
        self.__active_sensors = Set()
        self.__status = 0
        self.__last_error = None
        threading.Thread.__init__(self)

    def __startup (self) :

        # configuration values
        self.__conf = OssimConf (Const.CONFIG_FILE)

        # database connection
        self.__conn = OssimDB()
        self.__conn.connect ( self.__conf["ossim_host"],
                              self.__conf["ossim_base"],
                              self.__conf["ossim_user"],
                              self.__conf["ossim_pass"])

    def __cleanup (self) :
        self.__conn.close()

#    def __parse_options(self):
#
#        usage = "%prog [-c]"
#        parser = OptionParser(usage = usage)
#        parser.add_option("-c", "--check", dest="check", action="store_true",
#                          help="Check list of running sensors", metavar="FILE")
#        (options, args) = parser.parse_args()
#
#        return options

    def __check_sensors(self) :
        sensors = []
        active_sensors = Set()
        query = "SELECT * FROM sensor"
        hash = self.__conn.exec_query(query)

        for row in hash:
            sensors.append(row["ip"])

        # Only add those actually needed for scanning
        for sensor in sensors:
            query = "SELECT net.name,ips,sensor.ip AS sensor_ip FROM net,net_scan,net_sensor_reference,sensor WHERE net.name = net_scan.net_name AND net_scan.plugin_id = 3001 AND net_sensor_reference.net_name = net_scan.net_name AND sensor.name = net_sensor_reference.sensor_name AND sensor.ip = '%s'" % sensor
            hash = self.__conn.exec_query(query)
            if hash != []:
                active_sensors.add(sensor)

            query = "SELECT sensor.ip, inet_ntoa(host_scan.host_ip) AS temporal FROM host_scan,host,sensor,host_sensor_reference WHERE plugin_id = 3001 AND host_sensor_reference.sensor_name = sensor.name AND host_sensor_reference.host_ip = inet_ntoa(host_scan.host_ip) AND host.ip = inet_ntoa(host_scan.host_ip) AND sensor.ip = '%s' " % sensor
            hash = self.__conn.exec_query(query)
            if hash != []:
                active_sensors.add(sensor)

        pids = Set()

        for sensor in active_sensors:
            pid = os.fork()
            if pid:
                pids.add(pid)
            else:
                if self.__is_active(sensor):
                    print "Sensor %s is alive" % sensor
                os._exit(0)
        for pid in pids:
            os.waitpid(pid, 0)

        print "All checks done, leaving"
        sys.exit()

    def __rm_rf (self, what) :
        """ Recursively delete a directory """

        # Never delete /, /bin, /etc, etc...
        if len(what) < 5:
            return

        self.__debug("Deleting " + what)

        if os.path.isdir(what) == True:
            os.chdir(what)
            for dirpath,dirnames,dirfiles in os.walk(what, topdown=False):
                for file in dirfiles:
                    if file == []: pass
                    os.remove(os.path.join(dirpath,file))
                for dir in dirnames:
                    if dir == []: pass
                    os.rmdir(os.path.join(dirpath,dir))
            os.chdir("..")
            os.rmdir(what)

    def delete(self, delete_date) :
# Short sanity check.
        if len(delete_date) < 14:
            return False
        self.__startup()
        delete_dir = os.path.normpath(self.__conf["nessus_rpt_path"])
        deleted_dir = os.path.join(delete_dir, delete_date)
        self.__rm_rf(deleted_dir)
        query = "DELETE FROM host_vulnerability WHERE scan_date = '%s'" % delete_date
        self.__conn.exec_query(query)
        query = "DELETE FROM net_vulnerability WHERE scan_date = '%s'" % delete_date
        self.__conn.exec_query(query)
        return True

    def restore(self, restore_date) :
        self.__startup()
        restore_dir = os.path.normpath(self.__conf["nessus_rpt_path"])
        os.chdir(restore_dir)
        restore_file = os.path.join(restore_dir, "backup_" + restore_date + ".tar.gz")
        self.__debug("Restore file: " + restore_file)
        if os.path.exists(restore_file):
            self.__debug("Attempting to restore " + restore_file)
            tar = tarfile.open(restore_file, "r:gz")
            for tarinfo in tar:
                tar.extract(tarinfo)
            tar.close()
            os.remove(restore_file)
            return True
        return False
 
    def archive(self, archive_date) :
        """ Create a tar / gzip copy of a directory, delete it afterwards """
        self.__startup()
        archive_dir = os.path.normpath(self.__conf["nessus_rpt_path"])
        os.chdir(archive_dir)
        archived_dir = os.path.join(archive_dir, archive_date)
        archived_file = os.path.join(archive_dir, "backup_" + archive_date + ".tar.gz")
        # File exists already, don't overwrite
        if os.path.exists(archived_file) == True :
            return False

        tar = tarfile.open(archived_file, "w:gz")

        try:
            #tar.add(archived_dir)
            tar.add(archive_date)
        except OSError:
            return False
        tar.close()
        self.__rm_rf(archived_dir)
        return True

    def __isIpInNet (self, ip, networks) :
        pattern_ip = '(\d+)\.(\d+)\.(\d+)\.(\d+)'
        pattern_net = '(\d+)\.(\d+)\.(\d+)\.(\d+)/(\d+)'
        for network in networks:
            result_ip = re.findall(str(pattern_ip),ip)
            result_net = re.findall(str(pattern_net),network)
            for result_network in result_net:  
                try:
                    (ip_oct1, ip_oct2, ip_oct3, ip_oct4) = result_ip[0]
                    (net_oct1, net_oct2, net_oct3, net_oct4, mask) = result_network
                except IndexError:
                    return 0
                ip_val = int(ip_oct1)*256*256*256 + int(ip_oct2)*256*256 + int(ip_oct3)*256 + int(ip_oct4)
                net_val = int(net_oct1)*256*256*256 + int(net_oct2)*256*256 + int(net_oct3)*256 + int(net_oct4)
                if (ip_val >> (32 - int(mask))) == (net_val >> (32 - int(mask))):
                    return 1
        return 0

    def __get_networks (self, ip) :
        query = "SELECT * FROM net"
        host_networks = []
        hash = self.__conn.exec_query(query)
        for row in hash:
            temp = []
            temp.append(row["ips"])
            if self.__isIpInNet(ip, temp):
                host_networks.append(row["name"])
        return host_networks

    def __test_write_fatal (self, path) :
        if os.access(path, os.W_OK) == False :
            self.__last_error = "Nessus scan failed. Write permission needed on %s for user %d or group %d"  % (path, os.geteuid(), os.getegid())
            print "Nessus scan failed. Write permission needed on %s for user %d or group %d"  % (path, os.geteuid(), os.getegid())
            self.__cleanup()
            return

    def __test_write (self, path) :
        if os.access(path, os.W_OK) == False :
            return False
        return True

    def __debug (self, message) :
        if self.__set_debug == True :
            print message

    def __is_active (self, sensor) :
        cmd = "%s -c %s -x -s -q %s %s %s %s" % (Const.NESSUS_BIN, self.__nessusrc, sensor, self.__nessus_port, self.__nessus_user, self.__nessus_pass)
        pattern = "Session ID(.*)Targets"
        output = os.popen(cmd)
        for line in output.readlines():
            result = re.findall(pattern, line)
            if result != []:
                output.close()
                return True
        output.close()
        return False

    def __append (self, source, dest) :
        tempfd1 = open(source,"r")
        tempfd2 = open(dest,"a")
        for line in tempfd1:
            tempfd2.write(line)
        tempfd1.close()
        tempfd2.close()

    def __update_cross_correlation (self, nsr_file) :
        """ This function updates the host_plugin_sid cross-correlation table
        in order to do snort or whatever <-> nessus correlation. """

        try:
            tempfd = open(nsr_file)
        except Exception, e:
            print "Unable to open file %s: %s" % (nsr_file, e)
            return

        refs = {}

        pattern = re.compile("^([^\|]*)\|[^\|]*\|([^\|]*)\|.*")
        for line in tempfd.readlines():
            result = pattern.search(line)
        if result is not None:
            try:
                (first, second) = result.groups()
                if not refs.has_key(first):
                    refs[first] = {"sids": Set() }
                refs[first]["sids"].add(second)
            except Exception, e:
                print "%s" % e

        tempfd.close()

        self.__debug("Updating...")
        for ip in refs.iterkeys():
            self.__debug("Deleting %s" % ip)
            query = "DELETE FROM host_plugin_sid WHERE host_ip = inet_aton('%s') and plugin_id = 3001" % ip
            self.__conn.exec_query(query)
            for plugin_sid in refs[ip]["sids"]:
                self.__debug("Inserting %d" % int(plugin_sid))
                query = "INSERT INTO host_plugin_sid(host_ip, plugin_id, plugin_sid) values(inet_aton('%s'), 3001, %d)" % (ip, int(plugin_sid))
                self.__conn.exec_query(query)

    def __update_vulnerability_tables(self, nsr_txt_file, today):
        """ This function updates the host_vulnerability & net_vulnerability
        tables used within the vulnmeter """

        risk_values = {
            "None": 0,
            "Verylow/none": 1,
            "Low": 2,
            "Low/Medium": 3,
            "Medium/Low": 4,
            "Medium": 5,
            "Medium/High": 6,
            "High/Medium": 7,
            "High": 8,
            "Veryhigh": 9,
            "Critical": 10
        }

        hosts = Set()
        hv = {}
        try:
            vulnsfd = open(nsr_txt_file, "r")
        except Exception, e:
            print "Unable to open file %s: %s" % (nsr_txt_file, e)
            return

        for line in vulnsfd:
            pattern1 = "^(\d+\.\d+\.\d+\.\d+)"
            pattern2 = "Risk [Ff]actor\s*:\W+(\w*)"
            result1 = re.findall(str(pattern1),line)
            result2 = re.findall(str(pattern2),line)
            try:
                (host) = result1[0]
                if not hv.has_key(host):
                    hv[host] = 0
            except IndexError:
                continue 
            try:
                (risk) = result2[0]
            except IndexError:
                # continue
		risk = "None"
            if risk == "":
                # continue
		risk = "None"
            
            hosts.add(host)
            risk = re.sub(" \/.*|if.*","", risk)
            risk = re.sub(" ","", risk)
            rv = 0
            """
            Need to modify this in order to catch weird nessus plugin things
            like:
            - Low to High
            - Low (if you are not using Kerberos) / High (if kerberos is enabled)
            - Low (remotely) / High (locally)
            - etc...
            """
            if risk_values.has_key(risk):
                rv = risk_values[risk]

            hv[host] += rv


        vulnsfd.close()

        net_vuln_lvl = {}

        self.__debug("\nUpdating host vulnerability levels")

        if hosts is not None:
            for host in hosts:
                vulnerability = hv[host]

                query = "INSERT INTO host_vulnerability VALUES ('%s', '%s', '%s')" % (host, today , vulnerability)
                self.__conn.exec_query(query)


                vuln_networks = self.__get_networks(host)
                if vuln_networks is not None:
                    for net in vuln_networks:
                        try: net_vuln_lvl[net]
                        except: 
                            net_vuln_lvl[net] = 0
                        self.__debug("Increasing %s by %d due to %s" % (net, vulnerability, host))
                        net_vuln_lvl[net] += vulnerability

        self.__debug("\nUpdating net vulnerability levels")

        if net_vuln_lvl is not None:
            for net in net_vuln_lvl:
                query = "INSERT INTO net_vulnerability(net, scan_date, vulnerability) VALUES('%s', '%s', %d)" % (net, today, int(net_vuln_lvl[net]))
                self.__conn.exec_query(query)

    def __backup_vulnerability_tables(self, today):
        """ This function backups host_vulnerability and net_vulnerability into
        a text file """

        self.__debug("\nBacking up vulnerability levels (into files)")

        uid = pwd.getpwnam("mysql")[2]

        os.chown(today, uid, -1)

        result_vuln_sql_host = os.path.join(today, "vuln_host.sql.txt")
        result_vuln_sql_net = os.path.join(today, "vuln_net.sql.txt")

        query = "SELECT * FROM host_vulnerability INTO OUTFILE '%s'" % result_vuln_sql_host
        try:
            self.__conn.exec_query(query)
        except Exception, e:
            print "Error executing sql backup query:", e

        query = "SELECT * FROM net_vulnerability INTO OUTFILE '%s'" % result_vuln_sql_net
        try:
            self.__conn.exec_query(query)
        except Exception, e:
            print "Error executing sql backup query:", e

        # No need for the webserver to access these files directly
        try:
            os.chmod(result_vuln_sql_host,0600)
            os.chmod(result_vuln_sql_net,0600)
        except OSError, e:
            print e

    def status (self) :
        return self.__status

    def reset_status (self) :
        self.__status = 0
        self.__last_error = None

    def get_error (self) :
        return self.__last_error

    def run (self) :
        self.__startup()
        self.__status = 1

        if self.__conf["nessus_path"]:
            Const.NESSUS_BIN = self.__conf["nessus_path"]
        if self.__conf["nessus_distributed"] == 1:
            nessus_distributed = True
        else:
            nessus_distributed = False
        self.__nessus_user = self.__conf["nessus_user"]
        self.__nessus_pass = self.__conf["nessus_pass"]
        self.__nessus_host = self.__conf["nessus_host"]
        self.__nessus_port = self.__conf["nessus_port"]

        today_date = datetime.datetime.today().strftime("%Y%m%d%H%M00")

        self.__dirnames["nessus_rpt_path"] = os.path.normpath(self.__conf["nessus_rpt_path"]) + "/"
        self.__dirnames["nessus_tmp"] = os.path.join(self.__dirnames["nessus_rpt_path"], "tmp") + "/"
        self.__dirnames["sensors"] = os.path.join(self.__dirnames["nessus_tmp"], "sensors") + "/"
        self.__dirnames["today"] = os.path.join(self.__dirnames["nessus_rpt_path"], today_date) + "/"
        self.__filenames["targets"] = os.path.join(self.__dirnames["nessus_tmp"], "targets.txt")
        self.__filenames["result_nsr_txt"] = os.path.join(self.__dirnames["nessus_tmp"], "result.txt")
        self.__filenames["result_nsr"] = os.path.join(self.__dirnames["nessus_tmp"], "result.nsr")
        self.__linknames["last"] = os.path.join(self.__dirnames["nessus_rpt_path"],"last")
        self.__filenames["today_nsr"] = os.path.join(self.__dirnames["nessus_tmp"],"temp_res." + today_date + ".nsr")


        self.__test_write_fatal(self.__dirnames["nessus_rpt_path"])

        if self.__test_write(self.__dirnames["today"]) == False :
            self.__debug("Creating todays scan dir: %s" % self.__dirnames["today"])
            try :
                os.mkdir(self.__dirnames["today"], 0755)
            except OSError, e :
                print e


        if self.__test_write(self.__dirnames["nessus_tmp"]) == False :
            print "Creating temp dir: %s" % self.__dirnames["nessus_tmp"]
            try :
                os.mkdir(self.__dirnames["nessus_tmp"], 0755)
            except OSError, e :
                print e

        try :
            os.unlink(self.__filenames["result_nsr"])
        except OSError, e :
            pass

        # No need to generate fake nessusrc, nessus takes care of that

        self.__nessusrc = os.path.join(self.__dirnames["nessus_tmp"], ".nessusrc")
        if self.__conf["nessusrc_path"]:
            self.__nessusrc = self.__conf["nessusrc_path"]

        # Are we only checking the sensors ? 
#        options = self.__parse_options()
#        if options.check:
#            self.__check_sensors() 

        self.__status = 10

        if nessus_distributed == True :
            self.__debug("Entering distributed mode")
            if self.__test_write(self.__dirnames["sensors"]) == False :
                self.__debug("Creating sensor temp dir: %s" % self.__dirnames["sensors"])
                try :
                    os.mkdir(self.__dirnames["sensors"], 0755)
                except OSError, e :
                    print e
            scan_networks = [] 
            sensors = []
            active_sensors = []
            query = "SELECT * FROM sensor"
            hash = self.__conn.exec_query(query)
            for row in hash:
                sensors.append(row["ip"])

            self.__filenames["targetfile"] = {}
            self.__filenames["nsrfile"] = {}

            self.__status = 15
            for sensor in sensors:
                self.__filenames["targetfile"][sensor] = os.path.join(self.__dirnames["sensors"], sensor + ".targets.txt")
                sensorfd = open(self.__filenames["targetfile"][sensor],"w")

                query = "SELECT net.name,ips,sensor.ip AS sensor_ip FROM net,net_scan,net_sensor_reference,sensor WHERE net.name = net_scan.net_name AND net_scan.plugin_id = 3001 AND net_sensor_reference.net_name = net_scan.net_name AND sensor.name = net_sensor_reference.sensor_name AND sensor.ip = '%s'" % sensor
                hash = self.__conn.exec_query(query)
                for row in hash:
                    self.__debug("Adding net %s" % row["ips"])
                    sensorfd.writelines(row["ips"] + "\n")
                    scan_networks.append(row["ips"])

                query = "SELECT sensor.ip, inet_ntoa(host_scan.host_ip) AS temporal FROM host_scan,host,sensor,host_sensor_reference WHERE plugin_id = 3001 AND host_sensor_reference.sensor_name = sensor.name AND host_sensor_reference.host_ip = inet_ntoa(host_scan.host_ip) AND host.ip = inet_ntoa(host_scan.host_ip) AND sensor.ip = '%s' " % sensor
                hash = self.__conn.exec_query(query)
                for row in hash:
                    if(self.__isIpInNet(row["temporal"], scan_networks)):
                        self.__debug("DUP host, already defined within network: %s" % row["temporal"])
                    else :
                        try:
                            sensorfd.writelines(row["temporal"] + "\n")
                        except KeyError, e:
                            pass
                        self.__debug("Adding host %s" % row["temporal"])
                sensorfd.close()

            pids = Set()
            self.__status = 20
            for sensor in sensors:
                pid = os.fork()
                if pid:
                    pids.add(pid)
                else:
                    if os.stat(self.__filenames["targetfile"][sensor])[stat.ST_SIZE] == 0:
                        try :
                            os.unlink(self.__filenames["targetfile"][sensor])
                        except OSError, e :
                            pass
                        self.__debug("Child %s exiting" % sensor)
                        os._exit(0)

                    if self.__is_active(sensor) == False:
                        try :
                            os.unlink(self.__filenames["targetfile"][sensor])
                        except OSError, e :
                            pass
                        self.__debug("Child %s exiting" % sensor)
                        os._exit(0)

                    self.__filenames["nsrfile"][sensor] = os.path.join(self.__dirnames["sensors"], sensor + ".temp_res.nsr")
                    targetfd = open(self.__filenames["targetfile"][sensor], "r")
                    num_hosts = 0
                    pattern = re.compile("(.*)\/(.*)")
                    for line in targetfd:
                        result = pattern.match(line)
                        if result is not None:
                            (network,mask) = result.groups()
                            num_hosts += int((2 << (32 - int(mask))-1) -2)
                        else:
                            num_hosts += 1
                    targetfd.close()
                    self.__debug("%s up and running, starting scan against %s hosts" % (sensor, num_hosts))

                    self.__debug("Starting scan against:\n----------------------")
                    targetfd = open(self.__filenames["targetfile"][sensor], "r")
                    for line in targetfd:
                        self.__debug(line)
                    targetfd.close()

                    cmd = "%s -c %s -x -T nsr -q %s %s %s %s %s %s" % (Const.NESSUS_BIN, self.__nessusrc, sensor, self.__nessus_port, self.__nessus_user, self.__nessus_pass, self.__filenames["targetfile"][sensor], self.__filenames["nsrfile"][sensor] )
                    # Discard output
                    os.system(cmd)


                    # Append results to main result file
                    self.__append(self.__filenames["nsrfile"][sensor], self.__filenames["result_nsr"])

                    self.__debug("Child %s exiting" % sensor)
                    os._exit(0)

            self.__debug("Waiting for scans to finish")
            for pid in pids:
                os.waitpid(pid, 0)
            self.__debug("Scan finished, cleaning up a bit")


            # Back to parent


            for sensor in active_sensors:
                try:
                    os.unlink (self.__filenames["nsrfile"][sensor])
                    os.unlink (self.__filenames["targetfile"][sensor])
                except OSError:
                    pass



        else: # non-distributed
            self.__debug("Entering non-distributed mode")

            sensorfd = open(self.__filenames["targets"], "w")
            scan_networks = []
            self.__debug("Adding networks")
            query = "SELECT name,ips FROM net, net_scan WHERE net.name = net_scan.net_name AND net_scan.plugin_id = 3001"
            hash = self.__conn.exec_query(query)
            for row in hash:
                self.__debug("Adding network %s" % row["ips"])
                sensorfd.writelines(row["ips"] + "\n")
                scan_networks.append(row["ips"])

            self.__status = 15
                  
            self.__debug("Adding hosts")
            query = "SELECT inet_ntoa(host_ip) AS temporal FROM host_scan WHERE plugin_id = 3001"
            hash = self.__conn.exec_query(query)
            for row in hash:
                try:
                    if self.__isIpInNet(row["temporal"],scan_networks) == True:
                        print "Dup: %s. Please check your config" % row["temporal"]
                    else:
                        self.__debug("Adding host %s" % row["temporal"])
                        sensorfd.writelines(row["temporal"] + "\n")
                except KeyError:
                    pass
            if scan_networks != []:
                print scan_networks

            self.__status = 20

            sensorfd.close()
            self.__debug("Going to scan:")
            self.__debug("--------------")
            tempfd = open(self.__filenames["targets"], "r")
            for line in tempfd.readlines():
                self.__debug(line)
            tempfd.close()
            cmd = "%s -c %s -x -T nsr -q %s %s %s %s %s %s" % (Const.NESSUS_BIN, self.__nessusrc, self.__nessus_host, self.__nessus_port, self.__nessus_user, self.__nessus_pass, self.__filenames["targets"], self.__filenames["result_nsr"])
            os.system(cmd)

        # Start Converting & calculating

        self.__status = 50

        # Convert to txt so we can match vulnerabilities
        if os.path.exists(self.__filenames["result_nsr"]) == True :
            cmd = "%s -c %s -T text -i %s -o %s" % (Const.NESSUS_BIN, self.__nessusrc, self.__filenames["result_nsr"], self.__filenames["result_nsr_txt"])
            os.system(cmd)
        else:
            self.__status = -1
            self.__last_error = "Result file " + self.__filenames["result_nsr"] + " not present after scan"
            print "Scan failed check output and try enabling debug"
            return

        if os.path.exists(self.__filenames["result_nsr"]) == True :
            self.__update_vulnerability_tables(self.__filenames["result_nsr"], today_date) 

        try:
            if os.stat(self.__filenames["today_nsr"])[stat.ST_SIZE] > 0:
                self.__debug("\nAppending results")
                self.__append(self.__filenames["today_nsr"],self.__filenames["result_nsr"])
        except OSError:
            pass

        self.__rm_rf(self.__dirnames["today"])

        self.__status = 75

        self.__debug("Today is %s" % today_date)
        
        if os.path.exists(self.__filenames["result_nsr"]) == True :
            cmd = "%s -c %s -T html_graph -i %s -o %s" % (Const.NESSUS_BIN, self.__nessusrc, self.__filenames["result_nsr"], self.__dirnames["today"])
            os.system(cmd)

        self.__status = 90

        if os.path.isdir(self.__dirnames["today"]) == True and len(self.__dirnames["today"]) > 4:
            if self.__conf["ossim_type"] == "mysql":
                self.__backup_vulnerability_tables(self.__dirnames["today"]) 
            os.chmod(self.__dirnames["today"],0755)
            os.chdir(self.__dirnames["today"])
            for dirpath,dirnames,dirfiles in os.walk(self.__dirnames["today"]):
                for dir in dirnames:
                    if dir == []: pass
                    os.chmod(dir, 0755)
      
        if os.path.exists(self.__filenames["today_nsr"]) == True :
            os.remove(self.__filenames["today_nsr"])

        if os.path.exists(self.__filenames["result_nsr"]) == True:
            os.rename(self.__filenames["result_nsr"],self.__filenames["today_nsr"])

        try:
            os.remove(self.__linknames["last"])
        except Exception, e:
            pass
        if os.path.exists(self.__dirnames["today"]) == True:
            os.symlink(self.__dirnames["today"], self.__linknames["last"])

        self.__status = 95

        if os.path.exists(self.__filenames["today_nsr"]) == True :
            self.__update_cross_correlation(self.__filenames["today_nsr"])

        self.__debug("Parent exiting")

        self.__status = 0

        return

if __name__ == "__main__":


    donessus = DoNessus()
    donessus.start()
# vim:ts=4 sts=4 tw=79 expandtab:
