import thread, time, socket, string, sys, re

from Config import Conf, Plugin
from Logger import Logger
logger = Logger.logger
from Watchdog import Watchdog
from Event import WatchRule
from MonitorScheduler import MonitorScheduler

class ServerConn:

    __conn = None

    MSG_CONNECT         = 'connect id="%s" type="sensor"\n'
    MSG_APPEND_PLUGIN   = 'session-append-plugin id="%s" ' +\
                          'plugin_id="%s" enabled="%s" state="%s"\n'

    def __init__(self, conf, plugins):
        self.conf = conf
        self.server_ip = self.conf.get("output-server", "ip")
        self.server_port = self.conf.get("output-server", "port")
        self.plugins = plugins
        self.sequence = 0

        self.monitor_scheduler = MonitorScheduler()
        self.monitor_scheduler.start()


    # connect to server
    #  attempts == 0 means that agent try to connect forever
    #  waittime = seconds between attempts
    def connect(self, attempts = 3, waittime = 10.0):

        self.sequence = 1
        count = 1

        if self.__conn is None:

            logger.info("Connecting to server (%s, %s).." \
                % (self.server_ip, self.server_port))

            while 1:

                self.__connect_to_server()
                if self.__conn is not None:
                    self.__append_plugins()
                    break

                else:
                    logger.info("Can't connect to server, " +\
                                "retrying in %d seconds" % (waittime))
                    time.sleep(waittime)

                # check #attempts
                if attempts != 0 and count == attempts:
                    break
                count += 1

        else:
            logger.info("Reusing server connection (%s, %s).." \
                % (self.server_ip, self.server_port))

        return self.__conn


    def close(self):
        logger.info("Closing server connection..")
        if self.__conn is not None:
            self.__conn.close()
            self.__conn = None


    # Reset the current connection by closing and reopening it
    def reconnect(self, attempts = 0, waittime = 10.0):

        self.close()
        time.sleep(1)
        while 1:
            if self.connect(attempts, waittime) is not None:
                break


    def send(self, msg):

        while 1:
            try:
                self.__conn.send(msg)
            except socket.error, e:
                logger.error(e)
                self.reconnect()
            except AttributeError: # self.__conn == None
                self.reconnect()
            else:
                logger.debug(msg)
                break


    def __connect_to_server(self):

        self.__conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        data = ""
        try:
            self.__conn.connect((self.server_ip, int(self.server_port)))
            self.__conn.send(self.MSG_CONNECT % (self.sequence))
            logger.debug("Waiting for server..")
            data = self.__conn.recv(1024)
        except socket.error, e:
            logger.error("Error connecting to server: " + str(e))
            self.__conn = None
        else:
            if data == 'ok id="' + str(self.sequence) + '"\n':
                logger.info("Server connected!")
            else:
                logger.error("Bad response from server: %s" % (str(data)))
                self.__conn = None

        return self.__conn


    def __append_plugins(self):

        logger.debug("Apending plugins..")
        msg = ''

        for plugin in self.plugins:
            self.sequence += 1
            if plugin.getboolean("config", "enable"):
                msg = self.MSG_APPEND_PLUGIN % \
                        (str(self.sequence),
                        plugin.get("config", "plugin_id"),
                        'true', 'start')
            else:
                msg = self.MSG_APPEND_PLUGIN % \
                        (str(self.sequence),
                        plugin.get("config", "plugin_id"),
                        'false', 'stop')
            self.send(msg)


    def recv_line(self):

        char = data = ''

        while 1:
            try:
                char = self.__conn.recv(1)
                data += char
                if char == '\n':
                    break
            except socket.error, e:
                logger.error('Error receiving data from server: ' + str(e))
                time.sleep(10)
                self.reconnect()
            except AttributeError:
                logger.error('Error receiving data from server')
                time.sleep(10)
                self.reconnect()

        return data


    # receive control messages from server
    def __recv_control_messages(self):

        ####### watch-rule test #######
        if (0):
            time.sleep(1)
            data = 'watch-rule plugin_id="2001" ' +\
               'plugin_sid="1" condition="gt" value="1" ' +\
               'from="127.0.0.1" to="127.0.0.1" ' +\
               'port_from="4566" port_to="22"'
            self.__control_monitors(data)
        ###############################

        while 1:

            try:
                # receive message from server (line by line)
                data = self.recv_line()
                logger.info("Received message from server: " + data)

                # 1) type of control messages: plugin management
                #    (start, stop, enable and disable plugins)
                #
                if data.startswith(Watchdog.PLUGIN_START_REQ) or \
                   data.startswith(Watchdog.PLUGIN_STOP_REQ) or \
                   data.startswith(Watchdog.PLUGIN_ENABLE_REQ) or \
                   data.startswith(Watchdog.PLUGIN_DISABLE_REQ):

                    self.__control_plugins(data)

                # 2) type of control messages: watch rules (monitors)
                #
                elif data.startswith('watch-rule'):

                    self.__control_monitors(data)

            except Exception, e:
                logger.error(
                    'Unexpected exception receiving from server: ' + str(e))


    def __control_plugins(self, data):

        # get plugin_id of process to start/stop/enable/disable
        pattern = re.compile('(\S+) plugin_id="([^"]*)"')
        result = pattern.search(data)
        if result is not None:
            (command, plugin_id) = result.groups()
        else:
            logger.warning("Bad message from server: %s" % (data))
            return

        # get plugin from plugin list searching by the plugin_id given
        for plugin in self.plugins:
            if int(plugin.get("config", "plugin_id")) == int(plugin_id):

                if command == Watchdog.PLUGIN_START_REQ:
                    Watchdog.start_process(plugin)

                elif command == Watchdog.PLUGIN_STOP_REQ:
                    Watchdog.stop_process(plugin)

                elif command == Watchdog.PLUGIN_ENABLE_REQ:
                    Watchdog.enable_process(plugin)

                elif command == Watchdog.PLUGIN_DISABLE_REQ:
                    Watchdog.disable_process(plugin)

                break

    def __control_monitors(self, data):

        # build a watch rule, the server request.
        watch_rule = WatchRule()
        for attr in watch_rule.EVENT_ATTRS:
            pattern = ' %s="([^"]*)"' % (attr)
            result = re.findall(pattern, data)
            if result != []:
                watch_rule[attr] = result[0]

        for plugin in self.plugins:

            # look for the monitor to be called
            if plugin.get("config", "plugin_id") == watch_rule['plugin_id'] and\
               plugin.get("config", "type").lower() == 'monitor':

                self.monitor_scheduler.\
                    new_monitor(type = plugin.get("config", "source"),
                                plugin = plugin,
                                watch_rule = watch_rule)
                break


    # launch new thread to manage control messages
    def control_messages(self):
        thread.start_new_thread(self.__recv_control_messages, ())


# vim:ts=4 sts=4 tw=79 expandtab:

