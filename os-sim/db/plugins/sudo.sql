-- sudo
-- plugin_id: 4005
DELETE FROM plugin WHERE id = "4005";
DELETE FROM plugin_sid where plugin_id = "4005";


INSERT INTO plugin (id, type, name, description) VALUES (4005, 1, 'sudo', 'Sudo allows users to run programs with the security privileges of another user in a secure manner');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4005, 1, NULL, NULL, 'sudo: Failed su');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4005, 2, NULL, NULL, 'sudo: Succesful su');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4005, 3, NULL, NULL, 'sudo: Command executed');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4005, 4, NULL, NULL, 'sudo: User not in sudoers');
