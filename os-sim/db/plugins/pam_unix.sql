-- pam_unix
-- type: detector
-- plugin_id: 4004
--
-- $Id: pam_unix.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "4004";
DELETE FROM plugin_sid where plugin_id = "4004";


INSERT INTO plugin (id, type, name, description) VALUES (4004, 1, 'pam_unix', 'Pam Unix authentication mechanism');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4004, 1, NULL, NULL, 'pam_unix: authentication successful');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4004, 2, NULL, NULL, 'pam_unix: authentication failure');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4004, 3, NULL, NULL, 'pam_unix: 2 more authentication failures');

