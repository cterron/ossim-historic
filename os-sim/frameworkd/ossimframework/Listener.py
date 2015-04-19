import threading, socket, sys, SocketServer

import Const

class FrameworkBaseRequestHandler(SocketServer.StreamRequestHandler):

    def handle(self):

        print __name__, ":", "Request from", self.client_address
        # print self.request, self.server

        #
        # echo server :)
        #
        for line in self.rfile.readlines(): print line
        self.wfile.writelines(self.rfile.readlines())


#
#class FrameworkTCPServer(SocketServer.TCPServer):
#
#    def __init__(self, serverAddress, requestHandler):
#        SocketServer.TCPServer.__init__(self, serverAddress, requestHandler)
#


class Listener(threading.Thread):

    def __init__(self):

        self.__server = None
        threading.Thread.__init__(self)


    def run(self):

        try:
            serverAddress = (socket.gethostbyname(socket.gethostname()),
                             int(Const.LISTENER_PORT))
            self.__server = SocketServer.TCPServer(serverAddress,
                                                   FrameworkBaseRequestHandler)
        except socket.error, e:
            print __name__, ":", e
            sys.exit()

        self.__server.serve_forever()


