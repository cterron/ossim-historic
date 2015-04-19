-- nmap
-- type: monitor
-- plugin_id: 2008
--
-- $Id: nmap-monitor.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "2008";
DELETE FROM plugin_sid where plugin_id = "2008";


INSERT INTO plugin (id, type, name, description) VALUES (2008, 2, 'nmap', 'Nmap: network mapper');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2008, 1, NULL, NULL, 'nmap: port opened');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2008, 2, NULL, NULL, 'nmap: port closed');

