-- Stonegate firewall
-- plugin_id: 1526
--
-- $Id $
--

DELETE FROM plugin WHERE id = "1526";
DELETE FROM plugin_sid WHERE plugin_id = "1526";

INSERT INTO plugin (id, type, name, description) VALUES (1526, 1, 'stonegate', 'Stonegate Firewall');

INSERT INTO plugin_sid VALUES (1526, 1, 'NULL', 'NULL', 1, 1, 'stonegate: new connection');
INSERT INTO plugin_sid VALUES (1526, 2, 'NULL', 'NULL', 1, 1, 'stonegate: connection closed ');
INSERT INTO plugin_sid VALUES (1526, 3, 'NULL', 'NULL', 1, 1, 'stonegate: incomplete connection closed');
INSERT INTO plugin_sid VALUES (1526, 4, 'NULL', 'NULL', 1, 1, 'stonegate: connection discarded');
INSERT INTO plugin_sid VALUES (1526, 5, 'NULL', 'NULL', 1, 1, 'stonegate: packet discarded');
INSERT INTO plugin_sid VALUES (1526, 6, 'NULL', 'NULL', 1, 1, 'stonegate: error');
INSERT INTO plugin_sid VALUES (1526, 7, 'NULL', 'NULL', 1, 1, 'stonegate: Related connection');
INSERT INTO plugin_sid VALUES (1526, 8, 'NULL', 'NULL', 1, 1, 'stonegate: Related packet');
INSERT INTO plugin_sid VALUES (1526, 9, 'NULL', 'NULL', 1, 1, 'stonegate: Tester notice');
INSERT INTO plugin_sid VALUES (1526, 10, 'NULL', 'NULL', 1, 1, 'stonegate: Security policy reload');

