-- Reserved plugin_sid's
-- Here will be all the plugins that we are testing. Or plugins still not ready or semi-deprecated
-- 
--

-- Prelude
DELETE FROM plugin WHERE id = "1513";
DELETE FROM plugin_sid where plugin_id = "1513";

INSERT INTO plugin (id, type, name, description) VALUES (1513, 1, 'prelude', 'Prelude Hybrid IDS');

-- syslog 
INSERT INTO plugin (id, type, name, description) VALUES (4002, 1, 'syslogd', 'Syslog Daemon');

-- Nessus
INSERT INTO plugin (id, type, name, description) VALUES (3001, 3, 'nessus', 'Nessus');

-- nmap
INSERT INTO plugin (id, type, name, description) VALUES (3002, 3, 'nmap', 'NMap');



