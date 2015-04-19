import urllib2, os, time, sys, mutex
import util

m = mutex.mutex()

#
# get host info from ntop dump data
#
# FIXME!
# something is very bad with urllib :'(
#
def get_hostinfo2(url):

    if not os.path.isdir("/var/lib/ossim"):
        os.mkdir("/var/lib/ossim", 0755)

    try:
        util.debug(__name__, "Reading ntop dump data...", "<-", "YELLOW")
        fd = urllib2.urlopen(url)
    except urllib2.URLError, e:
        util.debug (__name__, e, '!!', 'RED');
        return None

    fhost = open('/var/lib/ossim/ossim_ntop_hostinfo.py', 'w')
    for line in fd.readlines():
        print line
        fhost.write(line)

    fhost.close()
    fd.close()

def get_hostinfo(url):

    if not os.path.isdir("/var/lib/ossim"):
        os.mkdir("/var/lib/ossim", 0755)

    os.system("/usr/bin/wget -O /var/lib/ossim/ossim_ntop_hostinfo.py %s 2> /dev/null" % (url))


def get_value(rule, url):
   
    (pattern, category, proto) = sid2pattern(int(rule["plugin_sid"]))

    #
    # Try to read dump data from disc. 
    #
    #   if modified_time(file) + 1min > local_time:
    #       update disc dump data
    #   else:
    #       read actual disc dump data
   
    try:
        if os.path.getmtime('/var/lib/ossim/ossim_ntop_hostinfo.py') + 60 <= int(time.time()) \
           and m.test() == 0:
            m.lock(get_hostinfo, url)
    except OSError:
        if m.test() == 0:
            m.lock(get_hostinfo, url)

    m.unlock()

    try:
        if sys.path.count("/var/lib/ossim/") == 0:
            sys.path.append("/var/lib/ossim/")
        import ossim_ntop_hostinfo

        if category != '':
            return ossim_ntop_hostinfo.ntopDict[rule["from"]][proto][category][pattern]
        elif proto != '':
            return ossim_ntop_hostinfo.ntopDict[rule["from"]][proto][pattern]
        else:
            return ossim_ntop_hostinfo.ntopDict[rule["from"]][pattern]

    except SyntaxError, e:
        util.debug(__name__, e, '!!', 'RED')
        os.remove('/var/lib/ossim/ossim_ntop_hostinfo.py')
        return None

    except KeyError:
        return None



def sid2pattern(sid):

    """Returns a tuple with the search pattern and the category"""
    
    if sid == 1:
        (pattern, category, proto) = ('firstSeen', '', '')
    elif sid == 2:
        (pattern, category, proto) = ('lastSeen', '', '')
    elif sid == 3:
        (pattern, category, proto) = ('minTTL', '', '')
    elif sid == 4:
        (pattern, category, proto) = ('maxTTL', '', '')
    elif sid == 5:
        (pattern, category, proto) = ('pktSent', '', '')
    elif sid == 6:
        (pattern, category, proto) = ('pktRcvd', '', '')
    elif sid == 7:
        (pattern, category, proto) = ('bytesSent', '', '')
    elif sid == 8:
        (pattern, category, proto) = ('bytesRcvd', '', '')
    elif sid == 9:
        (pattern, category, proto) = ('pktDuplicatedAckSent', '', '')
    elif sid == 10:
        (pattern, category, proto) = ('pktDuplicatedAckRcvd', '', '')
    elif sid == 11:
        (pattern, category, proto) = ('pktBroadcastSent', '', '')
    elif sid == 12:
        (pattern, category, proto) = ('bytesMulticastSent', '', '')
    elif sid == 13:
        (pattern, category, proto) = ('pktMulticastSent', '', '')
    elif sid == 14:
        (pattern, category, proto) = ('bytesMulticastRcvd', '', '')
    elif sid == 15:
        (pattern, category, proto) = ('pktMulticastRcvd', '', '')
    elif sid == 16:
        (pattern, category, proto) = ('bytesSent', '', '')
    elif sid == 17:
        (pattern, category, proto) = ('bytesSentLoc', '', '')
    elif sid == 18:
        (pattern, category, proto) = ('bytesSentRem', '', '')
    elif sid == 19:
        (pattern, category, proto) = ('bytesRcvd', '', '')
    elif sid == 20:
        (pattern, category, proto) = ('bytesRcvdLoc', '', '')
    elif sid == 21:
        (pattern, category, proto) = ('bytesRcvdFromRem', '', '')
    elif sid == 22:
        (pattern, category, proto) = ('actualRcvdThpt', '', '')
    elif sid == 23:
        (pattern, category, proto) = ('lastHourRcvdThpt', '', '')
    elif sid == 24:
        (pattern, category, proto) = ('averageRcvdThpt', '', '')
    elif sid == 25:
        (pattern, category, proto) = ('peakRcvdThpt', '', '')
    elif sid == 26:
        (pattern, category, proto) = ('actualSentThpt', '', '')
    elif sid == 27:
        (pattern, category, proto) = ('lastHourSentThpt', '', '')
    elif sid == 28:
        (pattern, category, proto) = ('averageSentThpt', '', '')
    elif sid == 29:
        (pattern, category, proto) = ('peakSentThpt', '', '')
    elif sid == 30:
        (pattern, category, proto) = ('actualTThpt', '', '')
    elif sid == 31:
        (pattern, category, proto) = ('averageTThpt', '', '')
    elif sid == 32:
        (pattern, category, proto) = ('peakTThpt', '', '')
    elif sid == 33:
        (pattern, category, proto) = ('actualRcvdPktThpt', '', '')
    elif sid == 34:
        (pattern, category, proto) = ('averageRcvdPktThpt', '', '')
    elif sid == 35:
        (pattern, category, proto) = ('peakRcvdPktThpt', '', '')
    elif sid == 36:
        (pattern, category, proto) = ('actualSentPktThpt', '', '')
    elif sid == 37:
        (pattern, category, proto) = ('averageSentPktThpt', '', '')
    elif sid == 38:
        (pattern, category, proto) = ('peakSentPktThpt', '', '')
    elif sid == 39:
        (pattern, category, proto) = ('actualTPktThpt', '', '')
    elif sid == 40:
        (pattern, category, proto) = ('averageTPktThpt', '', '')
    elif sid == 41:
        (pattern, category, proto) = ('peakTPktThpt', '', '')
    elif sid == 42:
        (pattern, category, proto) = ('ipBytesSent', '', '')
    elif sid == 43:
        (pattern, category, proto) = ('ipBytesRcvd', '', '')
    elif sid == 44:
        (pattern, category, proto) = ('tcpBytesSent', '', '')
    elif sid == 45:
        (pattern, category, proto) = ('tcpBytesRcvd', '', '')
    elif sid == 46:
        (pattern, category, proto) = ('udpBytesSent', '', '')
    elif sid == 47:
        (pattern, category, proto) = ('udpBytesRcvd', '', '')
    elif sid == 48:
        (pattern, category, proto) = ('icmpSent', '', '')
    elif sid == 49:
        (pattern, category, proto) = ('icmpRcvd', '', '')
    elif sid == 50:
        (pattern, category, proto) = ('tcpSentRem', '', '')
    elif sid == 51:
        (pattern, category, proto) = ('udpSentLoc', '', '')
    elif sid == 52:
        (pattern, category, proto) = ('udpSentRem', '', '')
    elif sid == 53:
        (pattern, category, proto) = ('ospfSent', '', '')
    elif sid == 54:
        (pattern, category, proto) = ('igmpSent', '', '')
    elif sid == 55:
        (pattern, category, proto) = ('tcpRcvdLoc', '', '')
    elif sid == 56:
        (pattern, category, proto) = ('tcpRcvdFromRem', '', '')
    elif sid == 57:
        (pattern, category, proto) = ('udpRcvdLoc', '', '')
    elif sid == 58:
        (pattern, category, proto) = ('udpRcvdFromRem', '', '')
    elif sid == 59:
        (pattern, category, proto) = ('ospfRcvd', '', '')
    elif sid == 60:
        (pattern, category, proto) = ('igmpRcvd', '', '')
    elif sid == 61:
        (pattern, category, proto) = ('tcpFragmentsSent', '', '')
    elif sid == 62:
        (pattern, category, proto) = ('tcpFragmentsRcvd', '', '')
    elif sid == 63:
        (pattern, category, proto) = ('udpFragmentsSent', '', '')
    elif sid == 64:
        (pattern, category, proto) = ('udpFragmentsRcvd', '', '')
    elif sid == 65:
        (pattern, category, proto) = ('icmpFragmentsSent', '', '')
    elif sid == 66:
        (pattern, category, proto) = ('icmpFragmentsRcvd', '', '')
    elif sid == 67:
        (pattern, category, proto) = ('stpSent', '', '')
    elif sid == 68:
        (pattern, category, proto) = ('stpRcvd', '', '')
    elif sid == 69:
        (pattern, category, proto) = ('ipxSent', '', '')
    elif sid == 70:
        (pattern, category, proto) = ('ipxRcvd', '', '')
    elif sid == 71:
        (pattern, category, proto) = ('osiSent', '', '')
    elif sid == 72:
        (pattern, category, proto) = ('osiRcvd', '', '')
    elif sid == 73:
        (pattern, category, proto) = ('dlcSent', '', '')
    elif sid == 74:
        (pattern, category, proto) = ('dlcRcvd', '', '')
    elif sid == 75:
        (pattern, category, proto) = ('arp_rarpSent', '', '')
    elif sid == 76:
        (pattern, category, proto) = ('arp_rarpRcvd', '', '')
    elif sid == 77:
        (pattern, category, proto) = ('arpReqPktsSent', '', '')
    elif sid == 78:
        (pattern, category, proto) = ('arpReplyPktsSent', '', '')
    elif sid == 79:
        (pattern, category, proto) = ('arpReplyPktsRcvd', '', '')
    elif sid == 80:
        (pattern, category, proto) = ('decnetSent', '', '')
    elif sid == 81:
        (pattern, category, proto) = ('decnetRcvd', '', '')
    elif sid == 82:
        (pattern, category, proto) = ('appletalkSent', '', '')
    elif sid == 83:
        (pattern, category, proto) = ('appletalkRcvd', '', '')
    elif sid == 84:
        (pattern, category, proto) = ('netbiosSent', '', '')
    elif sid == 85:
        (pattern, category, proto) = ('netbiosRcvd', '', '')
    elif sid == 86:
        (pattern, category, proto) = ('ipv6Sent', '', '')
    elif sid == 87:
        (pattern, category, proto) = ('ipv6Rcvd', '', '')
    elif sid == 88:
        (pattern, category, proto) = ('otherSent', '', '')
    elif sid == 89:
        (pattern, category, proto) = ('otherRcvd', '', '')
    elif sid == 90:
        (pattern, category, proto) = ('synPktsSent', '', '')
    elif sid == 91:
        (pattern, category, proto) = ('synPktsRcvd', '', '')
    elif sid == 92:
        (pattern, category, proto) = ('rstPktsSent', '', '')
    elif sid == 93:
        (pattern, category, proto) = ('rstPktsRcvd', '', '')
    elif sid == 94:
        (pattern, category, proto) = ('rstAckPktsSent', '', '')
    elif sid == 95:
        (pattern, category, proto) = ('rstAckPktsRcvd', '', '')
    elif sid == 96:
        (pattern, category, proto) = ('synFinPktsSent', '', '')
    elif sid == 97:
        (pattern, category, proto) = ('synFinPktsRcvd', '', '')
    elif sid == 98:
        (pattern, category, proto) = ('finPushUrgPktsSent', '', '')
    elif sid == 99:
        (pattern, category, proto) = ('finPushUrgPktsRcvd', '', '')
    elif sid == 100:
        (pattern, category, proto) = ('nullPktsSent', '', '')
    elif sid == 101:
        (pattern, category, proto) = ('nullPktsRcvd', '', '')
    elif sid == 102:
        (pattern, category, proto) = ('ackScanSent', '', '')
    elif sid == 103:
        (pattern, category, proto) = ('ackScanRcvd', '', '')
    elif sid == 104:
        (pattern, category, proto) = ('xmasScanSent', '', '')
    elif sid == 105:
        (pattern, category, proto) = ('xmasScanRcvd', '', '')
    elif sid == 106:
        (pattern, category, proto) = ('finScanSent', '', '')
    elif sid == 107:
        (pattern, category, proto) = ('finScanRcvd', '', '')
    elif sid == 108:
        (pattern, category, proto) = ('nullScanSent', '', '')
    elif sid == 109:
        (pattern, category, proto) = ('nullScanRcvd', '', '')
    elif sid == 110:
        (pattern, category, proto) = ('rejectedTCPConnSent', '', '')
    elif sid == 111:
        (pattern, category, proto) = ('rejectedTCPConnRcvd', '', '')
    elif sid == 112:
        (pattern, category, proto) = ('establishedTCPConnSent', '', '')
    elif sid == 113:
        (pattern, category, proto) = ('establishedTCPConnRcvd', '', '')
    elif sid == 114:
        (pattern, category, proto) = ('terminatedTCPConnServer', '', '')
    elif sid == 115:
        (pattern, category, proto) = ('terminatedTCPConnClient', '', '')
    elif sid == 116:
        (pattern, category, proto) = ('udpToClosedPortSent', '', '')
    elif sid == 117:
        (pattern, category, proto) = ('udpToClosedPortRcvd', '', '')
    elif sid == 118:
        (pattern, category, proto) = ('udpToDiagnosticPortSent', '', '')
    elif sid == 119:
        (pattern, category, proto) = ('udpToDiagnosticPortRcvd', '', '')
    elif sid == 120:
        (pattern, category, proto) = ('tcpToDiagnosticPortSent', '', '')
    elif sid == 121:
        (pattern, category, proto) = ('tcpToDiagnosticPortRcvd', '', '')
    elif sid == 122:
        (pattern, category, proto) = ('tinyFragmentSent', '', '')
    elif sid == 123:
        (pattern, category, proto) = ('tinyFragmentRcvd', '', '')
    elif sid == 124:
        (pattern, category, proto) = ('icmpFragmentSent', '', '')
    elif sid == 125:
        (pattern, category, proto) = ('icmpFragmentRcvd', '', '')
    elif sid == 126:
        (pattern, category, proto) = ('overlappingFragmentSent', '', '')
    elif sid == 127:
        (pattern, category, proto) = ('overlappingFragmentRcvd', '', '')
    elif sid == 128:
        (pattern, category, proto) = ('closedEmptyTCPConnSent', '', '')
    elif sid == 129:
        (pattern, category, proto) = ('closedEmptyTCPConnRcvd', '', '')
    elif sid == 130:
        (pattern, category, proto) = ('icmpPortUnreachSent', '', '')
    elif sid == 131:
        (pattern, category, proto) = ('icmpPortUnreachRcvd', '', '')
    elif sid == 132:
        (pattern, category, proto) = ('icmpHostNetUnreachSent', '', '')
    elif sid == 133:
        (pattern, category, proto) = ('icmpProtocolUnreachSent', '', '')
    elif sid == 134:
        (pattern, category, proto) = ('icmpProtocolUnreachRcvd', '', '')
    elif sid == 135:
        (pattern, category, proto) = ('icmpHostNetUnreachRcvd', '', '')
    elif sid == 136:
        (pattern, category, proto) = ('icmpAdminProhibitedSent', '', '')
    elif sid == 137:
        (pattern, category, proto) = ('icmpAdminProhibitedRcvd', '', '')
    elif sid == 138:
        (pattern, category, proto) = ('malformedPktsSent', '', '')
    elif sid == 139:
        (pattern, category, proto) = ('malformedPktsRcvd', '', '')

    # IP Specific
    elif sid == 140:
        (pattern, category, proto) = ('sentLoc', 'FTP', 'IP')
    elif sid == 141:
        (pattern, category, proto) = ('sentRem', 'FTP', 'IP')
    elif sid == 142:
        (pattern, category, proto) = ('rcvdLoc', 'FTP', 'IP')
    elif sid == 143:
        (pattern, category, proto) = ('rcvdFromRem', 'FTP', 'IP')
    elif sid == 144:
        (pattern, category, proto) = ('sentLoc', 'HTTP', 'IP')
    elif sid == 145:
        (pattern, category, proto) = ('sentRem', 'HTTP', 'IP')
    elif sid == 146:
        (pattern, category, proto) = ('rcvdLoc', 'HTTP', 'IP')
    elif sid == 147:
        (pattern, category, proto) = ('rcvdFromRem', 'HTTP', 'IP')
    elif sid == 148:
        (pattern, category, proto) = ('sentLoc', 'DNS', 'IP')
    elif sid == 149:
        (pattern, category, proto) = ('sentRem', 'DNS', 'IP')
    elif sid == 150:
        (pattern, category, proto) = ('rcvdLoc', 'DNS', 'IP')
    elif sid == 151:
        (pattern, category, proto) = ('rcvdFromRem', 'DNS', 'IP')
    elif sid == 152:
        (pattern, category, proto) = ('sentLoc', 'Telnet', 'IP')
    elif sid == 153:
        (pattern, category, proto) = ('sentRem', 'Telnet', 'IP')
    elif sid == 154:
        (pattern, category, proto) = ('rcvdLoc', 'Telnet', 'IP')
    elif sid == 155:
        (pattern, category, proto) = ('rcvdFromRem', 'Telnet', 'IP')
    elif sid == 156:
        (pattern, category, proto) = ('sentLoc', 'NBios-IP', 'IP')
    elif sid == 157:
        (pattern, category, proto) = ('sentRem', 'NBios-IP', 'IP')
    elif sid == 158:
        (pattern, category, proto) = ('rcvdLoc', 'NBios-IP', 'IP')
    elif sid == 159:
        (pattern, category, proto) = ('rcvdFromRem', 'NBios-IP', 'IP')
    elif sid == 160:
        (pattern, category, proto) = ('sentLoc', 'Mail', 'IP')
    elif sid == 161:
        (pattern, category, proto) = ('sentRem', 'Mail', 'IP')
    elif sid == 162:
        (pattern, category, proto) = ('rcvdLoc', 'Mail', 'IP')
    elif sid == 163:
        (pattern, category, proto) = ('rcvdFromRem', 'Mail', 'IP')
    elif sid == 164:
        (pattern, category, proto) = ('sentLoc', 'DHCP-BOOTP', 'IP')
    elif sid == 165:
        (pattern, category, proto) = ('sentRem', 'DHCP-BOOTP', 'IP')
    elif sid == 166:
        (pattern, category, proto) = ('rcvdLoc', 'DHCP-BOOTP', 'IP')
    elif sid == 167:
        (pattern, category, proto) = ('rcvdFromRem', 'DHCP-BOOTP', 'IP')
    elif sid == 168:
        (pattern, category, proto) = ('sentLoc', 'SNMP', 'IP')
    elif sid == 169:
        (pattern, category, proto) = ('sentRem', 'SNMP', 'IP')
    elif sid == 170:
        (pattern, category, proto) = ('rcvdLoc', 'SNMP', 'IP')
    elif sid == 171:
        (pattern, category, proto) = ('rcvdFromRem', 'SNMP', 'IP')
    elif sid == 172:
        (pattern, category, proto) = ('sentLoc', 'NNTP', 'IP')
    elif sid == 173:
        (pattern, category, proto) = ('sentRem', 'NNTP', 'IP')
    elif sid == 174:
        (pattern, category, proto) = ('rcvdLoc', 'NNTP', 'IP')
    elif sid == 175:
        (pattern, category, proto) = ('rcvdFromRem', 'NNTP', 'IP')
    elif sid == 176:
        (pattern, category, proto) = ('sentLoc', 'NFS', 'IP')
    elif sid == 177:
        (pattern, category, proto) = ('sentRem', 'NFS', 'IP')
    elif sid == 178:
        (pattern, category, proto) = ('rcvdLoc', 'NFS', 'IP')
    elif sid == 179:
        (pattern, category, proto) = ('rcvdFromRem', 'NFS', 'IP')
    elif sid == 180:
        (pattern, category, proto) = ('sentLoc', 'X11', 'IP')
    elif sid == 181:
        (pattern, category, proto) = ('sentRem', 'X11', 'IP')
    elif sid == 182:
        (pattern, category, proto) = ('rcvdLoc', 'X11', 'IP')
    elif sid == 183:
        (pattern, category, proto) = ('rcvdFromRem', 'X11', 'IP')
    elif sid == 184:
        (pattern, category, proto) = ('sentLoc', 'SSH', 'IP')
    elif sid == 185:
        (pattern, category, proto) = ('sentRem', 'SSH', 'IP')
    elif sid == 186:
        (pattern, category, proto) = ('rcvdLoc', 'SSH', 'IP')
    elif sid == 187:
        (pattern, category, proto) = ('rcvdFromRem', 'SSH', 'IP')
    elif sid == 188:
        (pattern, category, proto) = ('sentLoc', 'Gnutella', 'IP')
    elif sid == 189:
        (pattern, category, proto) = ('sentRem', 'Gnutella', 'IP')
    elif sid == 190:
        (pattern, category, proto) = ('rcvdLoc', 'Gnutella', 'IP')
    elif sid == 191:
        (pattern, category, proto) = ('rcvdFromRem', 'Gnutella', 'IP')
    elif sid == 192:
        (pattern, category, proto) = ('sentLoc', 'Kazaa', 'IP')
    elif sid == 193:
        (pattern, category, proto) = ('sentRem', 'Kazaa', 'IP')
    elif sid == 194:
        (pattern, category, proto) = ('rcvdLoc', 'Kazaa', 'IP')
    elif sid == 195:
        (pattern, category, proto) = ('rcvdFromRem', 'Kazaa', 'IP')
    elif sid == 196:
        (pattern, category, proto) = ('sentLoc', 'WinMX', 'IP')
    elif sid == 197:
        (pattern, category, proto) = ('sentRem', 'WinMX', 'IP')
    elif sid == 198:
        (pattern, category, proto) = ('rcvdLoc', 'WinMX', 'IP')
    elif sid == 199:
        (pattern, category, proto) = ('rcvdFromRem', 'WinMX', 'IP')
    elif sid == 200:
        (pattern, category, proto) = ('sentLoc', 'DirectConnect', 'IP')
    elif sid == 201:
        (pattern, category, proto) = ('sentRem', 'DirectConnect', 'IP')
    elif sid == 202:
        (pattern, category, proto) = ('rcvdLoc', 'DirectConnect', 'IP')
    elif sid == 203:
        (pattern, category, proto) = ('rcvdFromRem', 'DirectConnect', 'IP')
    elif sid == 204:
        (pattern, category, proto) = ('sentLoc', 'eDonkey', 'IP')
    elif sid == 205:
        (pattern, category, proto) = ('sentRem', 'eDonkey', 'IP')
    elif sid == 206:
        (pattern, category, proto) = ('rcvdLoc', 'eDonkey', 'IP')
    elif sid == 207:
        (pattern, category, proto) = ('rcvdFromRem', 'eDonkey', 'IP')
    elif sid == 208:
        (pattern, category, proto) = ('sentLoc', 'Messenger', 'IP')
    elif sid == 209:
        (pattern, category, proto) = ('sentRem', 'Messenger', 'IP')
    elif sid == 210:
        (pattern, category, proto) = ('rcvdLoc', 'Messenger', 'IP')
    elif sid == 211:
        (pattern, category, proto) = ('rcvdFromRem', 'Messenger', 'IP')
    elif sid == 212:
        (pattern, category, proto) = ('sentLoc', 'PROXY', 'IP')
    elif sid == 213:
        (pattern, category, proto) = ('sentRem', 'PROXY', 'IP')
    elif sid == 214:
        (pattern, category, proto) = ('rcvdLoc', 'PROXY', 'IP')
    elif sid == 215:
        (pattern, category, proto) = ('rcvdFromRem', 'PROXY', 'IP')
    elif sid == 216:
        (pattern, category, proto) = ('sentLoc', 'NEWS', 'IP')
    elif sid == 217:
        (pattern, category, proto) = ('sentRem', 'NEWS', 'IP')
    elif sid == 218:
        (pattern, category, proto) = ('rcvdLoc', 'NEWS', 'IP')
    elif sid == 219:
        (pattern, category, proto) = ('rcvdFromRem', 'NEWS', 'IP')

    # ICMP
    elif sid == 220:
        (pattern, category, proto) = ('SENT_ECHO', '', 'ICMP')
    elif sid == 221:
        (pattern, category, proto) = ('SENT_ECHOREPLY', '', 'ICMP')
    elif sid == 222:
        (pattern, category, proto) = ('SENT_UNREACH', '', 'ICMP')
    elif sid == 223:
        (pattern, category, proto) = ('SENT_ROUTERADVERT', '', 'ICMP')
    elif sid == 224:
        (pattern, category, proto) = ('SENT_TMXCEED', '', 'ICMP')
    elif sid == 225:
        (pattern, category, proto) = ('SENT_PARAMPROB', '', 'ICMP')
    elif sid == 226:
        (pattern, category, proto) = ('SENT_MASKREPLY', '', 'ICMP')
    elif sid == 227:
        (pattern, category, proto) = ('SENT_MASKREQ', '', 'ICMP')
    elif sid == 228:
        (pattern, category, proto) = ('SENT_INFO_REQUEST', '', 'ICMP')
    elif sid == 229:
        (pattern, category, proto) = ('SENT_INFO_REPLY', '', 'ICMP')
    elif sid == 230:
        (pattern, category, proto) = ('SENT_TIMESTAMP', '', 'ICMP')
    elif sid == 231:
        (pattern, category, proto) = ('SENT_TIMESTAMPREPLY', '', 'ICMP')
    elif sid == 232:
        (pattern, category, proto) = ('SENT_SOURCE_QUENCH', '', 'ICMP')
    elif sid == 233:
        (pattern, category, proto) = ('RCVD_ECHO', '', 'ICMP')
    elif sid == 234:
        (pattern, category, proto) = ('RCVD_ECHOREPLY', '', 'ICMP')
    elif sid == 235:
        (pattern, category, proto) = ('RCVD_UNREACH', '', 'ICMP')
    elif sid == 236:
        (pattern, category, proto) = ('RCVD_ROUTERADVERT', '', 'ICMP')
    elif sid == 237:
        (pattern, category, proto) = ('RCVD_TMXCEED', '', 'ICMP')
    elif sid == 238:
        (pattern, category, proto) = ('RCVD_PARAMPROB', '', 'ICMP')
    elif sid == 239:
        (pattern, category, proto) = ('RCVD_MASKREPLY', '', 'ICMP')
    elif sid == 240:
        (pattern, category, proto) = ('RCVD_MASKREQ', '', 'ICMP')
    elif sid == 241:
        (pattern, category, proto) = ('RCVD_INFO_REQUEST', '', 'ICMP')
    elif sid == 242:
        (pattern, category, proto) = ('RCVD_INFO_REPLY', '', 'ICMP')
    elif sid == 243:
        (pattern, category, proto) = ('RCVD_TIMESTAMP', '', 'ICMP')
    elif sid == 244:
        (pattern, category, proto) = ('RCVD_TIMESTAMPREPLY', '', 'ICMP')
    elif sid == 245:
        (pattern, category, proto) = ('RCVD_SOURCE_QUENCH', '', 'ICMP')
    else:
        util.debug(__name__, "%d: Bad plugin_sid" % (sid), "**", 'RED')
        (pattern, category, proto) = ('Unknown', 'Unknown', 'Unknown')
        
    return (pattern, category, proto)


