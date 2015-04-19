-- symantec-ams
-- plugin_id: 1554
--

DELETE FROM plugin WHERE id = 1554;
DELETE FROM plugin_sid WHERE plugin_id=1554;
INSERT INTO plugin (id, type, name, description) VALUES (1554, 1, 'symantec-ams', 'Symantec AntiVirus Corporate Edition');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (1554, 1, NULL, NULL, 'symantec-ams: Virus Found', 2, 5);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (1554, 2, NULL, NULL, 'symantec-ams: Risk Repaired', 2, 5);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (1554, 3, NULL, NULL, 'symantec-ams: Risk Repaired Failed', 4, 5);
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name, priority, reliability) VALUES (1554, 4, NULL, NULL, 'symantec-ams: Virus Definition File Update', 0, 3);

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1554, 50, NULL, NULL, 'symantec-ams: Configuration Error');
