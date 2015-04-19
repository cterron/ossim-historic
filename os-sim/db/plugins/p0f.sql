-- p0f
-- plugin_id: 1511
--
-- $Id: p0f.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "1511";
DELETE FROM plugin_sid where plugin_id = "1511";


INSERT INTO plugin (id, type, name, description) VALUES (1511, 1, 'p0f', 'Passive OS fingerprinting tool');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1511, 1, NULL, NULL, 'p0f: New OS');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1511, 2, NULL, NULL, 'p0f: OS Change');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1511, 3, NULL, NULL, 'p0f: OS Deleted');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1511, 4, NULL, NULL, 'p0f: OS Same');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1511, 5, NULL, NULL, 'p0f: OS Event unknown');


