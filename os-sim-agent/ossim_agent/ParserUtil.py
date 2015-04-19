# Set of functions to be used in plugin configuration

import socket, re, time, md5

# translate a host name to IPv4 address
def resolv(host):

    try:
        addr = socket.gethostbyname(host)
    except socket.gaierror:
        return host

    return addr

def md5sum(datastring):
    return md5.new(datastring).hexdigest();

def snort_id(id):
    return str(1000 + int(id))


def normalize_protocol(protocol):

    proto_table = {
        '1':    'icmp',
        '6':    'tcp',
        '17':   'udp',
    }

#
#   fill protocols table reading /etc/protocols
#
#    try:
#        fd = open('/etc/protocols')
#    except IOError:
#        pass
#    else:
#        pattern = re.compile("(\w+)\s+(\d+)\s+\w+")
#        for line in fd.readlines():
#            result = pattern.search(line)
#            if result:
#                proto_name   = result.groups()[0]
#                proto_number = result.groups()[1]
#                if not proto_table.has_key(proto_number):
#                    proto_table[proto_number] = proto_name
#        fd.close()
#

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
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
            return date
        except ValueError:
            pass
    
    # syslog-ng date
    # Oct 27 2007 10:50:46
    pattern = "(\w+)\s+(\d+)\s+(\d\d\d\d)\s+(\d\d):(\d\d):(\d\d)"
    result = re.findall(str(pattern), string)
    if result != []:
        (monthmmm, day, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, monthmmm, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
            return date
        except ValueError:
            pass


    # Snare
    # Sun Jan 28 15: 15:32 2007
    pattern = "\S+\s+(\S+)\s+(\d+)\s+(\d\d):(\d\d):(\d\d)\s+(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (monthmmm, day, hour, minute, second, year) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, monthmmm, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
            return date
        except ValueError:
            pass




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
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %B %d %H %M %S"))
            return date
        except ValueError:
            pass


    # heartbeat
    # 2006/10/19_11:40:05
    pattern = "(\d+)/(\d+)/(\d+)_(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (year, month, day, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
            return date
        except ValueError:
            pass



    # netgear
    # 11/03/2004 19:45:46
    pattern = "(\d+)/(\d+)/(\d{4})\s(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (day, month, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
            return date
        except ValueError:
            pass

    # tarantella
    # 2007/10/18 14:38:03

    pattern = "(\d+)/(\d+)/(\d+)\s(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (year, month, day, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
            return date
        except ValueError:
            pass

    # OSSEC
    # 2007 Nov 17 06:26:18
    
    pattern = "(\d+)\s+(\w+)\s+(\d+)\s+(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (year, month, day, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%Y %m %d %H %M %S"))
            return date
        except ValueError:
            pass

    # ibm applications
    # 11/03/07 19:22:22
    pattern = "(\d+)/(\d+)/(\d{2})\s(\d+):(\d+):(\d+)"
    result = re.findall(str(pattern), string)
    if result != []:
        (day, month, year, hour, minute, second) = result[0]
        datestring = "%s %s %s %s %s %s" % \
            (year, month, day, hour, minute, second)
        date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                            "%y %m %d %H %M %S"))
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
        try:
            date = time.strftime('%Y-%m-%d %H:%M:%S',
                             time.strptime(datestring,
                                           "%Y %b %d %H %M %S"))
            return date
        except ValueError:
            pass



    return string


def upper(string):
    return string.upper()


def hextoint(string):
    try:
        return int(string, 16)
    except ValueError:
        pass
        
def intrushield_sid(mcafee_sid,mcafee_name):
    # All McAfee Intrushield id are divisible by 256, and this length doesn't fit in OSSIM's table
    mcafee_sid = hextoint(mcafee_sid)/256
    mcafee_name = mcafee_name.replace('-',':')

    # Calculate hash based in event name
    mcafee_subsid=abs(mcafee_name.__hash__())

    # Ugly method to avoid duplicated sids
    mcafee_hash2 = 0
    for i in range(0,len(mcafee_name)):
        mcafee_hash2 = mcafee_hash2 + ord( mcafee_name[i] )

    ossim_sid = int(str(mcafee_hash2)[-1:]+str(int(str(mcafee_subsid)[-7:])+mcafee_sid))

    return ossim_sid

