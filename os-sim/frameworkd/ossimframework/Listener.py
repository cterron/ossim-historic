import threading, socket, sys, SocketServer, os, re
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
        try:
            command = line.split()[0]
        except ValueError, IndexError:
            pass

        #
        #  TODO:
        #  move all this code to an external class in DoNessus.py
        #  for example, NessusManager
        #

        if command == "nessus":
            result = re.findall("action=\"([a-z]+)\"", line)
            if result != []:
                action = result[0]
            
            if FrameworkBaseRequestHandler.__nessus == None:
                FrameworkBaseRequestHandler.__nessus = DoNessus()

            if action == "report":
                result = re.findall("title=\"([a-zA-Z+]+)\" list=\"([0-9\.,]*)\"", line)
                if result != []:
                    (title,list) = result[0]
                    sensor_list = list.split(",")
                    print __name__, ": Report host list:", sensor_list
                    FrameworkBaseRequestHandler.__nessus.generate_report(title, sensor_list)

            if action == "scan":
                sensor_list = []
                hosts_list = []
                hostgroups_list = []
                nets_list = []
                netgroups_list = []
                
                result = re.findall("target_type=\"([a-z]+)\"" , line)
                if result != []:
                    target_type = result[0]
                    # need to be modifified to support schedule for host, hostgropup, etc..
                    if target_type == "schedule":
                        result = re.findall("id=\"([0-9]*)\"", line)
                        if result != []:
                            id = result[0]
                            print __name__, ": Got schedule request."
                            FrameworkBaseRequestHandler.__nessus.load_shedule(id)
                    elif target_type == "sensors":
                        result = re.findall("list=\"([0-9\.,]*)\"", line)
                        if result != []:
                            list = result[0]
                            if not list == "":
                                sensor_list = list.split(",")
                        print __name__, ": Sensor_list:", sensor_list
                        FrameworkBaseRequestHandler.__nessus.set_scan_type("sensor")
                        FrameworkBaseRequestHandler.__nessus.load_sensors(sensor_list)
                    elif target_type == "hosts":
                        result = re.findall("netgroups=\"([0-9a-zA-Z\._,]*)\" nets=\"([0-9a-zA-Z\._,]*)\" hostgroups=\"([0-9a-zA-Z\._,]*)\" hosts=\"([0-9\.,]*)\"", line)
                        if result != []:
                            (netgroups,nets,hostgroups,hosts) = result[0]
                            nets_list = FrameworkBaseRequestHandler.__nessus.get_nets(netgroups,nets)
                            if not hostgroups == "":
                                hostgroups_list = hostgroups.split(",")
                            if not hosts == "":
                                hosts_list = hosts.split(",")
                        print __name__, ": Net_list:", nets_list
                        print __name__, ": Host_list:", hosts_list
                        print __name__, ": Hostgroup_list:", hostgroups_list
                        FrameworkBaseRequestHandler.__nessus.set_scan_type("hosts")
                        FrameworkBaseRequestHandler.__nessus.load_hosts(hostgroups_list, nets_list, hosts_list)

                    if FrameworkBaseRequestHandler.__nessus.status() == 0:
                        try:
                            FrameworkBaseRequestHandler.__nessus.start()
                            self.wfile.write("ok\n")
                        except AssertionError: 
                            FrameworkBaseRequestHandler.__nessus.run()
                            self.wfile.write("ok\n")
                    elif FrameworkBaseRequestHandler.__nessus.status() > 0 :
                        print __name__, ": scan already started, status:", FrameworkBaseRequestHandler.__nessus.status()
                        self.wfile.write("Scan already started, status: " + str(FrameworkBaseRequestHandler.__nessus.status()) + "%\n")
            
            if action == "status":
                print __name__, ": status:", FrameworkBaseRequestHandler.__nessus.status()
                self.wfile.write(str(FrameworkBaseRequestHandler.__nessus.status()) + "\n")
            if action == "reset" and FrameworkBaseRequestHandler.__nessus.status() == -1:
                print __name__, ": Resetting status"
                self.wfile.write("Resetting status\n")
                FrameworkBaseRequestHandler.__nessus.reset_status()
            if FrameworkBaseRequestHandler.__nessus.status() == -1:
                print __name__, ": Previous scan aborted raising errors, please check your logfile. Error: " + \
                FrameworkBaseRequestHandler.__nessus.get_error()
                self.wfile.write("Previous scan aborted raising errors, please check your logfile. Error: " + \
                str(FrameworkBaseRequestHandler.__nessus.get_error()) + "\n")
            if action == "archive":
                result = re.findall("report=\"([a-z0-9.]+)\"", line)
                if result != []:
                    report = result[0]
                    print __name__, ": Got archive request for", report
                    FrameworkBaseRequestHandler.__nessus.archive(report)
                    self.wfile.write("nessus archive ack " + param + "\n")
            if action == "delete":
                result = re.findall("report=\"([a-z0-9.]+)\"", line)
                if result != []:
                    report = result[0]
                print __name__, ": Got delete request for", report
                if report.endswith(".report"):
                    FrameworkBaseRequestHandler.__nessus.delete(report, True)
                else:
                    FrameworkBaseRequestHandler.__nessus.delete(report, False)
                self.wfile.write("nessus delete ack " + report + "\n")
            if action == "restore":
                result = re.findall("report=\"([a-z0-9.]+)\"", line)
                if result != []:
                    report = result[0]
                print __name__, ": Got restore request for", report
                FrameworkBaseRequestHandler.__nessus.restore(report)
                self.wfile.write("nessus restore ack " + report + "\n")
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
