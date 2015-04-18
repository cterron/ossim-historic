import xml.sax
import time
import sys
import urllib2

import Monitor
import util

class MonitorNtop(Monitor.Monitor):

    plugin_id = '2005'

    def run(self):
    
        util.debug (__name__, "plugin started", '--')
        rule = self.split_data(self.data)
        util.debug (__name__, "request received... (%s)" % str(rule), 
                    '<=', 'GREEN')
        
        if rule is not None:
   
            # absolute
            if rule["absolute"] == 'true' or \
               rule["absolute"] == 'yes' or \
               rule["absolute"] == '1':
                self.__evaluate(rule = rule, absolute = 'true')

            # relative
            else:
                self.__evaluate(rule = rule)


    def __get_value(self, rule):
       
        # search url to connect ntop
        url = 'http://' + \
              self.plugins[MonitorNtop.plugin_id]['location'] + \
              '/dumpData.html?language=xml'

        (pattern, category) = self.sid2pattern(int(rule["plugin_sid"]))
        ntopParser = xml.sax.make_parser()
        ntopHandler = NtopHandler(pattern, category)
        ntopParser.setContentHandler(ntopHandler)
        try:
            ntopParser.parse(xml.sax.saxutils.prepare_input_source(url))
        except urllib2.URLError:
            util.debug (__name__, "Error: Ntop is not running!",
                        '!!', 'RED')
            return None
        except xml.sax.SAXParseException, e:
            util.debug (__name__, "%s" % (e), "!!", "RED")
            return sys.exit()
        
        try:
            return NtopHandler.ntop_data[rule["to"]]
        except KeyError:
            return None
    

    def __evaluate(self, rule, absolute = ""):

        if absolute == 'true':
            vfirst = 0
        else:
            vfirst = int(self.__get_value(rule))
            if vfirst is None: vfirst = 0

        pfreq = int(self.plugins[MonitorNtop.plugin_id]['frequency'])
        f = 0

        while 1:

            if rule["interval"] != '':

                #  calculate time to sleep
                if int(rule["interval"]) < pfreq:
                    util.debug (__name__, 
                                "waitting %d secs..." % int(rule["interval"]),
                                '**')
                    time.sleep(float(rule["interval"]))
                else:
                    if int(rule["interval"]) < f + pfreq:
                        util.debug (__name__,
                            "waitting %d secs..." % (int(rule["interval"])-f),
                            '**')
                        time.sleep(int(rule["interval"]) - f)
                    else:
                        util.debug (__name__, "waitting %d secs..." % pfreq,
                                    '**')
                        time.sleep(pfreq)


            util.debug (__name__, "getting ntop value...", '<=')
            vlast = self.__get_value(rule)
            if vlast is None:
                util.debug (__name__, "no data for %s" % rule["to"],
                            '!!', 'YELLOW')

            if ((rule["condition"] == 'eq') and \
                 (vlast == vfirst + int(rule["value"])) or \
                (rule["condition"] == 'ne') and \
                 (vlast != vfirst + int(rule["value"])) or \
                (rule["condition"] == 'gt') and \
                 (vlast > vfirst + int(rule["value"])) or \
                (rule["condition"] == 'ge') and \
                 (vlast >= vfirst + int(rule["value"])) or \
                (rule["condition"] == 'le') and \
                 (vlast <= vfirst + int(rule["value"])) or \
                (rule["condition"] == 'lt') and \
                 (vlast < vfirst + int(rule["value"]))):

                sensor = self.plugins[MonitorNtop.plugin_id]['sensor']
                interface = self.plugins[MonitorNtop.plugin_id]['interface']
                date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                     time.localtime(time.time()))

                self.agent.sendMessage(type         = 'monitor', 
                                       date         = date, 
                                       sensor       = sensor, 
                                       interface    = interface,
                                       plugin_id    = rule["plugin_id"], 
                                       plugin_sid   = rule["plugin_sid"],
                                       priority     = '', 
                                       protocol     = 'tcp', 
                                       src_ip       = rule["from"],
                                       src_port     = rule["port_from"], 
                                       dst_ip       = rule["to"], 
                                       dst_port     = rule["port_to"],
                                       condition    = rule["condition"],
                                       value        = rule["value"])
                
                util.debug (__name__, 'plugin finished', '--')
                break # alert sent, finished
                
            else:
                util.debug (__name__, 'No alert', '--', 'GREEN')
            
            f += pfreq
            if f >= int(rule["interval"]):  # finish if interval exceded
                util.debug (__name__, 'plugin finished', '--')
                break
 

    def sid2pattern(self, sid):
        """Returns a tuple with the search pattern and the category"""
        
        if sid == 1:
            (pattern, category) = ('firstSeen', '')
        elif sid == 2:
            (pattern, category) = ('lastSeen', '')
        elif sid == 3:
            (pattern, category) = ('minTTL', '')
        elif sid == 4:
            (pattern, category) = ('maxTTL', '')
        elif sid == 5:
            (pattern, category) = ('pktSent', '')
        elif sid == 6:
            (pattern, category) = ('pktRcvd', '')
        elif sid == 7:
            (pattern, category) = ('bytesSent', '')
        elif sid == 8:
            (pattern, category) = ('bytesRcvd', '')
        elif sid == 9:
            (pattern, category) = ('pktDuplicatedAckSent', '')
        elif sid == 10:
            (pattern, category) = ('pktDuplicatedAckRcvd', '')
        elif sid == 11:
            (pattern, category) = ('pktBroadcastSent', '')
        elif sid == 12:
            (pattern, category) = ('bytesMulticastSent', '')
        elif sid == 13:
            (pattern, category) = ('pktMulticastSent', '')
        elif sid == 14:
            (pattern, category) = ('bytesMulticastRcvd', '')
        elif sid == 15:
            (pattern, category) = ('pktMulticastRcvd', '')
        elif sid == 16:
            (pattern, category) = ('bytesSent', '')
        elif sid == 17:
            (pattern, category) = ('bytesSentLoc', '')
        elif sid == 18:
            (pattern, category) = ('bytesSentRem', '')
        elif sid == 19:
            (pattern, category) = ('bytesRcvd', '')
        elif sid == 20:
            (pattern, category) = ('bytesRcvdLoc', '')
        elif sid == 21:
            (pattern, category) = ('bytesRcvdFromRem', '')
        elif sid == 22:
            (pattern, category) = ('actualRcvdThpt', '')
        elif sid == 23:
            (pattern, category) = ('lastHourRcvdThpt', '')
        elif sid == 24:
            (pattern, category) = ('averageRcvdThpt', '')
        elif sid == 25:
            (pattern, category) = ('peakRcvdThpt', '')
        elif sid == 26:
            (pattern, category) = ('actualSentThpt', '')
        elif sid == 27:
            (pattern, category) = ('lastHourSentThpt', '')
        elif sid == 28:
            (pattern, category) = ('averageSentThpt', '')
        elif sid == 29:
            (pattern, category) = ('peakSentThpt', '')
        elif sid == 30:
            (pattern, category) = ('actualTThpt', '')
        elif sid == 31:
            (pattern, category) = ('averageTThpt', '')
        elif sid == 32:
            (pattern, category) = ('peakTThpt', '')
        elif sid == 33:
            (pattern, category) = ('actualRcvdPktThpt', '')
        elif sid == 34:
            (pattern, category) = ('averageRcvdPktThpt', '')
        elif sid == 35:
            (pattern, category) = ('peakRcvdPktThpt', '')
        elif sid == 36:
            (pattern, category) = ('actualSentPktThpt', '')
        elif sid == 37:
            (pattern, category) = ('averageSentPktThpt', '')
        elif sid == 38:
            (pattern, category) = ('peakSentPktThpt', '')
        elif sid == 39:
            (pattern, category) = ('actualTPktThpt', '')
        elif sid == 40:
            (pattern, category) = ('averageTPktThpt', '')
        elif sid == 41:
            (pattern, category) = ('peakTPktThpt', '')
        elif sid == 42:
            (pattern, category) = ('ipBytesSent', '')
        elif sid == 43:
            (pattern, category) = ('ipBytesRcvd', '')
        elif sid == 44:
            (pattern, category) = ('tcpBytesSent', '')
        elif sid == 45:
            (pattern, category) = ('tcpBytesRcvd', '')
        elif sid == 46:
            (pattern, category) = ('udpBytesSent', '')
        elif sid == 47:
            (pattern, category) = ('udpBytesRcvd', '')
        elif sid == 48:
            (pattern, category) = ('icmpSent', '')
        elif sid == 49:
            (pattern, category) = ('icmpRcvd', '')
        elif sid == 50:
            (pattern, category) = ('tcpSentRem', '')
        elif sid == 51:
            (pattern, category) = ('udpSentLoc', '')
        elif sid == 52:
            (pattern, category) = ('udpSentRem', '')
        elif sid == 53:
            (pattern, category) = ('ospfSent', '')
        elif sid == 54:
            (pattern, category) = ('igmpSent', '')
        elif sid == 55:
            (pattern, category) = ('tcpRcvdLoc', '')
        elif sid == 56:
            (pattern, category) = ('tcpRcvdFromRem', '')
        elif sid == 57:
            (pattern, category) = ('udpRcvdLoc', '')
        elif sid == 58:
            (pattern, category) = ('udpRcvdFromRem', '')
        elif sid == 59:
            (pattern, category) = ('ospfRcvd', '')
        elif sid == 60:
            (pattern, category) = ('igmpRcvd', '')
        elif sid == 61:
            (pattern, category) = ('tcpFragmentsSent', '')
        elif sid == 62:
            (pattern, category) = ('tcpFragmentsRcvd', '')
        elif sid == 63:
            (pattern, category) = ('udpFragmentsSent', '')
        elif sid == 64:
            (pattern, category) = ('udpFragmentsRcvd', '')
        elif sid == 65:
            (pattern, category) = ('icmpFragmentsSent', '')
        elif sid == 66:
            (pattern, category) = ('icmpFragmentsRcvd', '')
        elif sid == 67:
            (pattern, category) = ('stpSent', '')
        elif sid == 68:
            (pattern, category) = ('stpRcvd', '')
        elif sid == 69:
            (pattern, category) = ('ipxSent', '')
        elif sid == 70:
            (pattern, category) = ('ipxRcvd', '')
        elif sid == 71:
            (pattern, category) = ('osiSent', '')
        elif sid == 72:
            (pattern, category) = ('osiRcvd', '')
        elif sid == 73:
            (pattern, category) = ('dlcSent', '')
        elif sid == 74:
            (pattern, category) = ('dlcRcvd', '')
        elif sid == 75:
            (pattern, category) = ('arp_rarpSent', '')
        elif sid == 76:
            (pattern, category) = ('arp_rarpRcvd', '')
        elif sid == 77:
            (pattern, category) = ('arpReqPktsSent', '')
        elif sid == 78:
            (pattern, category) = ('arpReplyPktsSent', '')
        elif sid == 79:
            (pattern, category) = ('arpReplyPktsRcvd', '')
        elif sid == 80:
            (pattern, category) = ('decnetSent', '')
        elif sid == 81:
            (pattern, category) = ('decnetRcvd', '')
        elif sid == 82:
            (pattern, category) = ('appletalkSent', '')
        elif sid == 83:
            (pattern, category) = ('appletalkRcvd', '')
        elif sid == 84:
            (pattern, category) = ('netbiosSent', '')
        elif sid == 85:
            (pattern, category) = ('netbiosRcvd', '')
        elif sid == 86:
            (pattern, category) = ('ipv6Sent', '')
        elif sid == 87:
            (pattern, category) = ('ipv6Rcvd', '')
        elif sid == 88:
            (pattern, category) = ('otherSent', '')
        elif sid == 89:
            (pattern, category) = ('otherRcvd', '')
        elif sid == 90:
            (pattern, category) = ('synPktsSent', '')
        elif sid == 91:
            (pattern, category) = ('synPktsRcvd', '')
        elif sid == 92:
            (pattern, category) = ('rstPktsSent', '')
        elif sid == 93:
            (pattern, category) = ('rstPktsRcvd', '')
        elif sid == 94:
            (pattern, category) = ('rstAckPktsSent', '')
        elif sid == 95:
            (pattern, category) = ('rstAckPktsRcvd', '')
        elif sid == 96:
            (pattern, category) = ('synFinPktsSent', '')
        elif sid == 97:
            (pattern, category) = ('synFinPktsRcvd', '')
        elif sid == 98:
            (pattern, category) = ('finPushUrgPktsSent', '')
        elif sid == 99:
            (pattern, category) = ('finPushUrgPktsRcvd', '')
        elif sid == 100:
            (pattern, category) = ('nullPktsSent', '')
        elif sid == 101:
            (pattern, category) = ('nullPktsRcvd', '')
        elif sid == 102:
            (pattern, category) = ('ackScanSent', '')
        elif sid == 103:
            (pattern, category) = ('ackScanRcvd', '')
        elif sid == 104:
            (pattern, category) = ('xmasScanSent', '')
        elif sid == 105:
            (pattern, category) = ('xmasScanRcvd', '')
        elif sid == 106:
            (pattern, category) = ('finScanSent', '')
        elif sid == 107:
            (pattern, category) = ('finScanRcvd', '')
        elif sid == 108:
            (pattern, category) = ('nullScanSent', '')
        elif sid == 109:
            (pattern, category) = ('nullScanRcvd', '')
        elif sid == 110:
            (pattern, category) = ('rejectedTCPConnSent', '')
        elif sid == 111:
            (pattern, category) = ('rejectedTCPConnRcvd', '')
        elif sid == 112:
            (pattern, category) = ('establishedTCPConnSent', '')
        elif sid == 113:
            (pattern, category) = ('establishedTCPConnRcvd', '')
        elif sid == 114:
            (pattern, category) = ('terminatedTCPConnServer', '' )
        elif sid == 115:
            (pattern, category) = ('terminatedTCPConnClient', '' )
        elif sid == 116:
            (pattern, category) = ('udpToClosedPortSent', '')
        elif sid == 117:
            (pattern, category) = ('udpToClosedPortRcvd', '')
        elif sid == 118:
            (pattern, category) = ('udpToDiagnosticPortSent', '' )
        elif sid == 119:
            (pattern, category) = ('udpToDiagnosticPortRcvd', '' )
        elif sid == 120:
            (pattern, category) = ('tcpToDiagnosticPortSent', '' )
        elif sid == 121:
            (pattern, category) = ('tcpToDiagnosticPortRcvd', '' )
        elif sid == 122:
            (pattern, category) = ('tinyFragmentSent', '')
        elif sid == 123:
            (pattern, category) = ('tinyFragmentRcvd', '')
        elif sid == 124:
            (pattern, category) = ('icmpFragmentSent', '')
        elif sid == 125:
            (pattern, category) = ('icmpFragmentRcvd', '')
        elif sid == 126:
            (pattern, category) = ('overlappingFragmentSent', '' )
        elif sid == 127:
            (pattern, category) = ('overlappingFragmentRcvd', '' )
        elif sid == 128:
            (pattern, category) = ('closedEmptyTCPConnSent', '')
        elif sid == 129:
            (pattern, category) = ('closedEmptyTCPConnRcvd', '')
        elif sid == 130:
            (pattern, category) = ('icmpPortUnreachSent', '')
        elif sid == 131:
            (pattern, category) = ('icmpPortUnreachRcvd', '')
        elif sid == 132:
            (pattern, category) = ('icmpHostNetUnreachSent', '')
        elif sid == 133:
            (pattern, category) = ('icmpProtocolUnreachSent', '' )
        elif sid == 134:
            (pattern, category) = ('icmpProtocolUnreachRcvd', '' )
        elif sid == 135:
            (pattern, category) = ('icmpHostNetUnreachRcvd', '')
        elif sid == 136:
            (pattern, category) = ('icmpAdminProhibitedSent', '' )
        elif sid == 137:
            (pattern, category) = ('icmpAdminProhibitedRcvd', '' )
        elif sid == 138:
            (pattern, category) = ('malformedPktsSent', '')
        elif sid == 139:
            (pattern, category) = ('malformedPktsRcvd', '')
        elif sid == 140:
            (pattern, category) = ('sentLoc', 'FTP')
        elif sid == 141:
            (pattern, category) = ('sentRem', 'FTP')
        elif sid == 142:
            (pattern, category) = ('rcvdLoc', 'FTP')
        elif sid == 143:
            (pattern, category) = ('rcvdFromRem', 'FTP')
        elif sid == 144:
            (pattern, category) = ('sentLoc', 'HTTP')
        elif sid == 145:
            (pattern, category) = ('sentRem', 'HTTP')
        elif sid == 146:
            (pattern, category) = ('rcvdLoc', 'HTTP')
        elif sid == 147:
            (pattern, category) = ('rcvdFromRem', 'HTTP')
        elif sid == 148:
            (pattern, category) = ('sentLoc', 'DNS')
        elif sid == 149:
            (pattern, category) = ('sentRem', 'DNS')
        elif sid == 150:
            (pattern, category) = ('rcvdLoc', 'DNS')
        elif sid == 151:
            (pattern, category) = ('rcvdFromRem', 'DNS')
        elif sid == 152:
            (pattern, category) = ('sentLoc', 'Telnet')
        elif sid == 153:
            (pattern, category) = ('sentRem', 'Telnet')
        elif sid == 154:
            (pattern, category) = ('rcvdLoc', 'Telnet')
        elif sid == 155:
            (pattern, category) = ('rcvdFromRem', 'Telnet')
        elif sid == 156:
            (pattern, category) = ('sentLoc', 'NBios-IP')
        elif sid == 157:
            (pattern, category) = ('sentRem', 'NBios-IP')
        elif sid == 158:
            (pattern, category) = ('rcvdLoc', 'NBios-IP')
        elif sid == 159:
            (pattern, category) = ('rcvdFromRem', 'NBios-IP')
        elif sid == 160:
            (pattern, category) = ('sentLoc', 'Mail')
        elif sid == 161:
            (pattern, category) = ('sentRem', 'Mail')
        elif sid == 162:
            (pattern, category) = ('rcvdLoc', 'Mail')
        elif sid == 163:
            (pattern, category) = ('rcvdFromRem', 'Mail')
        elif sid == 164:
            (pattern, category) = ('sentLoc', 'DHCP-BOOTP')
        elif sid == 165:
            (pattern, category) = ('sentRem', 'DHCP-BOOTP')
        elif sid == 166:
            (pattern, category) = ('rcvdLoc', 'DHCP-BOOTP')
        elif sid == 167:
            (pattern, category) = ('rcvdFromRem', 'DHCP-BOOTP')
        elif sid == 168:
            (pattern, category) = ('sentLoc', 'SNMP')
        elif sid == 169:
            (pattern, category) = ('sentRem', 'SNMP')
        elif sid == 170:
            (pattern, category) = ('rcvdLoc', 'SNMP')
        elif sid == 171:
            (pattern, category) = ('rcvdFromRem', 'SNMP')
        elif sid == 172:
            (pattern, category) = ('sentLoc', 'NNTP')
        elif sid == 173:
            (pattern, category) = ('sentRem', 'NNTP')
        elif sid == 174:
            (pattern, category) = ('rcvdLoc', 'NNTP')
        elif sid == 175:
            (pattern, category) = ('rcvdFromRem', 'NNTP')
        elif sid == 176:
            (pattern, category) = ('sentLoc', 'NFS')
        elif sid == 177:
            (pattern, category) = ('sentRem', 'NFS')
        elif sid == 178:
            (pattern, category) = ('rcvdLoc', 'NFS')
        elif sid == 179:
            (pattern, category) = ('rcvdFromRem', 'NFS')
        elif sid == 180:
            (pattern, category) = ('sentLoc', 'X11')
        elif sid == 181:
            (pattern, category) = ('sentRem', 'X11')
        elif sid == 182:
            (pattern, category) = ('rcvdLoc', 'X11')
        elif sid == 183:
            (pattern, category) = ('rcvdFromRem', 'X11')
        elif sid == 184:
            (pattern, category) = ('sentLoc', 'SSH')
        elif sid == 185:
            (pattern, category) = ('sentRem', 'SSH')
        elif sid == 186:
            (pattern, category) = ('rcvdLoc', 'SSH')
        elif sid == 187:
            (pattern, category) = ('rcvdFromRem', 'SSH')
        elif sid == 188:
            (pattern, category) = ('sentLoc', 'Gnutella')
        elif sid == 189:
            (pattern, category) = ('sentRem', 'Gnutella')
        elif sid == 190:
            (pattern, category) = ('rcvdLoc', 'Gnutella')
        elif sid == 191:
            (pattern, category) = ('rcvdFromRem', 'Gnutella')
        elif sid == 192:
            (pattern, category) = ('sentLoc', 'Kazaa')
        elif sid == 193:
            (pattern, category) = ('sentRem', 'Kazaa')
        elif sid == 194:
            (pattern, category) = ('rcvdLoc', 'Kazaa')
        elif sid == 195:
            (pattern, category) = ('rcvdFromRem', 'Kazaa')
        elif sid == 196:
            (pattern, category) = ('sentLoc', 'WinMX')
        elif sid == 197:
            (pattern, category) = ('sentRem', 'WinMX')
        elif sid == 198:
            (pattern, category) = ('rcvdLoc', 'WinMX')
        elif sid == 199:
            (pattern, category) = ('rcvdFromRem', 'WinMX')
        elif sid == 200:
            (pattern, category) = ('sentLoc', 'DirectConnect')
        elif sid == 201:
            (pattern, category) = ('sentRem', 'DirectConnect')
        elif sid == 202:
            (pattern, category) = ('rcvdLoc', 'DirectConnect')
        elif sid == 203:
            (pattern, category) = ('rcvdFromRem', 'DirectConnect')
        elif sid == 204:
            (pattern, category) = ('sentLoc', 'eDonkey')
        elif sid == 205:
            (pattern, category) = ('sentRem', 'eDonkey')
        elif sid == 206:
            (pattern, category) = ('rcvdLoc', 'eDonkey')
        elif sid == 207:
            (pattern, category) = ('rcvdFromRem', 'eDonkey')
        elif sid == 208:
            (pattern, category) = ('sentLoc', 'Messenger')
        elif sid == 209:
            (pattern, category) = ('sentRem', 'Messenger')
        elif sid == 210:
            (pattern, category) = ('rcvdLoc', 'Messenger')
        elif sid == 211:
            (pattern, category) = ('rcvdFromRem', 'Messenger')
        elif sid == 212:
            (pattern, category) = ('sentLoc', 'PROXY')
        elif sid == 213:
            (pattern, category) = ('sentRem', 'PROXY')
        elif sid == 214:
            (pattern, category) = ('rcvdLoc', 'PROXY')
        elif sid == 215:
            (pattern, category) = ('rcvdFromRem', 'PROXY')
        elif sid == 216:
            (pattern, category) = ('sentLoc', 'NEWS')
        elif sid == 217:
            (pattern, category) = ('sentRem', 'NEWS')
        elif sid == 218:
            (pattern, category) = ('rcvdLoc', 'NEWS')
        elif sid == 219:
            (pattern, category) = ('rcvdFromRem', 'NEWS')
        elif sid == 220:
            (pattern, category) = ('SENT_ECHO', '')
        elif sid == 221:
            (pattern, category) = ('SENT_ECHOREPLY', '')
        elif sid == 222:
            (pattern, category) = ('SENT_UNREACH', '')
        elif sid == 223:
            (pattern, category) = ('SENT_ROUTERADVERT', '')
        elif sid == 224:
            (pattern, category) = ('SENT_TMXCEED', '')
        elif sid == 225:
            (pattern, category) = ('SENT_PARAMPROB', '')
        elif sid == 226:
            (pattern, category) = ('SENT_MASKREPLY', '')
        elif sid == 227:
            (pattern, category) = ('SENT_MASKREQ', '')
        elif sid == 228:
            (pattern, category) = ('SENT_INFO_REQUEST', '')
        elif sid == 229:
            (pattern, category) = ('SENT_INFO_REPLY', '')
        elif sid == 230:
            (pattern, category) = ('SENT_TIMESTAMP', '')
        elif sid == 231:
            (pattern, category) = ('SENT_TIMESTAMPREPLY', '')
        elif sid == 232:
            (pattern, category) = ('SENT_SOURCE_QUENCH', '')
        elif sid == 233:
            (pattern, category) = ('RCVD_ECHO', '')
        elif sid == 234:
            (pattern, category) = ('RCVD_ECHOREPLY', '')
        elif sid == 235:
            (pattern, category) = ('RCVD_UNREACH', '')
        elif sid == 236:
            (pattern, category) = ('RCVD_ROUTERADVERT', '')
        elif sid == 237:
            (pattern, category) = ('RCVD_TMXCEED', '')
        elif sid == 238:
            (pattern, category) = ('RCVD_PARAMPROB', '')
        elif sid == 239:
            (pattern, category) = ('RCVD_MASKREPLY', '')
        elif sid == 240:
            (pattern, category) = ('RCVD_MASKREQ', '')
        elif sid == 241:
            (pattern, category) = ('RCVD_INFO_REQUEST', '')
        elif sid == 242:
            (pattern, category) = ('RCVD_INFO_REPLY', '')
        elif sid == 243:
            (pattern, category) = ('RCVD_TIMESTAMP', '')
        elif sid == 244:
            (pattern, category) = ('RCVD_TIMESTAMPREPLY', '')
        elif sid == 245:
            (pattern, category) = ('RCVD_SOURCE_QUENCH', '')
        else:
            util.debug(__name__, "%d: Bad plugin_sid" % (sid), "**", 'RED')
            sys.exit()
            
        return (pattern, category)


class NtopHandler(xml.sax.handler.ContentHandler):
    
    ntop_data = {}
    
    def __init__(self, pattern, category):
        self.inContent = 0
        self.inCategory = 0
        self.theContent = ""
        self.pattern = pattern
        self.category = category
        self.ip = ''
        xml.sax.handler.ContentHandler.__init__(self)

    def startElement (self, name, attrs):
        if name == 'hostNumIpAddress':
            self.inContent = 1

        if name == self.category:
            self.inCategory = 1

        if name == self.pattern:
            self.inContent = 1



    def endElement (self, name):
        if self.inContent:
            self.theContent = util.normalizeWhitespace(self.theContent)

        if name == 'hostNumIpAddress':
            self.ip = self.theContent.encode("iso-8859-1")

        if self.category == '':
            if name == self.pattern:
                NtopHandler.ntop_data[self.ip] = \
                    self.theContent.encode("iso-8859-1")
        else:
            if name == self.pattern and self.inCategory:
                NtopHandler.ntop_data[self.ip] = \
                    self.theContent.encode("iso-8859-1")

        if name == self.category:
            self.inCategory = 0
            

    def characters (self, string):
        if self.inContent:
            self.theContent = string

