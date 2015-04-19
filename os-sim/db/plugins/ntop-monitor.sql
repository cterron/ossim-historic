-- ntop
-- type: monitor
-- plugin_id: 2005
--
-- $Id: ntop-monitor.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "2005";
DELETE FROM plugin_sid where plugin_id = "2005";


INSERT INTO plugin (id, type, name, description) VALUES (2005, 2, 'ntop', 'NTop');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 1, NULL, NULL, 'ntop: firstSeen');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 2, NULL, NULL, 'ntop: lastSeen');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 3, NULL, NULL, 'ntop: minTTL');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 4, NULL, NULL, 'ntop: maxTTL');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 5, NULL, NULL, 'ntop: pktSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 6, NULL, NULL, 'ntop: pktRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 7, NULL, NULL, 'ntop: ipBytesSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 8, NULL, NULL, 'ntop: ipBytesRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 9, NULL, NULL, 'ntop: pktDuplicatedAckSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 10, NULL, NULL, 'ntop: pktDuplicatedAckRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 11, NULL, NULL, 'ntop: pktBroadcastSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 12, NULL, NULL, 'ntop: bytesMulticastSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 13, NULL, NULL, 'ntop: pktMulticastSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 14, NULL, NULL, 'ntop: bytesMulticastRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 15, NULL, NULL, 'ntop: pktMulticastRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 16, NULL, NULL, 'ntop: bytesSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 17, NULL, NULL, 'ntop: bytesSentLoc');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 18, NULL, NULL, 'ntop: bytesSentRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 19, NULL, NULL, 'ntop: bytesRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 20, NULL, NULL, 'ntop: bytesRcvdLoc');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 21, NULL, NULL, 'ntop: bytesRcvdFromRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 22, NULL, NULL, 'ntop: actualRcvdThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 23, NULL, NULL, 'ntop: lastHourRcvdThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 24, NULL, NULL, 'ntop: averageRcvdThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 25, NULL, NULL, 'ntop: peakRcvdThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 26, NULL, NULL, 'ntop: actualSentThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 27, NULL, NULL, 'ntop: lastHourSentThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 28, NULL, NULL, 'ntop: averageSentThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 29, NULL, NULL, 'ntop: peakSentThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 30, NULL, NULL, 'ntop: actualTThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 31, NULL, NULL, 'ntop: averageTThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 32, NULL, NULL, 'ntop: peakTThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 33, NULL, NULL, 'ntop: actualRcvdPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 34, NULL, NULL, 'ntop: averageRcvdPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 35, NULL, NULL, 'ntop: peakRcvdPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 36, NULL, NULL, 'ntop: actualSentPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 37, NULL, NULL, 'ntop: averageSentPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 38, NULL, NULL, 'ntop: peakSentPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 39, NULL, NULL, 'ntop: actualTPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 40, NULL, NULL, 'ntop: averageTPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 41, NULL, NULL, 'ntop: peakTPktThpt');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 42, NULL, NULL, 'ntop: ipBytesSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 43, NULL, NULL, 'ntop: ipBytesRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 44, NULL, NULL, 'ntop: ipv6Sent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 45, NULL, NULL, 'ntop: ipv6Rcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 46, NULL, NULL, 'ntop: tcpBytesSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 47, NULL, NULL, 'ntop: tcpBytesRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 48, NULL, NULL, 'ntop: udpBytesSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 49, NULL, NULL, 'ntop: udpBytesRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 50, NULL, NULL, 'ntop: icmpSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 51, NULL, NULL, 'ntop: icmpRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 52, NULL, NULL, 'ntop: tcpSentRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 53, NULL, NULL, 'ntop: udpSentLoc');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 54, NULL, NULL, 'ntop: udpSentRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 55, NULL, NULL, 'ntop: tcpRcvdLoc');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 56, NULL, NULL, 'ntop: tcpRcvdFromRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 57, NULL, NULL, 'ntop: udpRcvdLoc');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 58, NULL, NULL, 'ntop: udpRcvdFromRem');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 59, NULL, NULL, 'ntop: tcpFragmentsSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 60, NULL, NULL, 'ntop: tcpFragmentsRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 61, NULL, NULL, 'ntop: udpFragmentsSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 62, NULL, NULL, 'ntop: udpFragmentsRcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 63, NULL, NULL, 'ntop: icmpFragmentsSent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2005, 64, NULL, NULL, 'ntop: icmpFragmentsRcvd');

