import sys, re
from OssimDB import OssimDB
import Const

class OssimConf :

    def __init__ (self, config_file) :
        self.__conf = {}
        self.__get_conf(config_file)

    def __setitem__ (self, key, item) :
        self.__conf[key] = item

    def __getitem__ (self, key) :
        if key in self.__conf:
            return self.__conf[key]
        else:
            return None

    def __repr__ (self):
        repr = ""
        for key, item in self.__conf.iteritems():
            repr += "%s\t: %s\n" % (key, item)
        return repr


    def __get_conf (self, config_file) :

        # Read config from file
        #
        try:
            config = open(config_file)
        except IOError, e:
            print "Error opening OSSIM configuration file (%s)" % e
            sys.exit()
       
        pattern = re.compile("^(\S+)\s*=\s*(\S+)")

        for line in config:
            result = pattern.match(line)
            if result is not None:
                (key, item) = result.groups()
                self[key] = item
       
        config.close()

        # Now, complete config info from Ossim database
        #
        db = OssimDB()
        db.connect(self["ossim_host"], 
                   self["ossim_base"], 
                   self["ossim_user"],
                   self["ossim_pass"])
        hash = db.exec_query("SELECT * FROM config")
        for row in hash:
            # values declared at config file override the database ones
            if row["conf"] not in self.__conf:
                self[row["conf"]] = row["value"]
        db.close()


if __name__ == "__main__":
    c = OssimConf(Const.CONFIG_FILE)
    print c


