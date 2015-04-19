import os, sys, time, re
import urllib

from OssimConf import OssimConf
from OssimDB import OssimDB
import threading
import Const


class AcidCache (threading.Thread) :

    __UPDATE_DB     = "_update_db.php"
    __STAT_ALERTS   = "_stat_alerts.php?sort_order=occur_d"
    __STAT_UADDR1   = "_stat_uaddr.php?addr_type=1%26sort_order=occur_d"
    __STAT_UADDR2   = "_stat_uaddr.php?addr_type=2%26sort_order=occur_d"
    __STAT_PORTS    = "_stat_ports.php?port_type=2%26proto=-1%26sort_order=dip_d"

    def __init__ (self) :
        self.__conf = OssimConf (Const.CONFIG_FILE)
        self.__urls = {}
        threading.Thread.__init__(self)

    def run (self) :

        # default scheme and ip values
        acid_scheme = ossim_scheme = "http://"
        acid_ip = ossim_ip = "127.0.0.1"

        # get ossim and acid links from config
        acid_link = self.__conf["acid_link"]+"/" or "http://localhost/acid/"
        acid_prefix = self.__conf["event_viewer"] or "acid"
        ossim_link = self.__conf["ossim_link"] or "http://localhost/ossim/"

        self.__urls = { 
            acid_prefix + "_update_db" :      AcidCache.__UPDATE_DB, 
            acid_prefix + "_stat_alerts" :    AcidCache.__STAT_ALERTS,
            acid_prefix + "_stat_uaddr1" :    AcidCache.__STAT_UADDR1,
            acid_prefix + "_stat_uaddr2" :    AcidCache.__STAT_UADDR2,
            acid_prefix + "_stat_ports2" :    AcidCache.__STAT_PORTS 
        }

        # web authentication?
        ossim_web_user = self.__conf["ossim_web_user"] or "ossim"
        ossim_web_pass = self.__conf["ossim_web_pass"] or "ossim"

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
                self.__urls[key] = "%s%s:%s@%s%s%s%s%s%s&user=%s&pass=%s" % \
                    (acid_scheme, ossim_web_user, ossim_web_pass, ossim_ip,
                     ossim_link, session, acid_link, acid_prefix, url, acid_user, acid_pass)
            else:
                self.__urls[key] = "%s%s%s%s%s%s%s&user=%s&pass=%s" % \
                    (ossim_scheme, ossim_ip, ossim_link, session, acid_link, 
                     acid_prefix, url, acid_user, acid_pass) 

        while 1:

            for key, url in self.__urls.iteritems():
                try:
                    fname = self.__conf["acid_path"] + "/" + key + ".html"
                    # TODO: hid the passwords!
                    print __name__, ': Fetching %s from "%s"' % (fname, url)

                    fin = urllib.urlopen(url)
                    fout = open (fname, "w")
                    fout.writelines(fin.readlines())
                    fin.close()
                    fout.close()

                except Exception, e:
                    print __name__, ":", e
 
            time.sleep(float(Const.SLEEP))


# vim:ts=4 sts=4 tw=79 expandtab:
