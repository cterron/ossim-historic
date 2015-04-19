-- ossim-ca
-- type: monitor
-- plugin_id: 2001
-- description: Ossim Compromise and Attack Monitor
--
-- $Id: ossim-monitor.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "2001";
DELETE FROM plugin_sid where plugin_id = "2001";


INSERT INTO plugin (id, type, name, description) VALUES (2001, 2, 'ossim-ca', 'Ossim compromise and attack monitor');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2001, 1, NULL, NULL, 'ossim-ca: Compromise value');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2001, 2, NULL, NULL, 'ossim-ca: Attack value');

