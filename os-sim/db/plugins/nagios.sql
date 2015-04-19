-- nagios
-- plugin_id: 1525
--
-- $Id: nagios.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "1525";
DELETE FROM plugin_sid where plugin_id = "1525";


INSERT INTO plugin (id, type, name, description) VALUES (1525, 1, 'nagios', 'Nagios: host/service/network monitoring and management system');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1525, 1, NULL, NULL, 'nagios: host up');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1525, 2, NULL, NULL, 'nagios: host down');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1525, 3, NULL, NULL, 'nagios: service ok');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1525, 4, NULL, NULL, 'nagios: service warning');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1525, 5, NULL, NULL, 'nagios: service critical');


