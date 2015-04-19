-- SSHd
-- plugin_id: 4003

DELETE FROM plugin WHERE id = "4003";
DELETE FROM plugin_sid where plugin_id = "4003";


INSERT INTO plugin (id, type, name, description) VALUES (4003, 1, 'sshd', 'SSHd: Secure Shell daemon');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 1, NULL, NULL, 'SSHd: Failed password', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 2, NULL, NULL, 'SSHd: Failed publickey', 2, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 3, NULL, NULL, 'SSHd: Invalid user', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 4, NULL, NULL, 'SSHd: Illegal user', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 5, NULL, NULL, 'SSHd: Root login refused', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 6, NULL, NULL, 'SSHd: User not allowed because listed in DenyUsers', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 7, NULL, NULL, 'SSHd: Login sucessful, Accepted password', 1, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 8, NULL, NULL, 'SSHd: Login sucessful, Accepted publickey', 1, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 9, NULL, NULL, 'SSHd: Bad protocol version identification', 3, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 10, NULL, NULL, 'SSHd: Did not receive identification string', 1, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (4003, 11, NULL, NULL, 'SSHd: Received disconnect', 1, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority,reliability) VALUES (4003, 12, NULL, NULL, 'SSHd: Authentication refused: bad ownership or modes', 1, 2);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority,reliability) VALUES (4003, 13, NULL, NULL, 'SSHd: User not allowed becase account is locked', 1, 2);
