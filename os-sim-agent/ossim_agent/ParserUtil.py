# Set of functions to be used in plugin configuration

import socket, re, time

# translate a host name to IPv4 address
def resolv(host):

    try:
        addr = socket.gethostbyname(host)
    except socket.gaierror:
        return host

    return addr


def snort_id(id):
    return str(1000 + int(id))


def normalize_protocol(protocol):

    # TODO: fill table with /etc/protocols
    proto_table = {
        '1':    'icmp',
        '6':    'tcp',
        '17':   'udp',
    }

    if proto_table.has_key(str(protocol)):
        return proto_table[str(protocol)]
    else:
        return str(protocol).lower()


def normalize_date(string):

    # syslog
    # Oct 27 10:50:46
    pattern = "(\w+)\s+(\d+)\s+(\d\d):(\d\d):(\d\d)"
    result = re.findall(str(pattern), string)
    if result != []:
        (monthmmm, day, hour, minute, second) = result[0]
        year = time.strftime('%Y', time.localtime(time.time()))
        datestring = "%s %s %s %s %s %s" % \
            (year, monthmmm, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
        return date

    # Snare
    # Sun Jan 28 15: 15:32 2007
    pattern = "\S+\s+(\S+)\s+(\d+)\s+(\d\d):(\d\d):(\d\d)\s+(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (monthmmm, day, hour, minute, second, year) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, monthmmm, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
        return date


    # snort
    # 11/08-19:19:06
    pattern = "(\d+)/(\d+)(/?(\d\d))?-(\d\d:\d\d:\d\d)"
    result = re.findall(str(pattern), string)
    if result != []:
        (month, day, placeholder, year, date) = result[0]
        if not year:
            year = time.strftime('%Y', time.localtime(time.time()))
        date = year + '-' + month + '-' + day + ' ' + date
        return date

    # arpwatch
    # Monday, March 15, 2004 15:39:19 +0000
    pattern = "(\w+), (\w+) (\d{1,2}), (\d{4}) (\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (dayw, month, day, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %B %d %H %M %S"))
        return date

    # heartbeat
    # 2006/10/19_11:40:05
    pattern = "(\d+)/(\d+)/(\d+)_(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (year, month, day, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
        return date

    # netgear
    # 11/03/2004 19:45:46
    pattern = "(\d+)/(\d+)/(\d+)\s(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (day, month, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
        return date


    # rrd, nagios
    # 1162540224
    pattern = "^(\d+)$"
    result = re.findall(str(pattern), string)
    if result != []:
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.localtime(float(string)))
        return date

    # apache
    # 29/Jan/2007:17:02:20
    pattern = "(\d\d)\/(\w\w\w)\/(\d\d\d\d):(\d\d):(\d\d):(\d\d)"
    result = re.findall(str(pattern), string)
    if result != []:
        (day, month, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
        return date

    return string


def upper(string):
    return string.upper()

