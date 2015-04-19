-- cisco-router
-- plugin_id: 1510
--
-- $Log: cisco-router.sql,v $
-- Revision 1.2  2007/03/26 18:36:15  juanmals
-- delete previous sids before inserting new ones
--
-- Revision 1.1  2006/11/07 14:43:33  dvgil
-- Part of cisco-router plugin migrated from the old agent. Still incomplete
--
DELETE FROM plugin WHERE id = "1510";
DELETE FROM plugin_sid where plugin_id = "1510";


INSERT INTO plugin (id, type, name, description) VALUES (1510, 1, 'cisco-router', 'Cisco router');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (1510, 1, NULL, NULL, 'cisco router: Attempted to connect to RSHELL', 3, 1);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1510, 2, NULL, NULL, 'cisco router: Clear counter on all interfaces');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1510, 3, NULL, NULL, 'cisco router: Line protocol changed state');

