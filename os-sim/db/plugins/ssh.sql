-- ssh
-- plugin_id: 4003
DELETE FROM plugin WHERE id = "4003";
DELETE FROM plugin_sid where plugin_id = "4003";


INSERT INTO plugin (id, type, name, description) VALUES (4003, 1, 'ssh', 'SSH: Secure Shell is a program for logging into a remote machine and for executing commands');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 1, NULL, NULL, 'ssh: Failed password');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 2, NULL, NULL, 'ssh: Failed publickey');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 3, NULL, NULL, 'ssh: Invalid user');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 4, NULL, NULL, 'ssh: Illegal user');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 5, NULL, NULL, 'ssh: Root login refused');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 6, NULL, NULL, 'User not allowed because listed in DenyUsers');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 7, NULL, NULL, 'ssh: Login sucessful (Accepted password)');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 8, NULL, NULL, 'ssh: Login sucessful (Accepted publickey)');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 9, NULL, NULL, 'ssh: Bad protocol version identification');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4003, 10, NULL, NULL, 'ssh: Did not receive identification string');
