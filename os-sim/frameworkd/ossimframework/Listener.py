import threading, socket, sys, SocketServer, os
from time import sleep

import Const
import Action
from DoNessus import DoNessus

class FrameworkBaseRequestHandler(SocketServer.StreamRequestHandler):

    __nessus = None

    def handle(self):

        print __name__, ":", "Request from", self.client_address

        while 1:
            try:
                line= self.rfile.readline()
                if len(line) > 0:
                    self.process_data(line)
                    line = ""
                else:
                    return
            except IndexError:
                pass
            except socket.error:
                return
            except AttributeError:
                return


            sleep(1)

        return

    def process_data(self, line):

        print __name__, ":", line

        command = None
        subcommand = None
        sensors = None
        param = None
        extra_data = None

        try:
            command = line.split()[0]

            #
            # FIXME:
            #
            # line *should* be:
            #        command attr1="blabla" attr2="blabla" attr3="blabla"
            # for example:
            #        nessus action="start"
            #        nessus action="stop"
            #
            subcommand = line.split()[1]
            sensors = param = line.split()[2]
        except ValueError, IndexError:
            pass

        #
        #  TODO:
        #  move all this code to an external class in DoNessus.py
        #  for example, NessusManager
        #

        sensor_list = []

        if command == "nessus":

            if FrameworkBaseRequestHandler.__nessus == None:
                FrameworkBaseRequestHandler.__nessus = DoNessus()

            if subcommand == "report":
                extra_data = line.split()[3]
                # Ugly variable confusion
                title =  sensors
                sensor_list = extra_data.split(",")
                print __name__, ": Report host list:", sensor_list
                FrameworkBaseRequestHandler.__nessus.generate_report(title, sensor_list)

            if subcommand == "start":

                # I know this is dirty but is_digit isn't working "as is". 
                try: 
                    int(sensors)
                    print __name__, ": Got schedule request."
                    sensor_list = FrameworkBaseRequestHandler.__nessus.get_sensors_by_id(sensors)
                    print __name__, ": Sensor_list_1:", sensor_list
                    if sensor_list == False:
                        self.wfile.write("wrong scheduler id\n")
                        print __name__, ": wrong scheduler id."
                        return
                except ValueError, e:
                    sensor_list = sensors.split(",")
                    print __name__, ": Splitting up sensors."

                print __name__, ": Sensor_list:", sensor_list

                if FrameworkBaseRequestHandler.__nessus.status() == 0:
                    try:
                        FrameworkBaseRequestHandler.__nessus.load_sensors(sensor_list)
                        FrameworkBaseRequestHandler.__nessus.start()
                        self.wfile.write("ok\n")
                    except AssertionError: 
                        FrameworkBaseRequestHandler.__nessus.load_sensors(sensor_list)
                        FrameworkBaseRequestHandler.__nessus.run()
                        self.wfile.write("ok\n")
                elif FrameworkBaseRequestHandler.__nessus.status() > 0 :
                    print __name__, ": scan already started, status:", FrameworkBaseRequestHandler.__nessus.status()
                    self.wfile.write("Scan already started, status: " + str(FrameworkBaseRequestHandler.__nessus.status()) + "%\n")
            if subcommand == "status":
                print __name__, ": status:", FrameworkBaseRequestHandler.__nessus.status()
                self.wfile.write(str(FrameworkBaseRequestHandler.__nessus.status()) + "\n")
            if subcommand == "reset" and FrameworkBaseRequestHandler.__nessus.status() == -1:
                print __name__, ": Resetting status"
                self.wfile.write("Resetting status\n")
                FrameworkBaseRequestHandler.__nessus.reset_status()
            if FrameworkBaseRequestHandler.__nessus.status() == -1:
                print __name__, ": Previous scan aborted raising errors, please check your logfile. Error: " + \
                FrameworkBaseRequestHandler.__nessus.get_error()
                self.wfile.write("Previous scan aborted raising errors, please check your logfile. Error: " + \
                str(FrameworkBaseRequestHandler.__nessus.get_error()) + "\n")
            if subcommand == "archive":
                print __name__, ": Got archive request for", param
                FrameworkBaseRequestHandler.__nessus.archive(param)
                self.wfile.write("nessus archive ack " + param + "\n")
            if subcommand == "delete":
                print __name__, ": Got delete request for", param
                if param.endswith(".report"):
                    FrameworkBaseRequestHandler.__nessus.delete(param, True)
                else:
                    FrameworkBaseRequestHandler.__nessus.delete(param, False)
                self.wfile.write("nessus delete ack " + param + "\n")
            if subcommand == "restore":
                print __name__, ": Got restore request for", param
                FrameworkBaseRequestHandler.__nessus.restore(param)
                self.wfile.write("nessus restore ack " + param + "\n")
        else:
            a = Action.Action(line)
            a.start()

        return

class Listener(threading.Thread):

    def __init__(self):

        self.__server = None
        threading.Thread.__init__(self)


    def run(self):

        try:
            serverAddress = ("", int(Const.LISTENER_PORT))
            self.__server = SocketServer.TCPServer(serverAddress,
                                                   FrameworkBaseRequestHandler)
        except socket.error, e:
            print __name__, ":", e
            sys.exit()

        self.__server.serve_forever()

if __name__ == "__main__":

    listener = Listener()
    listener.start()

# vim:ts=4 sts=4 tw=79 expandtab:
