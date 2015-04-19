#!/usr/bin/python2.3

import sys, os, string, re, time
from optparse import OptionParser
import MySQLdb

RRD_BIN = "/usr/bin/rrdtool"  # overriden if there is an entry at ossim.conf
SLEEP   = 300                 # overriden by -s command line option
LOG_DIR = "/var/log/ossim/"


def global_update(st, rrdtool_bin, rrd_path):
    __update(st, "global", rrdtool_bin, rrd_path)
    
def net_update(st, rrdtool_bin, rrd_path):
    __update(st, "net", rrdtool_bin, rrd_path)

def host_update(st, rrdtool_bin, rrd_path):
    __update(st, "host", rrdtool_bin, rrd_path)

def level_update(st, rrdtool_bin, rrd_path):
    __sec_update(st, rrdtool_bin, rrd_path)
    

# update all rrds in rrd_path
def __update(st, type, rrdtool_bin, rrd_path):

    for rrd_file in os.listdir(rrd_path):
        
        if rrd_file.rfind(".rrd") != -1:
        
            rrd_file = os.path.join(rrd_path, rrd_file)
            rrd_name = os.path.basename(rrd_file.split(".rrd")[0])
            
            rrd_info_day   = get_rrd_value("N-1D", "N", rrdtool_bin, rrd_file)
            rrd_info_week  = get_rrd_value("N-7D", "N", rrdtool_bin, rrd_file)
            rrd_info_month = get_rrd_value("N-1M", "N", rrdtool_bin, rrd_file)
            rrd_info_year  = get_rrd_value("N-1Y", "N", rrdtool_bin, rrd_file)

            __update_db(st, type, rrd_info_day,   rrd_name, 'day')
            __update_db(st, type, rrd_info_week,  rrd_name, 'week')
            __update_db(st, type, rrd_info_month, rrd_name, 'month')
            __update_db(st, type, rrd_info_year,  rrd_name, 'year')


# connect to ossim database
def db_connect(conf):
    
    if conf["OSSIM_TYPE"] == 'mysql':

        if not conf.has_key("OSSIM_PASS"):
            conf["OSSIM_PASS"] = ""

        db = MySQLdb.connect(host   = conf["OSSIM_HOST"],
                             db     = conf["OSSIM_BASE"],
                             user   = conf["OSSIM_USER"],
                             passwd = conf["OSSIM_PASS"])
        st = db.cursor()
        return (db, st)
        
    else:
        print >>sys.stderr, "Database not supported"
        sys.exit()


# update the rrd database entry
def __update_db(st, type, info, rrd_name, range):

    query = """
        DELETE FROM control_panel 
            WHERE id = '%s' AND rrd_type = '%s' AND time_range = '%s'
        """ % (rrd_name, type, range)

    st.execute(query)

    if not (type == 'host' and info["max_c"] == 0 and info["max_a"] == 0):

        query = """
            INSERT INTO control_panel 
            (id, rrd_type, time_range, max_c, max_a, max_c_date, max_a_date)
            VALUES ('%s', '%s', '%s', %f, %f, '%s', '%s')
            """ % (rrd_name, type, range, info["max_c"], info["max_a"],
                   info["max_c_date"], info["max_a_date"])

        st.execute(query)

        print "updating %s (%s):    \tC=%f, A=%f" % \
            (rrd_name, range, info["max_c"], info["max_a"])
        sys.stdout.flush()


# get value from config table
def get_db_config(st, key):

    query = "SELECT value FROM config WHERE conf = '%s'" % (key)
    st.execute(query)
    res = st.fetchone()
    return res[0]


def __sec_update(st, rrdtool_bin, rrd_path):

    for rrd_file in os.listdir(rrd_path):

        result = re.findall("level_(\w+)\.rrd", rrd_file)
        if result != []:

            user = result[0]

            rrd_file = os.path.join(rrd_path, rrd_file)
            rrd_name = os.path.basename(rrd_file.split(".rrd")[0])

            threshold = get_db_config(st, "threshold")


            # get last value for day level
            rrd_info = get_rrd_value("N-15m", "N", rrdtool_bin, rrd_file)

            query = """
        UPDATE control_panel
            SET c_sec_level = %f, a_sec_level = %f
            WHERE id = 'global_%s' and time_range = 'day'
        """ % (rrd_info["max_c"], rrd_info["max_a"], user)

            print "updating level (day):  C=%s%%, A=%s%%" % \
                (rrd_info["max_c"], rrd_info["max_a"])

            st.execute(query)


            # calculate average for week, month and year levels
            for range in ["week", "month", "year"]:

                range2date = {
                    "day"  : "N-1D",
                    "week" : "N-7D",
                    "month": "N-1M",
                    "year" : "N-1Y",
                }

                output = os.popen("%s fetch %s AVERAGE -s %s -e N" % \
                    (rrdtool_bin, rrd_file, range2date[range]))

                pattern = "(\d+):\s+(\S+)\s+(\S+)"
                C_level = A_level = count = 0
                for line in output.readlines():
                    result = re.findall(pattern, line)
                    if result != []:
                        (date, compromise, attack) = result[0]
                        if compromise != "nan" and attack != "nan":
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

                st.execute(query)


# return a tuple with the date of C and A max values
def __get_max_date(start, end, rrdtool_bin, rrd_file):
    
    max_c = max_a = 0.0
    max_c_date = max_a_date = c_date = a_date = ""
    
    # execute rrdtool fetch and obtain max c & a date
    cmd = "%s fetch %s MAX -s %s -e %s" % (rrdtool_bin, rrd_file, start, end)
    output = os.popen(cmd)
    pattern = "(\d+):\s+(\S+)\s+(\S+)"
    for line in output.readlines():
        result = re.findall(pattern, line)
        if result != []:
            (date, compromise, attack) = result[0]
            if compromise != "nan" and attack != "nan":
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
def get_rrd_value(start, end, rrdtool_bin, rrd_file):
   
    rrd_info = {}

    # C max 
    # (2nd line of rrdtool graph ds0)
    cmd = "%s graph /dev/null -s %s -e %s -X 2 DEF:obs=%s:ds0:AVERAGE PRINT:obs:MAX:%%lf" % (rrdtool_bin, start, end, rrd_file)
    output = os.popen(cmd)
    output.readline()
    c_max = output.readline()
    output.close()
   
    # ignore 'nan' values
    if c_max != "nan\n":
        rrd_info["max_c"] = float(c_max)
    else:
        rrd_info["max_c"] = 0
    
    # A max 
    # (2nd line of rrdtool graph ds1)
    cmd = "%s graph /dev/null -s %s -e %s -X 2 DEF:obs=%s:ds1:AVERAGE PRINT:obs:MAX:%%lf" % (rrdtool_bin, start, end, rrd_file)
    output = os.popen(cmd)
    output.readline()
    a_max = output.readline()
    output.close()

    # ignore 'nan' values
    if a_max != "nan\n":
        rrd_info["max_a"] = float(a_max)
    else:
        rrd_info["max_a"] = 0

    (rrd_info["max_c_date"], rrd_info["max_a_date"]) = \
        __get_max_date(start, end, rrdtool_bin, rrd_file)

    return rrd_info


# Get config options from /etc/ossim/framework/ossim.conf
def get_conf(config_file = "/etc/ossim/framework/ossim.conf"):
    
    conf = {}

    try:
        config = open(config_file)
    except IOError, e:
        print e
        sys.exit()

    for line in config:
        
        result = re.findall("^ossim_type\s*=\s*(\S+)", line)
        if result != []:
            conf["OSSIM_TYPE"] = result[0]
        result = re.findall("^ossim_base\s*=\s*(\S+)", line)
        if result != []:
            conf["OSSIM_BASE"] = result[0]
        result = re.findall("^ossim_user\s*=\s*(\S+)", line)
        if result != []:
            conf["OSSIM_USER"] = result[0]
        result = re.findall("^ossim_pass\s*=\s*(\S+)", line)
        if result != []:
            conf["OSSIM_PASS"] = result[0]
        result = re.findall("^ossim_host\s*=\s*(\S+)", line)
        if result != []:
            conf["OSSIM_HOST"] = result[0]
        
        result = re.findall("^mrtg_rrd_files_path\s*=\s*(\S+)", line)
        if result != []:
            conf["MRTG_RRD_FILES_PATH"] = result[0]
        
        result = re.findall("^rrdtool_path\s*=\s*(\S+)", line)
        if result != []:
            conf["RRDTOOL_PATH"] = result[0]
        
        
    config.close
    return conf

# Run script in daemon mode
def daemonize():

    try:
        print "control_panel: Forking into background..."
        pid = os.fork()
        if pid > 0: sys.exit(0)
    except OSError, e:
        print >>sys.stderr, "fork failed: %d (%s)" % (e.errno, e.strerror)
        sys.exit(1)

# Parse command line options
def parse_options():
    
    parser = OptionParser(usage = "%prog [-d] [-v] [-s delay] [-c config_file]")
    parser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                      help="make lots of noise")
    parser.add_option("-d", "--daemon", dest="daemon", action="store_true",
                      help="Run script in daemon mode")
    parser.add_option("-s", "--sleep", dest="sleep", action="store",
                      help = "delay between iterations (seconds)", 
                      metavar="delay")
    parser.add_option("-c", "--config", dest="config_file", action="store",
                       help = "read config from FILE", metavar="FILE")
    (options, args) = parser.parse_args()

    if options.verbose and options.daemon:
        parser.error("incompatible options -v -d")
    
    return options

def main():

    options = parse_options()

    # seconds between iterations
    if options.sleep is None:
        options.sleep = SLEEP

    # read config from framework/ossim.conf
    if options.config_file is not None:
        conf = get_conf(options.config_file)
    else:
        conf = get_conf()

    # daemonize
    if options.daemon is not None:
        daemonize()
        sys.stderr = open(os.path.join(LOG_DIR,'control_panel_error.log'),'w')

    # Redirect standard file descriptors (daemon mode)
    if not options.verbose:
        if not os.path.isdir(LOG_DIR):
            os.mkdir(LOG_DIR, 0755)
        sys.stdin  = open('/dev/null', 'r')
        sys.stdout = open(os.path.join(LOG_DIR, 'control_panel.log'), 'w')
    
    # db connect
    (db, st) = db_connect(conf)

    # get extra config values from config table
    if not conf.has_key("MRTG_RRD_FILES_PATH"):
        conf["MRTG_RRD_FILES_PATH"] = get_db_config(st, "mrtg_rrd_files_path")
    if not conf.has_key("RRDTOOL_PATH"):
        conf["RRDTOOL_PATH"] = get_db_config(st, "rrdtool_path")

    # where are the rrd files?
    rrdtool_bin = RRD_BIN
    if conf.has_key("RRDTOOL_PATH"):
        rrdtool_bin = os.path.join(conf["RRDTOOL_PATH"], "rrdtool")
    try:
        global_path = os.path.join(conf["MRTG_RRD_FILES_PATH"], 
                                   'global_qualification')
        net_path  = os.path.join(conf["MRTG_RRD_FILES_PATH"], 
                                 'net_qualification')
        host_path = os.path.join(conf["MRTG_RRD_FILES_PATH"], 
                                 'host_qualification')
        level_path = os.path.join(conf["MRTG_RRD_FILES_PATH"], 
                                 'level_qualification')
    except OSError, e:
        print >>sys.stderr, "Error reading RRD path:"
        print >>sys.stderr, e
        sys.exit()

    while 1:

        try:

            # let's go to party...
            global_update(st, rrdtool_bin, global_path)
            level_update(st, rrdtool_bin, level_path)
            net_update(st, rrdtool_bin, net_path)
            host_update(st, rrdtool_bin, host_path)

            # sleep to next iteration
            print "\ncontrol panel update finished at %s" % \
                time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
            print "Next iteration in %d seconds...\n\n" % (int(options.sleep))
            sys.stdout.flush()

            time.sleep(float(options.sleep))

        except KeyboardInterrupt:
            sys.exit()

        except Exception, e:
            print >> sys.stderr, "Unexpected exception:", e

    db.close()


main()
    

