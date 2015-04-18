import re
import sys
import time

import Parser
import util

class ParserRRD(Parser.Parser):

    RRD_ids  = {
        "rrd_threshold": '1507',
        "rrd_anomaly":   '1508',
    }

    RRD_sids = {
        
        'global': 
            {
                'activeHostSendersNum': '1',
                'arpRarpBytes':         '2',
                'broadcastPkts':        '3',
                'ethernetBytes':        '4',
                'ethernetPkts':         '5',
                'fragmentedIpBytes':    '6',
                'icmpBytes':            '7',
                'igmpBytes':            '8',
                'ipBytes':              '9',
                'IP_DHCP-BOOTPBytes':   '10',
                'IP_DNSBytes':          '11',
                'IP_eDonkeyBytes':      '12',
                'IP_FTPBytes':          '13',
                'IP_GnutellaBytes':     '14',
                'IP_HTTPBytes':         '15',
                'IP_KazaaBytes':        '16',
                'IP_MailBytes':         '17',
                'IP_MessengerBytes':    '18',
                'IP_NBios-IPBytes':     '19',
                'IP_NFSBytes':          '20',
                'IP_NNTPBytes':         '21',
                'IP_SNMPBytes':         '22',
                'IP_SSHBytes':          '23',
                'IP_TelnetBytes':       '24',
                'ipv6Bytes':            '25',
                'IP_WinMXBytes':        '26',
                'IP_X11Bytes':          '27',
                'ipxBytes':             '28',
                'knownHostsNum':        '29',
                'multicastPkts':        '30',
                'otherBytes':           '31',
                'otherIpBytes':         '32',
                'stpBytes':             '33',
                'tcpBytes':             '34',
                'udpBytes':             '35',
                'upTo1024Pkts':         '36',
                'upTo128Pkts':          '37',
                'upTo1518Pkts':         '38',
                'upTo256Pkts':          '39',
                'upTo512Pkts':          '40',
                'upTo64Pkts':           '41',    
            },
            
        'host':
            {
                'arp_rarpRcvd':         '42',
                'arp_rarpSent':         '43',
                'arpReplyPktsRcvd':     '44',
                'arpReplyPktsSent':     '45',
                'arpReqPktsSent':       '46',
                'bytesBroadcastSent':   '47',
                'bytesRcvdLoc':         '48',
                'bytesRcvd':            '49',
                'bytesSentLoc':         '50',
                'bytesSent':            '51',
                'icmpRcvd':             '52',
                'icmpSent':             '53',
                'ipBytesRcvd':          '54',
                'ipBytesSent':          '55',
                'IP_DNSRcvdBytes':      '56',
                'IP_FTPRcvdBytes':      '57',
                'IP_FTPSentBytes':      '58',
                'IP_HTTPRcvdBytes':     '59',
                'IP_HTTPSentBytes':     '60',
                'IP_MailRcvdBytes':     '61',
                'IP_MailSentBytes':     '62',
                'IP_SNMPRcvdBytes':     '63',
                'IP_SSHRcvdBytes':      '64',
                'IP_SSHSentBytes':      '65',
                'IP_TelnetRcvdBytes':   '66',
                'IP_TelnetSentBytes':   '67',
                'pktBroadcastSent':     '68',
                'pktRcvd':              '69',
                'pktSent':              '70',
                'tcpRcvdLoc':           '71',
                'tcpSentLoc':           '72',
                'totContactedRcvdPeers':'73',
                'totContactedSentPeers':'74',
                'udpRcvdLoc':           '75',
                'synPktsSent':          '76',
                'synPktsRcvd':          '77',
                'totContactedSentPeers':'78',
                'totContactedRcvdPeers':'79',
                'web_sessions':         '80',
                'mail_sessions':        '81',
                'nb_sessions':          '82',
            },
    }


    def process(self):

        if self.plugin["source"] == 'rrd_plugin':
            self.__processSyslog()
            
        else:
            util.debug (__name__,  "log type " + self.plugin["source"] +\
                        " unknown for RRD...", '!!', 'RED')
            sys.exit()


    def __processSyslog(self):
        
        util.debug ('ParserRRD', 'plugin started (syslog)...', '--')
        
        pattern = '([^:]+): (\S+) (\S+) (\S+) (\S+) (\S+) (\S+)'
            
        location = self.plugin["location"]
        try:
            fd = open(location, 'r')
        except IOError, e:
            util.debug(__name__, e, '!!', 'RED')
            sys.exit()
            
        # Move to the end of file
        fd.seek(0, 2)
            
        while 1:
            
            if self.plugin["enable"] == 'no':

                # plugin disabled, wait for enabled
                util.debug (__name__, 'plugin disabled', '**', 'RED')
                while self.plugin["enable"] == 'no':
                    time.sleep(1)
                    
                # lets parse again
                util.debug (__name__, 'plugin enabled', '**', 'GREEN')
                fd.seek(0, 2)

            where = fd.tell()
            line = fd.readline()
            if not line: # EOF reached
                time.sleep(1)
                fd.seek(where)
            else:
                result = re.findall(str(pattern), line)
                try: 
                    (plugin, time_alert, ip, interface, plugin_sid, \
                     priority, data) = result[0]
                   
                    if ip == 'GLOBAL': 
                        ip = '0.0.0.0'
                        sid = ParserRRD.RRD_sids["global"][plugin_sid]
                    else:
                        sid = ParserRRD.RRD_sids["host"][plugin_sid]

                    date = time.strftime('%Y-%m-%d %H:%M:%S', 
                                         time.localtime(float(time_alert)))

                    self.agent.sendAlert  (type = 'detector',
                                     date       = date,
                                     sensor     = self.plugin["sensor"],
                                     interface  = interface,
                                     plugin_id  = ParserRRD.RRD_ids[plugin],
                                     plugin_sid = sid,
                                     priority   = priority,
                                     protocol   = '',
                                     src_ip     = ip,
                                     src_port   = '',
                                     dst_ip     = '',
                                     dst_port   = '')

                except IndexError: 
                    pass
                except KeyError:
                    util.debug (__name__, 'Unknown plugin id (%s)' %
                                plugin_sid, '**', 'RED')
        fd.close()

