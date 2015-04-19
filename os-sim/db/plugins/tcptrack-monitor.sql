-- tcptrack
-- type: monitor
-- plugin_id: 2006
--
-- $Id: tcptrack-monitor.sql,v 1.3 2007/08/03 11:35:49 alberto_r Exp $
--
DELETE FROM plugin WHERE id = "2006";
DELETE FROM plugin_sid where plugin_id = "2006";

INSERT INTO plugin (id, type, name, description) VALUES (2006, 2, 'tcptrack', 'tcptrack');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2006, 1, NULL, NULL, 'tcptrack: Session Data Sent');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2006, 2, NULL, NULL, 'tcptrack: Session Data Rcvd');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2006, 3, NULL, NULL, 'tcptrack: Session Duration');


