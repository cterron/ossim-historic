import os, sys, time, re
import urllib

from OssimConf import OssimConf
from OssimDB import OssimDB
import threading
import Const


class AcidCache (threading.Thread) :

    __UPDATE_DB     = "/acid_update_db.php"
    __STAT_ALERTS   = "/acid_stat_alerts.php?sort_order=occur_d"
    __STAT_UADDR1   = "/acid_stat_uaddr.php?addr_type=1&sort_order=occur_d"
    __STAT_UADDR2   = "/acid_stat_uaddr.php?addr_type=2&sort_order=occur_d"
    __STAT_PORTS    = "/acid_stat_ports.php?port_type=2&proto=-1&sort_order=dip_d"

    def __init__ (self) :
        self.__conf = OssimConf (Const.CONFIG_FILE)
        self.__urls = { 
            "acid_update_db" :      AcidCache.__UPDATE_DB, 
            "acid_stat_alerts" :    AcidCache.__STAT_ALERTS,
            "acid_stat_uaddr1" :    AcidCache.__STAT_UADDR1,
            "acid_stat_uaddr2" :    AcidCache.__STAT_UADDR2,
            "acid_stat_ports2" :    AcidCache.__STAT_PORTS 
        }
        threading.Thread.__init__(self)

    def run (self) :

        # default scheme and ip values
        acid_scheme = ossim_scheme = "http://"
        acid_ip = ossim_ip = "127.0.0.1"

        # get ossim and acid links from config
        acid_link = self.__conf["acid_link"] or "http://localhost/acid/"
        ossim_link = self.__conf["ossim_link"] or "http://localhost/ossim/"

        # web authentication?
        acid_web_user = self.__conf["acid_web_user"] or "ossim"
        acid_web_pass = self.__conf["acid_web_pass"] or "ossim"

        result = re.compile("(\w+:\/\/)(.*?)(\/.*)$").search(acid_link)
        if result is not None:
            (acid_scheme, acid_ip, acid_link) = result.groups()

        result = re.compile("(\w+:\/\/)(.*?)(\/.*)$").search(ossim_link)
        if result is not None:
            (ossim_scheme, ossim_ip, ossim_link) = result.groups()

        # get target urls
        acid_user = self.__conf["acid_user"]
        acid_pass = self.__conf["acid_pass"]
        session = "/session/login.php?dest="
        for key, url in self.__urls.iteritems():

            if self.__conf["acid_user"]:
                self.__urls[key] = "%s%s:%s@%s%s%s%s%s&user=%s&pass=%s" % \
                    (ossim_scheme, acid_web_user, acid_web_pass, ossim_ip,
                     ossim_link, session, acid_link, url, acid_user, acid_pass)
            else:
                self.__urls[key] = "%s%s%s%s%s%s&user=%s&pass=%s" % \
                    (ossim_scheme, ossim_ip, ossim_link, session, acid_link, 
                     url, acid_user, acid_pass) 

        while 1:

            for key, url in self.__urls.iteritems():
                try:
                    fname = self.__conf["acid_path"] + "/" + key + ".html"
                    print 'Fetching %s..' % (fname)

                    fin = urllib.urlopen(url)
                    fout = open (fname, "w")
                    fout.writelines(fin.readlines())
                    fin.close()
                    fout.close()

                except Exception, e:
                    print e
 
            time.sleep(float(Const.SLEEP))


