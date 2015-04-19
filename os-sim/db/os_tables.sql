-- Type 4 == Special type
INSERT INTO plugin (id, type, name, description) VALUES (5001, 4, "os", "Operating Systems");
INSERT INTO plugin (id, type, name, description) VALUES (5002, 4, "services", "Services / Ports");

-- OS
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 1, NULL, NULL, 1, 1, "Generic Windows");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 2, NULL, NULL, 1, 1, "Generic Linux");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 3, NULL, NULL, 1, 1, "Generic Cisco");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 4, NULL, NULL, 1, 1, "Generic BSD");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 5, NULL, NULL, 1, 1, "Generic FreeBSD");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 6, NULL, NULL, 1, 1, "Generic NetBSD");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 7, NULL, NULL, 1, 1, "Generic OpenBSD");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 8, NULL, NULL, 1, 1, "Generic HP-UX");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 9, NULL, NULL, 1, 1, "Generic Solaris");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 10, NULL, NULL, 1, 1, "Generic Macos");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 11, NULL, NULL, 1, 1, "Generic Plan9");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 12, NULL, NULL, 1, 1, "Generic SCO");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 13, NULL, NULL, 1, 1, "Generic AIX");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES (5001, 14, NULL, NULL, 1, 1, "Generic UNIX");

