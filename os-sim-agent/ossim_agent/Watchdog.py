from Config import Conf, Plugin
from Output import Output
from Logger import Logger
logger = Logger.logger

import threading, string, time, os, commands

class Watchdog(threading.Thread):

    # plugin states
    PLUGIN_START_MSG            = 'plugin-process-started'
    PLUGIN_STOP_MSG             = 'plugin-process-stopped'
    PLUGIN_UNKNOWN_MSG          = 'plugin-process-unknown'
    PLUGIN_ENABLE_MSG           = 'plugin-enabled'
    PLUGIN_DISABLE_MSG          = 'plugin-disabled'

    PLUGIN_START_STATE_MSG      = PLUGIN_START_MSG   + " plugin_id=\"%s\"\n"
    PLUGIN_STOP_STATE_MSG       = PLUGIN_STOP_MSG    + " plugin_id=\"%s\"\n"
    PLUGIN_UNKNOWN_STATE_MSG    = PLUGIN_UNKNOWN_MSG + " plugin_id=\"%s\"\n"
    PLUGIN_ENABLE_STATE_MSG     = PLUGIN_ENABLE_MSG  + " plugin_id=\"%s\"\n"
    PLUGIN_DISABLE_STATE_MSG    = PLUGIN_DISABLE_MSG + " plugin_id=\"%s\"\n"

    # plugin server requests
    PLUGIN_START_REQ            = 'sensor-plugin-start'
    PLUGIN_STOP_REQ             = 'sensor-plugin-stop'
    PLUGIN_ENABLE_REQ           = 'sensor-plugin-enable'
    PLUGIN_DISABLE_REQ          = 'sensor-plugin-disable'

    def __init__(self, conf, plugins):

        self.conf = conf
        self.plugins = plugins
        self.interval = self.conf.getfloat("watchdog", "interval") or 3600.0
        threading.Thread.__init__(self)


    # find the process ID of a running program
    def pidof(program):

        pid = string.split(commands.getoutput('pidof %s' % program), ' ')
        if pid[0] == '':
            return None
        else:
            return pid[0]
    pidof = staticmethod(pidof)

    def start_process(plugin):

        id      = plugin.get("config", "plugin_id")
        process = plugin.get("config", "process")
        name    = plugin.get("config", "name")
        command = plugin.get("config", "startup")

        # start service
        if command:
            logger.info("Starting service %s (%s): %s.." % (id, name, command))
            logger.debug(commands.getstatusoutput(command))

        # notify result to server
        if not process:
            logger.info("plugin (%s) has an unknown state" % (name))
            Output.plugin_state(self.PLUGIN_UNKNOWN_STATE_MSG % (id))
        elif Watchdog.pidof(process) is not None:
            logger.info("plugin (%s) is running" % (name))
            Output.plugin_state(Watchdog.PLUGIN_START_STATE_MSG % (id))
        else:
            logger.warning("error starting plugin %s (%s)" % (id, name))
    start_process = staticmethod(start_process)

    def stop_process(plugin):

        id      = plugin.get("config", "plugin_id")
        process = plugin.get("config", "process")
        name    = plugin.get("config", "name")
        command = plugin.get("config", "shutdown")

        # stop service
        if command:
            logger.info("Stopping service %s (%s): %s.." % (id, name, command))
            logger.debug(commands.getstatusoutput(command))

        # notify result to server
        if not process:
            logger.info("plugin (%s) has an unknown state" % (name))
            Output.plugin_state(Watchdog.PLUGIN_UNKNOWN_STATE_MSG % (id))
        elif Watchdog.pidof(process) is None:
            logger.info("plugin (%s) is not running" % (name))
            Output.plugin_state(Watchdog.PLUGIN_STOP_STATE_MSG % (id))
        else:
            logger.warning("error stopping plugin %s (%s)" % (id, name))
    stop_process = staticmethod(stop_process)


    def enable_process(plugin):

        id      = plugin.get("config", "plugin_id")
        name    = plugin.get("config", "name")

        # enable plugin
        plugin.set("config", "enable", "yes")

        # notify to server
        logger.info("plugin (%s) is now enable" % (name))
        Output.plugin_state(Watchdog.PLUGIN_ENABLE_STATE_MSG % (id))
    enable_process = staticmethod(enable_process)

    def disable_process(plugin):

        id      = plugin.get("config", "plugin_id")
        name    = plugin.get("config", "name")

        # disable plugin
        plugin.set("config", "enable", "no")

        # notify to server
        logger.info("plugin (%s) is now disabled" % (name))
        Output.plugin_state(Watchdog.PLUGIN_DISABLE_STATE_MSG % (id))
    disable_process = staticmethod(disable_process)

    def run(self):

        while 1:

            for plugin in self.plugins:

                id      = plugin.get("config", "plugin_id")
                process = plugin.get("config", "process")
                name    = plugin.get("config", "name")

                # 1) unknown process to monitoring
                if not process:
                    logger.info("plugin (%s) has an unknown state" % (name))
                    Output.plugin_state(self.PLUGIN_UNKNOWN_STATE_MSG % (id))

                # 2) process is running
                elif self.pidof(process) is not None:
                    logger.info("plugin (%s) is running" % (name))
                    Output.plugin_state(self.PLUGIN_START_STATE_MSG % (id))

                # 3) process is not running
                else:
                    logger.warning("plugin (%s) is not running" % (name))
                    Output.plugin_state(self.PLUGIN_STOP_STATE_MSG % (id))

                    # restart services (if start=yes in plugin configuration)
                    if plugin.getboolean("config", "start"):
                        self.start_process(plugin)

                # send plugin enable/disable state
                if plugin.getboolean("config", "enable"):
                    logger.info("plugin (%s) is enabled" % (name))
                    Output.plugin_state(self.PLUGIN_ENABLE_STATE_MSG % (id))
                else:
                    logger.warning("plugin (%s) is disabled" % (name))
                    Output.plugin_state(self.PLUGIN_DISABLE_STATE_MSG % (id))


            time.sleep(float(self.interval))


    def shutdown(self):

        for plugin in self.plugins:

            # stop service (if stop=yes in plugin configuration)
            if plugin.getboolean("config", "stop"):
                self.stop_process(plugin)


# vim:ts=4 sts=4 tw=79 expandtab:

