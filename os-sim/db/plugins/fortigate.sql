-- fortinet / fortigate
-- plugin_id: 1554
--
-- $Id: fortigate.sql,v 1.6 2009/06/04 15:07:19 dkarg Exp $
--
DELETE FROM plugin WHERE id = "1554";
DELETE FROM plugin_sid where plugin_id = "1554";


INSERT INTO plugin (id, type, name, description) VALUES (1554, 1, 'fortigate', 'Fortinet / Fortigate');


-- Traffic Log
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 10000, NULL, NULL, 'Fortigate: Policy allowed traffic');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 13000, NULL, NULL, 'Fortigate: Policy violation traffic');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 16000, NULL, NULL, 'Fortigate: Policy other traffic');

-- Event Log
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 20000, NULL, NULL, 'Fortigate: System activity event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 23000, NULL, NULL, 'Fortigate: IPSec negotiation event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 26000, NULL, NULL, 'Fortigate: DHCP service event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 29000, NULL, NULL, 'Fortigate: L2TP/PPTP/PPPoE service event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 32000, NULL, NULL, 'Fortigate: admin event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 35000, NULL, NULL, 'Fortigate: HA activity event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 38000, NULL, NULL, 'Fortigate: Firewall authentication event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 41000, NULL, NULL, 'Fortigate: Pattern update event');
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, ?, NULL, NULL, 'Fortigate: Alert email notifications');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99500, NULL, NULL, 'Fortigate: FortiGate-4000 and FortiGate-5000 series chassis event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99600, NULL, NULL, 'Fortigate: SSL VPN user event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99700, NULL, NULL, 'Fortigate: SSL VPN administration event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99800, NULL, NULL, 'Fortigate: SSL VPN session event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 45000, NULL, NULL, 'Fortigate: VIP SSL event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 46000, NULL, NULL, 'Fortigate: LDB monitor event');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 47000, NULL, NULL, 'Fortigate: Performance statistics');

-- Antivirus Log
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 60000, NULL, NULL, 'Fortigate: Virus infected');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 63000, NULL, NULL, 'Fortigate: Filename blocked');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 66000, NULL, NULL, 'Fortigate: File oversized');


-- Web Filter Log
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, ?, NULL, NULL, 'Fortigate: Content block');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 93000, NULL, NULL, 'Fortigate: URL filter');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 93013, NULL, NULL, 'Fortigate: FortiGuard block');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99510, NULL, NULL, 'Fortigate: FortiGuard allowed');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 99000, NULL, NULL, 'FortiGuard error');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 91010, NULL, NULL, 'Fortigate: ActiveX script filter');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 91000, NULL, NULL, 'Fortigate: Cookie script filter');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 91005, NULL, NULL, 'Fortigate: Applet script filter');

-- Attack Log
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 70000, NULL, NULL, 'Fortigate: Attack signature');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 73000, NULL, NULL, 'Fortigate: Attack anomaly');

-- Spam Filter Log
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 80000, NULL, NULL, 'Fortigate: SMTP spam filter');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 83000, NULL, NULL, 'Fortigate: POP3 spam filter');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 86000, NULL, NULL, 'Fortigate: IMAP spam filter');

-- Instant Messaging
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 103000, NULL, NULL, 'Fortigate: Instant messaging activity');

-- Content Archive
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, ?, NULL, NULL, 'Fortigate: HTTP Content metadata');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 80001, NULL, NULL, 'Fortigate: FTP content metadata');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 60001, NULL, NULL, 'Fortigate: SMTP content metadata');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 70001, NULL, NULL, 'Fortigate: POP3 content metadata');
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, ?, NULL, NULL, 'Fortigate: IMAP content metadata');


INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 999, NULL, NULL, 'Fortigate: Uknown Event, please report at http://www.ossim.net/bugs/');

