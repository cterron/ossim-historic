--
-- DEPRECATED. Do not use this file, instead use:
-- os-sim/include/upgrades/0.9.9_mysql.sql
--
INSERT INTO config (conf, value) VALUES ('nagios_link', '/nagios/');
INSERT INTO config (conf, value) VALUES ('user_action_log', '1');
INSERT INTO config (conf, value) VALUES ("frameworkd_address","127.0.0.1");
INSERT INTO config (conf, value) VALUES ("frameworkd_port","40003");
INSERT INTO config (conf, value) VALUES ("nessusrc_path","/usr/share/ossim/www/vulnmeter/tmp/.nessusrc");
INSERT INTO config (conf, value) VALUES ('rrdpath_incidents', '/var/lib/ossim/rrd/incidents/');
INSERT INTO config (conf, value) VALUES ('event_viewer', 'acid');
DELETE FROM config WHERE conf = "alert_viewer";
ALTER TABLE host_services ADD sensor int(10) UNSIGNED NOT NULL AFTER origin;
ALTER TABLE host_os ADD sensor int(10) UNSIGNED NOT NULL AFTER anom;


/* ======== actions ======== */
DROP TABLE IF EXISTS action;
CREATE TABLE action (
    id              int NOT NULL auto_increment,
    action_type     varchar(100) NOT NULL,
    descr           varchar(255) NOT NULL,
    PRIMARY KEY     (id)
);

DROP TABLE IF EXISTS action_type;
CREATE TABLE action_type (
    _type            varchar (100) NOT NULL,
    descr           varchar (255) NOT NULL,
    PRIMARY KEY     (_type)
);

INSERT INTO action_type (_type, descr) VALUES ("email", "send an email message");
INSERT INTO action_type (_type, descr) VALUES ("exec", "execute an external program");


DROP TABLE IF EXISTS action_email;
CREATE TABLE action_email (
    action_id       int NOT NULL,
    _from           varchar(100) NOT NULL,
    _to             varchar(100) NOT NULL,
    subject         varchar(255) NOT NULL,
    message         varchar(255) NOT NULL,
    PRIMARY KEY     (action_id)
);


DROP TABLE IF EXISTS action_exec;
CREATE TABLE action_exec (
    action_id       int NOT NULL,
    command         varchar(255) NOT NULL,
    PRIMARY KEY     (action_id)
);


/* ======== response ========== */
DROP TABLE IF EXISTS response;
CREATE TABLE response (
    id          int NOT NULL auto_increment,
    descr       varchar(255),
    PRIMARY KEY (id)
);


DROP TABLE IF EXISTS response_host;
CREATE TABLE response_host (
    response_id int NOT NULL,
    host        varchar(15),
    _type       ENUM ('source', 'dest', 'sensor') NOT NULL DEFAULT 'source',
    PRIMARY KEY (response_id, host, _type)
);

DROP TABLE IF EXISTS response_net;
CREATE TABLE response_net (
    response_id int NOT NULL,
    net         varchar(255),
    _type       ENUM ('source', 'dest') NOT NULL DEFAULT 'source',
    PRIMARY KEY (response_id, net, _type)
);

DROP TABLE IF EXISTS response_port;
CREATE TABLE response_port (
    response_id int NOT NULL,
    port        int NOT NULL,
    _type       ENUM ('source', 'dest') NOT NULL DEFAULT 'source',
    PRIMARY KEY (response_id, port, _type)
);

DROP TABLE IF EXISTS response_plugin;
CREATE TABLE response_plugin (
    response_id int NOT NULL,
    plugin_id   int NOT NULL,
    PRIMARY KEY (response_id, plugin_id)
);

DROP TABLE IF EXISTS response_action;
CREATE TABLE response_action (
    response_id int NOT NULL,
    action_id   int NOT NULL,
    PRIMARY KEY (response_id, action_id)
);


DROP TABLE IF EXISTS incident_event;
CREATE TABLE incident_event (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    src_ips         VARCHAR(255) NOT NULL,
    src_ports       VARCHAR(255) NOT NULL,
    dst_ips         VARCHAR(255) NOT NULL,
    dst_ports       VARCHAR(255) NOT NULL,
    PRIMARY KEY (id, incident_id)
);

ALTER TABLE users ADD COLUMN email varchar(64) after pass;

--
-- User action logging
--

DROP TABLE IF EXISTS log_config;
CREATE TABLE log_config(
    code        INTEGER UNSIGNED NOT NULL,
    log         BOOL	DEFAULT "0",
    descr       VARCHAR(255) NOT NULL,
    priority    INTEGER UNSIGNED NOT NULL,
    PRIMARY KEY (code)
);

DROP TABLE IF EXISTS log_action;
CREATE TABLE log_action(
    login       VARCHAR(255) NOT NULL,
    ipfrom        VARCHAR(15) NOT NULL,
    date        TIMESTAMP,
    code        INTEGER UNSIGNED NOT NULL,
    info        VARCHAR(255) NOT NULL,
    PRIMARY KEY (date, code, info)
);

ALTER TABLE host_mac ADD sensor int(10) unsigned AFTER vendor;
ALTER TABLE host_mac DROP PRIMARY KEY;
ALTER TABLE host_mac ADD PRIMARY KEY (ip, sensor);

DELETE FROM log_config;
INSERT INTO log_config (code, log, descr, priority) VALUES (001, 1, 'User %1% logged in', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (002, 1, 'User %1% logged out', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (003, 1, 'Configuration - User %1% deleted', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (004, 1, 'Configuration - User %1% created', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (005, 1, 'Configuration - User %1% password changed', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (006, 0, 'Configuration - User %1% info modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (007, 1, 'Configuration - configuration modified %1%', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (008, 1, 'Configuration - Reset defaults values', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (009, 0, 'Configuration - RRD profile added', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (010, 1, 'Configuration - New host %1% scan configuration added', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (011, 0, 'Control panel - Alarm %1% deleted ', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (012, 1, 'Control panel - Alarm %1% closed', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (013, 0, 'Control panel - Alarms deleted (hole day)', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (014, 1, 'Reports - Incident closed %1%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (015, 0, 'Reports - Incident %1% modified', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (016, 0, 'Reports - Incident %1% deleted', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (018, 1, 'Reports - Security report generated', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (019, 0, 'Reports - PDF report generated', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (020, 1, 'Monitor - Riskmeter', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (021, 0, 'Monitor - Session', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (022, 0, 'Monitor - Netork', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (023, 1, 'Monitor - Sensors', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (024, 0, 'Policy - Host: new host added %1% %2%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (025, 1, 'Policy - Host: host %1% deleted', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (026, 0, 'Policy - Host: host %1% %2% modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (027, 0, 'Policy - Networks: new network added %1%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (028, 1, 'Policy - Networks: network %1% modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (029, 0, 'Policy - Networks: network %1% deleted', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (030, 1, 'Policy - Net. groups: new net group added %1%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (031, 0, 'Policy - Net. group %1% modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (032, 0, 'Policy - Net. group %1% deleted', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (033, 1, 'Policy - Sensors: New sensor added %1% %2%', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (034, 0, 'Policy - Sensors: Sensor %1% %2% modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (035, 0, 'Policy - Sensors: Sensor %1% deleted', 3);
INSERT INTO log_config (code, log, descr, priority) VALUES (036, 1, 'Policy - Signature: New signature group %1%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (037, 0, 'Policy - Signature: signature group modified %1%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (038, 1, 'Policy - Signature: signature group %1% deleted', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (039, 0, 'Policy - Ports: New port group added %1% %2%', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (040, 0, 'Policy - Ports: Port group %1% %2% modified', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (041, 1, 'Policy - Ports: Port group %1%  %2% deleted', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (042, 0, 'Correlation - Backlog %1% delete', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (043, 1, 'Tools - Network scan %1% ', 1);
INSERT INTO log_config (code, log, descr, priority) VALUES (044, 0, 'Tools - Backup restored %1% ', 2);
INSERT INTO log_config (code, log, descr, priority) VALUES (045, 0, 'Tools - Backup deleted %1% ', 3);


--
-- Users extra info, company and department
--
ALTER TABLE users ADD COLUMN company varchar(128) after email;
ALTER TABLE users ADD COLUMN department varchar(64) after company;

--
-- Incident type
--
ALTER TABLE incident ADD COLUMN type_id VARCHAR(64) NOT NULL DEFAULT "Generic" after ref;

DROP TABLE IF EXISTS incident_type;
CREATE TABLE incident_type (
    id          VARCHAR(64) NOT NULL,
    descr       VARCHAR(255) NOT NULL DEFAULT "",
    PRIMARY KEY (id)
);

INSERT INTO incident_type (id, descr) VALUES ("Generic", "");
INSERT INTO incident_type (id, descr) VALUES ("Expansion Virus", "");
INSERT INTO incident_type (id, descr) VALUES ("Corporative Nets Attack", "");
INSERT INTO incident_type (id, descr) VALUES ("Policy Violation", "");
INSERT INTO incident_type (id, descr) VALUES ("Security Weakness", "");
INSERT INTO incident_type (id, descr) VALUES ("Net Performance", "");
INSERT INTO incident_type (id, descr) VALUES ("Applications and Systems Failures", "");
INSERT INTO incident_type (id, descr) VALUES ("Anomalies", "");

-- Add Level to metric incidents
ALTER TABLE incident_metric change COLUMN  metric_type metric_type ENUM ('Compromise', 'Attack', 'Level') NOT NULL DEFAULT 'Compromise';

-- New false type
ALTER TABLE incident_ticket change COLUMN status status ENUM ('Open', 'Closed', 'False') NOT NULL DEFAULT 'Open';

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4001, 804, NULL, NULL, 'osiris: LOG_ID_SCHEDULER_INFO');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4001, 703, NULL, NULL, 'osiris: LOG_ID_NOTIFY_INFO');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4001, 304, NULL, NULL, 'osiris: LOG_ID_DB_AUTOACCEPT_ERROR');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (4001, 305, NULL, NULL, 'osiris: LOG_ID_DB_AUTOACCEPT');


-- Netgear--
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 1, NULL, NULL, 'Netgear: All ports forwarded');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 2, NULL, NULL, 'Netgear: UDP packet forwarded');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 3, NULL, NULL, 'Netgear: SMTP forwarded');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 4, NULL, NULL, 'Netgear: HTTP forwarded');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 5, NULL, NULL, 'Netgear: HTTPS forwarded');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 6, NULL, NULL, 'Netgear: TCP connection dropped');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 7, NULL, NULL, 'Netgear: IP packet dropped');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 8, NULL, NULL, 'Netgear: UDP packet dropped');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 9, NULL, NULL, 'Netgear: ICMP packet dropped');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 10, NULL, NULL, 'Netgear: Successful administrator login');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1519, 11, NULL, NULL, 'Netgear: Administrator login fail');--

-- Juniper Netscreen--

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1520, 1, NULL, NULL, 'Netscreen: Accepted ');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1520, 2, NULL, NULL, 'Netscreen: Packet Dropped');

-- Alert -> Event migration
ALTER TABLE incident change COLUMN ref ref ENUM ('Alarm', 'Event', 'Metric') NOT NULL DEFAULT 'Alarm' after date;
UPDATE incident SET ref='Event', date=date WHERE ref='';

RENAME TABLE alert TO event;
ALTER TABLE event CHANGE alert_condition event_condition INTEGER;
RENAME TABLE backlog_alert TO backlog_event;
ALTER TABLE backlog_event CHANGE alert_id event_id BIGINT NOT NULL;
ALTER TABLE backlog_event DROP PRIMARY KEY;
ALTER TABLE backlog_event ADD PRIMARY KEY (backlog_id, event_id);
ALTER TABLE alarm CHANGE alert_id event_id BIGINT NOT NULL;
ALTER TABLE alarm DROP PRIMARY KEY;
ALTER TABLE alarm ADD PRIMARY KEY (backlog_id, event_id);
-- In case you already had added incident_alert:
-- RENAME TABLE incident_alert TO incident_event;

-- Incident TAGs --
DROP TABLE IF EXISTS incident_tag_descr;
CREATE TABLE incident_tag_descr (
        id INT(11) NOT NULL,
        name VARCHAR(64),
        descr TEXT,
        PRIMARY KEY(id)
) ENGINE=INNODB;

DROP TABLE IF EXISTS incident_tag;
CREATE TABLE incident_tag (
        tag_id INT(11) NOT NULL REFERENCES incident_tags_descr(id),
        incident_id INT(11) NOT NULL REFERENCES incident(id),
        PRIMARY KEY (tag_id, incident_id)
) ENGINE=INNODB;

DROP TABLE IF EXISTS incident_tag_descr_seq;
CREATE TABLE incident_tag_descr_seq (
        id INT NOT NULL
);
INSERT INTO incident_tag_descr_seq VALUES (0);

--
-- New feature: Email subscription to incidents
-- ("Incident -> copy" column is now deprecated)
--
DROP TABLE IF EXISTS incident_subscrip;
CREATE TABLE incident_subscrip (
	login VARCHAR(64) NOT NULL REFERENCES users(login),
	incident_id INT(11) NOT NULL REFERENCES incident(id),
	PRIMARY KEY (login, incident_id)
) ENGINE=INNODB;

ALTER TABLE incident_ticket DROP copy;

--
-- New fields for Incident
--

-- These fields needs the new classes/Check_install.php script
-- (it's automatically called from www/session/login.php
-- each new login to the system)
ALTER TABLE incident ADD status ENUM ('Open', 'Closed') NOT NULL DEFAULT 'Open';
ALTER TABLE incident ADD last_update TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00';
ALTER TABLE incident ADD in_charge VARCHAR(64) NOT NULL;

ALTER TABLE incident ADD event_start TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00';
ALTER TABLE incident ADD event_end TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00';

--
-- Change to sequences on Incident Tickets
--
DROP TABLE IF EXISTS incident_ticket_seq; 
CREATE TABLE incident_ticket_seq (
	id INT NOT NULL
);
INSERT INTO incident_ticket_seq (id)
	SELECT max(id)+1 FROM incident_ticket;
ALTER TABLE incident_ticket MODIFY id INTEGER NOT NULL;

--
-- Fix TIMESTAMP MySQL auto-update behavior
-- http://dev.mysql.com/doc/refman/5.0/en/timestamp-4-1.html
--
ALTER TABLE incident MODIFY date TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00';
ALTER TABLE incident_ticket MODIFY date TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00';

--
-- Migration from status FALSE to Tags
--
UPDATE incident_ticket SET status='Closed' WHERE status='False';
ALTER TABLE incident_ticket MODIFY status ENUM ('Open', 'Closed') NOT NULL DEFAULT 'Open';


--
-- Mac changes anomalies
--
ALTER TABLE host_mac DROP PRIMARY KEY;
ALTER TABLE host_mac ADD PRIMARY KEY (ip, date, sensor);
ALTER TABLE host_mac ADD interface varchar(64) NOT NULL after sensor;
ALTER TABLE host_mac DROP  previous;
ALTER TABLE host_mac DROP anom;

--
-- POlicy support to not store some events in database
--
ALTER TABLE policy ADD store BOOLEAN NOT NULL DEFAULT '1' AFTER descr;



-- vim:ts=4 sts=4 tw=79 expandtab: 
