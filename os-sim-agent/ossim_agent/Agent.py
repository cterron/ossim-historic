import os, sys, time, signal, string

from Config import Conf, Plugin, Aliases, CommandLineOptions
from ParserLog import ParserLog
from Watchdog import Watchdog
from Logger import Logger
logger = Logger.logger
from Output import Output
from Conn import ServerConn
from Exceptions import AgentCritical

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
                plugin.read(path)
                plugin.set("config", "name", name)
                plugin.replace_aliases(aliases)
                self.plugins.append(plugin)
            else:
                logger.error("Can not read plugin configuration (%s) at (%s)" \
                    % (name, path))

        # server connection (only available if output-server is enabled)
        self.conn = None

        self.watchdog = None


    def init_logger(self):
        Logger.add_file_handler(self.conf.get("log", "file"))
        if self.options.verbose == 1:
            self.conf.set("log", "verbose", "info")
        elif self.options.verbose == 2:
            self.conf.set("log", "verbose", "debug")
        Logger.set_verbose(self.conf.get("log", "verbose"))

    def init_output(self):

        if self.conf.has_section("output-plain"):
            if self.conf.getboolean("output-plain", "enable"):
                Output.add_plain_output(self.conf)

        if self.conf.has_section("output-server"):
            if self.conf.getboolean("output-server", "enable"):
                if self.conn is not None:
                    Output.add_server_output(self.conn)

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
                if self.conn.connect():
                    self.conn.control_messages()
                else:
                    self.conn = None
                    logger.error("Server connection is now disabled!")


    # check if there is already a running instance
    def check_pid(self):
        if self.options.force is None:
            if os.path.isfile(self.conf.get("daemon", "pid")):
                raise AgentCritical("There is already a running instance")


    def daemonize(self):

        # Install a handler for the terminate signals
        signal.signal(signal.SIGTERM, self.terminate)

        # -d command-line argument
        if self.options.daemon:
            self.conf.set("daemon", "daemon", "True")

        if self.conf.getboolean("daemon", "daemon"):
            Logger.remove_console_output()
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
            os.remove(pidfile)

        # shutdown output plugins
        if self.watchdog:
            self.watchdog.shutdown()
        Output.shutdown()

        # kill program
        pid = os.getpid()
        os.kill(pid, signal.SIGKILL)



    # Wait for a Control-C and kill all threads
    def waitforever(self):
        while 1:
            time.sleep(1)


    def main(self):

        try:
            self.init_logger()
            self.check_pid()
            self.daemonize()
            self.connect_server()
            self.init_output()
            self.init_plugins()
            self.init_watchdog()
            self.waitforever()
        except KeyboardInterrupt:
            self.shutdown()
        except AgentCritical, e:
            logger.critical(e)
            self.shutdown()

if __name__ == "__main__":
    a = Agent()
    a.main()


# vim:ts=4 sts=4 tw=79 expandtab:
