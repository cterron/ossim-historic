import os, sys, time, signal
from OssimDB import OssimDB
from optparse import OptionParser
import Const

class Framework:

    def __init__ (self):
        self.__classes = [ "ControlPanel", "AcidCache" ]


    def __parse_options(self):
        
        usage = "%prog [-d] [-v] [-s delay] [-c config_file]"
        parser = OptionParser(usage = usage)
        parser.add_option("-v", "--verbose", dest="verbose", 
                          action="store_true", help="make lots of noise")
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


    def __daemonize(self):

        try:
            print "OSSIM Framework: Forking into background..."
            pid = os.fork()
            if pid > 0: sys.exit(0)
        except OSError, e:
            print >>sys.stderr, "fork failed: %d (%s)" % (e.errno, e.strerror)
            sys.exit(1)


    def waitforever(self):
        """Wait for a Control-C and kill all threads"""

        while 1:
            try:
                time.sleep(1)
            except KeyboardInterrupt:
                pid = os.getpid()
                os.kill(pid, signal.SIGTERM)


    def startup (self) :
        options = self.__parse_options()

        # configuration file
        if options.config_file is not None:
            Const.CONFIG_FILE = options.config_file

        if options.sleep is not None:
            Const.SLEEP = options.sleep

        # log directory
        if not os.path.isdir(Const.LOG_DIR):
            os.mkdir(Const.LOG_DIR, 0755)

        # daemonize
        if options.daemon is not None:
            self.__daemonize()

            # Redirect error file descriptor
            sys.stderr = open(os.path.join
                (Const.LOG_DIR, 'frameworkd_error.log'), 'w')

            # Redirect standard file descriptors (daemon mode)
            sys.stdin  = open('/dev/null', 'r')
            sys.stdout = open(os.path.join(Const.LOG_DIR, 'frameworkd.log'), 'w')


    def main(self):
        for c in self.__classes :
            exec "from %s import %s" % (c, c)
            exec "t = %s()" % (c)
            t.start()


if __name__ == "__main__" :
    
    f = Framework()
    f.startup()
    f.main()
    f.waitforever()

