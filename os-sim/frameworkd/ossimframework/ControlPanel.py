import os, sys, time, re

from OssimConf import OssimConf
from OssimDB import OssimDB
import threading
import Const


class ControlPanel (threading.Thread) :

    def __init__ (self) :
        self.__conf = None      # ossim configuration values (ossim.conf)
        self.__conn = None      # cursor to ossim database
        self.__rrd_path = {}    # path for global, net, host and level rrds
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

        # rrd paths
        if self.__conf["rrdtool_path"]:
            Const.RRD_BIN = os.path.join(self.__conf["rrdtool_path"], "rrdtool")

        try:
            for dest in [ "global", "net", "host", "level" ] :
                self.__rrd_path[dest] = \
                    os.path.join(self.__conf["mrtg_rrd_files_path"], 
                        '%s_qualification' % (dest))
        except OSError, e:
            print >>sys.stderr, "Error reading RRD path: " + e
            sys.exit()
 

    def __cleanup (self) :
        self.__conn.close()


    # update the rrd database entry
    def __update_db(self, type, info, rrd_name, range):

        query = """
            DELETE FROM control_panel 
                WHERE id = '%s' AND rrd_type = '%s' AND time_range = '%s'
            """ % (rrd_name, type, range)

        self.__conn.exec_query(query)

        if not (type == 'host' and info["max_c"] == 0 and info["max_a"] == 0):

            query = """
                INSERT INTO control_panel 
                (id, rrd_type, time_range, max_c, max_a, max_c_date, max_a_date)
                VALUES ('%s', '%s', '%s', %f, %f, '%s', '%s')
                """ % (rrd_name, type, range, info["max_c"], info["max_a"],
                       info["max_c_date"], info["max_a_date"])

            self.__conn.exec_query(query)

            print "updating %s (%s):    \tC=%f, A=%f" % \
                (rrd_name, range, info["max_c"], info["max_a"])
            sys.stdout.flush()


    # return a tuple with the date of C and A max values
    def __get_max_date(self, start, end, rrd_file):
        
        max_c = max_a = 0.0
        max_c_date = max_a_date = c_date = a_date = ""
        
        # execute rrdtool fetch and obtain max c & a date
        cmd = "%s fetch %s MAX -s %s -e %s" % \
            (Const.RRD_BIN, rrd_file, start, end)
        output = os.popen(cmd)
        pattern = "(\d+):\s+(\S+)\s+(\S+)"
        for line in output.readlines():
            result = re.findall(pattern, line)
            if result != []:
                (date, compromise, attack) = result[0]
                if compromise not in ("nan", "") and attack not in ("nan", ""):
                    if float(compromise) >= max_c:
                        c_date = date
                        max_c = float(compromise)
                    if float(attack) >= max_a:
                        a_date = date
                        max_a = float(attack)

        output.close()

        # convert date to datetime format
        if c_date != "":
            max_c_date = time.strftime('%Y-%m-%d %H:%M:%S',
                time.localtime(float(c_date)))
        if a_date != "":
            max_a_date = time.strftime('%Y-%m-%d %H:%M:%S',
                time.localtime(float(a_date)))
        
        return (max_c_date, max_a_date)


    # get a rrd C & A value
    def __get_rrd_value(self, start, end, rrd_file):
       
        rrd_info = {}

        # C max 
        # (2nd line of rrdtool graph ds0)
        cmd = "%s graph /dev/null -s %s -e %s -X 2 DEF:obs=%s:ds0:AVERAGE PRINT:obs:MAX:%%lf" % (Const.RRD_BIN, start, end, rrd_file)
        output = os.popen(cmd)
        output.readline()
        c_max = output.readline()
        output.close()
       
        # ignore 'nan' values
        if c_max not in ("nan\n", ""):
            rrd_info["max_c"] = float(c_max)
        else:
            rrd_info["max_c"] = 0
        
        # A max 
        # (2nd line of rrdtool graph ds1)
        cmd = "%s graph /dev/null -s %s -e %s -X 2 DEF:obs=%s:ds1:AVERAGE PRINT:obs:MAX:%%lf" % (Const.RRD_BIN, start, end, rrd_file)
        output = os.popen(cmd)
        output.readline()
        a_max = output.readline()
        output.close()

        # ignore 'nan' values
        if a_max not in ("nan\n", ""):
            rrd_info["max_a"] = float(a_max)
        else:
            rrd_info["max_a"] = 0

        (rrd_info["max_c_date"], rrd_info["max_a_date"]) = \
            self.__get_max_date(start, end, rrd_file)

        return rrd_info


    def __sec_update(self):

        for rrd_file in os.listdir(self.__rrd_path["level"]):

            result = re.findall("level_(\w+)\.rrd", rrd_file)
            if result != []:

                user = result[0]

                rrd_file = os.path.join(self.__rrd_path["level"], rrd_file)
                rrd_name = os.path.basename(rrd_file.split(".rrd")[0])

                threshold = self.__conf["threshold"]


                # get last value for day level
                rrd_info = self.__get_rrd_value("N-15m", "N", rrd_file)

                query = """
            UPDATE control_panel
                SET c_sec_level = %f, a_sec_level = %f
                WHERE id = 'global_%s' and time_range = 'day'
            """ % (rrd_info["max_c"], rrd_info["max_a"], user)

                print "updating level (day):  C=%s%%, A=%s%%" % \
                    (rrd_info["max_c"], rrd_info["max_a"])

                self.__conn.exec_query(query)


                # calculate average for week, month and year levels
                for range in ["week", "month", "year"]:

                    range2date = {
                        "day"  : "N-1D",
                        "week" : "N-7D",
                        "month": "N-1M",
                        "year" : "N-1Y",
                    }

                    output = os.popen("%s fetch %s AVERAGE -s %s -e N" % \
                        (Const.RRD_BIN, rrd_file, range2date[range]))

                    pattern = "(\d+):\s+(\S+)\s+(\S+)"
                    C_level = A_level = count = 0
                    for line in output.readlines():
                        result = re.findall(pattern, line)
                        if result != []:
                            (date, compromise, attack) = result[0]
                            if compromise not in ("nan", "") \
                              and attack not in ("nan", ""):
                                C_level += float(compromise)
                                A_level += float(attack)
                                count += 1

                    output.close

                    if count == 0:
                        query = """
                            UPDATE control_panel 
                                SET c_sec_level = 0, a_sec_level = 0
                                WHERE id = 'global_%s' and time_range = '%s'
                        """ % (user, range)

                    else:
                        query = """
                            UPDATE control_panel 
                                SET c_sec_level = %f, a_sec_level = %f
                                WHERE id = 'global_%s' and time_range = '%s'
                        """ % (C_level / count, A_level / count, user, range)

                        print "updating %s (%s):  C=%s%%, A=%s%%" % \
                            (rrd_name, range, C_level / count, A_level / count)

                    self.__conn.exec_query(query)


    # update all rrds in rrd_path
    def __update(self, type):

        for rrd_file in os.listdir(self.__rrd_path[type]):
            
            if rrd_file.rfind(".rrd") != -1:
            
                rrd_file = os.path.join(self.__rrd_path[type], rrd_file)
                rrd_name = os.path.basename(rrd_file.split(".rrd")[0])
                
                rrd_info_day   = self.__get_rrd_value("N-1D", "N", rrd_file)
                rrd_info_week  = self.__get_rrd_value("N-7D", "N", rrd_file)
                rrd_info_month = self.__get_rrd_value("N-1M", "N", rrd_file)
                rrd_info_year  = self.__get_rrd_value("N-1Y", "N", rrd_file)

                if rrd_info_day["max_c"] == rrd_info_day["max_a"] == \
                    rrd_info_week["max_c"] == rrd_info_week["max_a"] == \
                    rrd_info_month["max_c"] == rrd_info_month["max_a"] == \
                    rrd_info_year["max_c"] == rrd_info_year["max_a"] == 0:

                    print "Removing unused rrd file (%s).." % (rrd_file)
                    try:
                        os.remove(rrd_file)
                    except OSError, e:
                        print e

                else:
                    self.__update_db(type, rrd_info_day,   rrd_name, 'day')
                    self.__update_db(type, rrd_info_week,  rrd_name, 'week')
                    self.__update_db(type, rrd_info_month, rrd_name, 'month')
                    self.__update_db(type, rrd_info_year,  rrd_name, 'year')


    def run (self) :
        self.__startup()
        
        while 1:

            try:

                # let's go to party...
                for path in [ "host", "net", "global" ]:
                    self.__update(path)
                self.__sec_update()

                # sleep to next iteration
                print "\nControl panel update finished at %s" % \
                    time.strftime('%Y-%m-%d %H:%M:%S', 
                                  time.localtime(time.time()))
                print "Next iteration in %d seconds...\n\n"%(int(Const.SLEEP))
                sys.stdout.flush()

                time.sleep(float(Const.SLEEP))

            except KeyboardInterrupt:
                sys.exit()

            except Exception, e:
                print >> sys.stderr, "Unexpected exception:", e


        # never reached..
        self.__cleanup()

