import urllib2
import HTMLParser
import sys
import socket
import re

import util


def tobytes(data):

    data = data.replace('&nbsp;', ' ')
    if data.endswith('KB'):
        data = float(data.split(' ', 1)[0]) * 1024
    elif data.endswith('MB'):
        data = float(data.split(' ', 1)[0]) * 1024 * 1024
    elif data.endswith('GB'):
        data = float(data.split(' ', 1)[0]) * 1024 * 1024 * 1204
    
    return str(data)


def tosecs(data):

    # example: 27 sec
    if data.endswith('sec'):
        data = data.split(' ', 1)[0]

    # examples: 1 day  1:52:27
    #           2 days 4:56:23
    elif data.__contains__('day'):
        result = re.findall('(\d+) days? (\S+)', data)
        (days, time) = result[0]
        seconds = int(days) * 86400

        dt = time.split(':')
        seconds += int(dt.pop())
        try:
            seconds += seconds + int(dt.pop()) * 60
            seconds += seconds + int(dt.pop()) * 3600
        except IndexError:
            pass
        data = str(seconds)

    # example: 1:52:27
    else:
        dt = data.split(':')
        seconds = int(dt.pop())
        try:
            seconds += seconds + int(dt.pop()) * 60
            seconds += seconds + int(dt.pop()) * 3600
        except IndexError:
            pass
        data = str(seconds)

    return data




def get_value(rule, url):

    try:
        fd = urllib2.urlopen(url)
    except urllib2.URLError, e:
        util.debug (__name__, e, '!!', 'RED');
        sys.exit()
        
    parse = 0
    session = {}

    while 1:

        line = fd.readline()
        if not line: break

        #
        # search for Sessions section
        #
        if line.__contains__('Active TCP Sessions'):
            parse = 1

        #
        # </table> found, session section finished
        #
        if line.__contains__('</TABLE>') and parse == 1:
            parse = 0
            break

            
        if parse == 1:
            
            #
            # search for client and server ip
            #
            pattern = '<a.*?>(.*?)</a>'
            result = re.findall(pattern, line)
            if result != []: 
                if not session.has_key("client"):
                    try:
                        session["client"] = socket.gethostbyname(result[0])
                    except socket.error:
                        session["client"] = result[0]
                else:
                    try:
                        session["server"] = socket.gethostbyname(result[0])
                    except socket.error:
                        session["server"] = result[0]

            #
            # search for client and server port
            #
            pattern = '^:(.*?)</TD>'
            result = re.findall(pattern, line)
            if result != []: 
                if not session.has_key("client_port"):
                    try:
                        session["client_port"] = \
                            socket.getservbyname(result[0], 'tcp')
                    except socket.error:
                        session["client_port"] = result[0]
                else:
                    try:
                        session["server_port"] = \
                            socket.getservbyname(result[0], 'tcp')
                    except socket.error:
                        session["client_port"] = result[0]
            
            #
            # search for session data
            #
            pattern = '<TD.*?>(.*?)</TD>'
            result = re.findall(pattern, line)
            if result != []: 
                
                (session["data_sent"], session["data_rcvd"], \
                 session["active_since"], session["last_seen"], \
                 session["duration"], session["active"], \
                 session["latency"]) = result

                session["data_sent"] = tobytes(session["data_sent"])
                session["data_rcvd"] = tobytes(session["data_rcvd"])
                session["duration"]  = tosecs(session["duration"])
                session["active"]    = tosecs(session["active"])


                #
                # all data stored, check for alert
                #
                if session["client"] == rule["from"] and \
                   session["server"] == rule["to"] and \
                   ((str(session["client_port"]) == \
                     str(rule["port_from"]))) and \
                   ((str(session["server_port"]) == \
                     str(rule["port_to"]))):

                    if int(rule["plugin_sid"]) == 246:
                        return int(session["data_sent"])
                    elif int(rule["plugin_sid"]) == 247:
                        return int(session["data_recv"])
                    elif int(rule["plugin_sid"]) == 248:
                        return int(session["duration"])

                # no alert, check next iteration
                session = {}
   
    # no session opened
    return None

