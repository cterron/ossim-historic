import os, sys, time, re

import Const, Util, threading, tempfile
from OssimDB import OssimDB
from OssimConf import OssimConf

class Scheduler(threading.Thread):

    def __init__(self):
        self.__conf = OssimConf(Const.CONFIG_FILE)
        self.__db = OssimDB()
        self.__stored_id = 0
        self.__stored_num = 0
        self.__header_id = 0
        self.__set_debug = True
        threading.Thread.__init__(self)

    def __debug (self, message) :
        if self.__set_debug == True :
            print message

    def __check_last_db_id(self):
        db_last_id = self.__get_last_db_id()

        if db_last_id == self.__stored_id:
            # we're up to date
            return False
        return True

    def __check_db_scheduler_count(self):
        db_id_num = self.__get_db_scheduler_count()

        if db_id_num == self.__stored_num:
            # we're up to date
            return False
        return True

    def __get_last_db_id(self):
        query = "select max(id) as id from plugin_scheduler"
        hash = self.__db.exec_query(query)

        for row in hash:
            return row["id"]

        return 0

    def __get_db_scheduler_count(self):
        query = "select count(id) as id from plugin_scheduler"
        hash = self.__db.exec_query(query)

        for row in hash:
            return row["id"]

        return 0

    def __get_crontab(self):
         crontab = []

         cmd = "crontab -l"
         output = os.popen(cmd)

         pattern = "#### OSSIM scheduling information, everything below this line will be erased. Last schedule:\s*\((\d+)\)\s* ####"

         for line in output.readlines():
             result = re.findall(pattern, line)
             if result != []:
             # We fond our header. Let's see how many entries are in there and
             # return without the header line
                 self.__header_id = result[0]
                 output.close()
                 return crontab
             else:
             # Just append the line
                 crontab.append(line)

         # We didn't find the header 
         output.close()
         return crontab

    def __set_crontab(self, crontab):
         if len(crontab) < 1:
             self.__debug("Since at least the warning line has to be present, something went wrong if crontab has less than 1 entry. Not overwriting crontab")
             return False

         tmp_name = tempfile.mktemp(".ossim.scheduler")
         outfile =  open(tmp_name, "w")
         try:
             for line in crontab:
                 outfile.write(line)
         finally:
             outfile.close()

         cmd = "crontab %s" % tmp_name 
         status = os.system(cmd)
         os.unlink(tmp_name)
         if(status < 0):
             return False
         return True

    def run(self):

        self.__db.connect(self.__conf['ossim_host'],
                          self.__conf['ossim_base'],
                          self.__conf['ossim_user'],
                          self.__conf['ossim_pass'])

        while 1:
            
            try:
                # Check if we already have the latest DB id stored in memory
                # during this run
                if self.__check_last_db_id() == True or self.__check_db_scheduler_count() == True:

                    # Let's fetch the crontab up until our header (if present)
                    # and check if we have to recreate it
                    crontab = self.__get_crontab()
                    last_id = self.__get_last_db_id()
                    id_num = self.__get_db_scheduler_count()
                    for line in crontab:
                        self.__debug(line.strip())
                    # Ok, we have to redo the crontab entry
                    ossim_tag = "#### OSSIM scheduling information, everything below this line will be erased. Last schedule: (%d) ####" % int(last_id)
                    self.__debug(ossim_tag)
                    crontab.append(ossim_tag + "\n")

                    query = "SELECT * FROM plugin_scheduler"
                    hash = self.__db.exec_query(query)

                    if self.__conf["frameworkd_dir"]:
                        Const.FRAMEWORKD_DIR = self.__conf["frameworkd_dir"]

                    for row in hash:
                        entry = "%s\t%s\t%s\t%s\t%s\t%s" % (row["plugin_minute"], row["plugin_hour"], row["plugin_day_month"], row["plugin_month"], row["plugin_day_week"], os.path.join(Const.FRAMEWORKD_DIR, "DoNessus.py" + " -i " + str(row["id"])))
                        crontab.append(entry + "\n")
                        self.__debug(entry)

                    
                    self.__debug("Setting crontab")
                    if self.__set_crontab(crontab) == True:
                        self.__debug("Crontab successfully updated")
                        self.__stored_id = self.__header_id = last_id
                        self.__stored_num = id_num
                    else:
                        self.__debug("Crontab not updated, something went wrong (check output)")

            except Exception, e:
                print __name__, e
            self.__debug("Iteration...")
            time.sleep(float(Const.SLEEP))

        # never reached..
        self.__db.close()


if __name__ == "__main__":

    scheduler = Scheduler()
    scheduler.start()

# vim:ts=4 sts=4 tw=79 expandtab:
