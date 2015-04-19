-- opennms
-- type: monitor
-- plugin_id: 2004
--
-- $Id: opennms-monitor.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "2004";
DELETE FROM plugin_sid where plugin_id = "2004";


INSERT INTO plugin (id, type, name, description) VALUES (2004, 2, 'opennms', 'OpenNMS');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2004, 1, NULL, NULL, 'opennms: Service Up');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2004, 2, NULL, NULL, 'opennms: Service Down');
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2004, 3, NULL, NULL, 'open_nms: Service Availability (%)');
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2004, 4, NULL, NULL, 'open_nms: Service Deleted');
-- INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (2004, 5, NULL, NULL, 'open_nms: New Service Added');


