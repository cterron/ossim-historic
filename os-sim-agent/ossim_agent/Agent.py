import os, sys, time, signal, string, thread

from Config import Conf, Plugin, Aliases, CommandLineOptions
from ParserLog import ParserLog
from Watchdog import Watchdog
from Logger import Logger
logger = Logger.logger
from Output import Output
from Stats import Stats
from Conn import ServerConn
from Exceptions import AgentCritical
from ParserUnifiedSnort import ParserUnifiedSnort

class Agent:

    def __init__(self):

        # parse command line options
        self.options = CommandLineOptions().get_options()

        # read configuration
        self.conf = Conf()
        if self.options.config_file:
            conffile = self.options.config_file
        else:
            conffile = self.conf.DEFAULT_CONFIG_FILE
        self.conf.read([conffile])

        # aliases
        aliases = Aliases()
        aliases.read([os.path.join(os.path.dirname(conffile), "aliases.cfg")])

        # list of plugins
        self.plugins = []
        for name, path in self.conf.hitems("plugins").iteritems():
            if os.path.exists(path):
                plugin = Plugin()

                # Now read the config file
                plugin.read(path)
                plugin.set("config", "name", name)
                plugin.replace_aliases(aliases)
                plugin.replace_config(self.conf)
                self.plugins.append(plugin)
            else:
                logger.error("Can not read plugin configuration (%s) at (%s)" \
                    % (name, path))

        # server connection (only available if output-server is enabled)
        self.conn = None

        self.detector_objs = []
        self.watchdog = None


    def init_logger(self):

        # open file handlers (main and error logs)
        if self.conf.has_option("log", "file"):
            Logger.add_file_handler(self.conf.get("log", "file"))
        if self.conf.has_option("log", "error"):
            Logger.add_error_file_handler(self.conf.get("log", "error"))

        # adjust verbose level
        verbose = self.conf.get("log", "verbose")
        if self.options.verbose is not None:
            # -v or -vv command line argument
            #  -v -> self.options.verbose = 1
            # -vv -> self.options.verbose = 2
            for i in range(self.options.verbose):
                verbose = Logger.next_verbose_level(verbose)
        Logger.set_verbose(verbose)
            
    def init_stats(self):
        Stats.startup()
        if self.conf.has_section("log"):
            if self.conf.has_option("log", "stats"):
                Stats.set_file(self.conf.get("log", "stats"))

    def init_output(self):

        if self.conf.has_section("output-plain"):
            if self.conf.getboolean("output-plain", "enable"):
                Output.add_plain_output(self.conf)

        # output-server is enabled in connect_server()
        # if the connection becomes availble

        if self.conf.has_section("output-csv"):
            if self.conf.getboolean("output-csv", "enable"):
                Output.add_csv_output(self.conf)

        if self.conf.has_section("output-db"):
            if self.conf.getboolean("output-db", "enable"):
                Output.add_db_output(self.conf)

    def connect_server(self):
        if self.conf.has_section("output-server"):
            if self.conf.getboolean("output-server", "enable"):
                self.conn = ServerConn(self.conf, self.plugins)
                if self.conn.connect(attempts=0, waittime=30):
                    self.conn.control_messages()

                    # init server output
                    if self.conf.has_section("output-server"):
                        if self.conf.getboolean("output-server", "enable"):
                            if self.conn is not None:
                                Output.add_server_output(self.conn)

                else:
                    self.conn = None
                    logger.error("Server connection is now disabled!")


    # check if there is already a running instance
    def check_pid(self):
        pidfile = self.conf.get("daemon", "pid")

        # check for other ossim-agent instances when not using --force argument
        if self.options.force is None and os.path.isfile(pidfile):
            raise AgentCritical("There is already a running instance")

        # remove ossim-agent.pid file when using --force argument
        elif os.path.isfile(pidfile):
            try:
                os.remove(pidfile)
            except OSError, e:
                logger.warning(e)


    def createDaemon(self):
        """Detach a process from the controlling terminal and run it in the
        background as a daemon.

        Note (DK): Full credit for this daemonize function goes to Chad J. Schroeder.
        Found it at ASPN http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/278731
        Please check that url for useful comments on the function.
        """

        # Install a handler for the terminate signals
        signal.signal(signal.SIGTERM, self.terminate)

        # -d command-line argument
        if self.options.daemon:
            self.conf.set("daemon", "daemon", "True")

        if self.conf.getboolean("daemon", "daemon") and \
           self.options.verbose is None:
            logger.info("Forking into background..")

            UMASK = 0
            WORKDIR = "/"
            MAXFD = 1024
            if (hasattr(os, "devnull")):
                REDIRECT_TO = os.devnull
            else:
                REDIRECT_TO = "/dev/null"
         
            try:
                pid = os.fork()
            except OSError, e:
                raise Exception, "%s [%d]" % (e.strerror, e.errno)
                sys.exit(1)
         
            if (pid == 0):  # The first child.
                os.setsid()
         
                try:
                    pid = os.fork()   # Fork a second child.
                except OSError, e:
                    raise Exception, "%s [%d]" % (e.strerror, e.errno)
                    sys.exit(1)
         
                if (pid == 0):       # The second child.
                    os.chdir(WORKDIR)
                    os.umask(UMASK)
                else:
                    open(self.conf.get("daemon", "pid"), 'w').write("%d" % pid)
                    os._exit(0)       # Exit parent (the first child) of the second child.
            else:
                os._exit(0)  # Exit parent of the first child.
 
            import resource         # Resource usage information.
            maxfd = resource.getrlimit(resource.RLIMIT_NOFILE)[1]
            if (maxfd == resource.RLIM_INFINITY):
                maxfd = MAXFD
 
            for fd in range(0, maxfd):
                try:
                    os.close(fd)
                except OSError:      # ERROR, fd wasn't open to begin with (ignored)
                    pass
            os.open(REDIRECT_TO, os.O_RDWR) # standard input (0)
            os.dup2(0, 1)                   # standard output (1)
            os.dup2(0, 2)                   # standard error (2)
            return(0)

    def daemonize(self):
        """
        2007/04/32 DK: Buggy, left here for testing / double checking.
        Should be removed soon as well as the commented reference below.
        """

        # Install a handler for the terminate signals
        signal.signal(signal.SIGTERM, self.terminate)

        # -d command-line argument
        if self.options.daemon:
            self.conf.set("daemon", "daemon", "True")

        if self.conf.getboolean("daemon", "daemon") and \
           self.options.verbose is None:
            Logger.remove_console_handler()
            logger.info("Forking into background..")

            try:
                pid = os.fork()
                if pid > 0:
                    open(self.conf.get("daemon", "pid"), 'w').write("%d" % pid)
                    sys.exit(0)
            except OSError, e:
                logger.critical("fork failed: %s" % (e))
                sys.exit(1)


    def init_plugins(self):

        for plugin in self.plugins:
            if plugin.get("config", "type") == "detector":
                if plugin.get("config", "source") == "log":
                    parser = ParserLog(self.conf, plugin)
                    parser.start()
                    self.detector_objs.append(parser)
                elif plugin.get("config","source") == "snortlog":
                    parser = ParserUnifiedSnort(self.conf,plugin)
                    parser.start()
                    self.detector_objs.append(parser)


    def init_watchdog(self):
        if self.conf.getboolean("watchdog", "enable"):
            self.watchdog = Watchdog(self.conf, self.plugins)
            self.watchdog.start()


    def terminate(self, sig, params):
        self.shutdown()


    def shutdown(self):

        logger.warning("Kill signal received, exiting..")

        # Remove the pid file
        pidfile = self.conf.get("daemon", "pid")
        if os.path.exists(pidfile):
            f = open(pidfile)
            pid_from_file = f.readline()
            f.close()
            try:
                # don't remove the ossim-agent.pid file if it 
                # belongs to other ossim-agent process
                if pid_from_file == str(os.getpid()):
                    os.remove(pidfile)
            except OSError, e:
                logger.warning(e)

        # parsers
        for parser in self.detector_objs:
            if hasattr(parser, 'stop'):
                parser.stop()

        # Watchdog
        if self.watchdog:
            self.watchdog.shutdown()

        # output plugins
        Output.shutdown()

        # execution statistics
        Stats.shutdown()
        if Stats.dates['startup']:
            Stats.stats()

        # kill program
        pid = os.getpid()
        os.kill(pid, signal.SIGKILL)



    # Wait for a Control-C and kill all threads
    def waitforever(self):
        timer = 0
        while 1:
            time.sleep(1)
            timer += 1
            if timer > 30:
                Stats.log_stats()
                timer = 0


    def main(self):

        try:
            self.check_pid()
            self.createDaemon()
            self.init_stats()
            self.init_logger()
            #self.daemonize()
            thread.start_new_thread(self.connect_server, ())
            self.init_output()
            self.init_plugins()
            self.init_watchdog()
            self.waitforever()
        except KeyboardInterrupt:
            self.shutdown()
        except AgentCritical, e:
            logger.critical(e)
            self.shutdown()
        except Exception, e:
            logger.error("Unexpected exception: " + str(e))

            # print trace exception
            import traceback
            traceback.print_exc()

            # print to error.log too
            if self.conf.has_option("log", "error"):
                fd = open(self.conf.get("log", "error"), 'a+')
                traceback.print_exc(file = fd)
                fd.close()

if __name__ == "__main__":
    a = Agent()
    a.main()


# vim:ts=4 sts=4 tw=79 expandtab:
