
/* ======== config ======== */
DROP TABLE IF EXISTS config;
CREATE TABLE config (
    conf    varchar(255) NOT NULL,
    value   varchar(255),
    PRIMARY KEY (conf)
);


/* ======== hosts & nets ======== */
DROP TABLE IF EXISTS host;
CREATE TABLE host (
  ip                varchar(15) UNIQUE NOT NULL,
  hostname          varchar(128) NOT NULL,
  asset             smallint(6) NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL,
  persistence       int NOT NULL,
  nat               varchar(15),
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (ip)
);


DROP TABLE IF EXISTS net;
CREATE TABLE net (
  name              varchar(128) UNIQUE NOT NULL,
  ips               varchar(255) NOT NULL,
  priority          int NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL,
  persistence       int NOT NULL,
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (name)
);


DROP TABLE IF EXISTS net_group;
CREATE TABLE net_group (
  name              varchar(128) UNIQUE NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (name)
);

DROP TABLE IF EXISTS net_group_scan;
CREATE TABLE net_group_scan (
    net_group_name               varchar(128) NOT NULL,
      plugin_id       INTEGER NOT NULL,
      plugin_sid      INTEGER NOT NULL,
      PRIMARY KEY (net_group_name, plugin_id, plugin_sid)
);        
         
DROP TABLE IF EXISTS net_group_reference;
CREATE TABLE net_group_reference (
    net_group_name        varchar(128) NOT NULL,
    net_name     varchar(128) NOT NULL,
    PRIMARY KEY     (net_group_name, net_name)
);


/* ======== signatures ======== */
DROP TABLE IF EXISTS signature_group;
CREATE TABLE signature_group (
  name              varchar(64) NOT NULL,
  descr             varchar(255),
  PRIMARY KEY       (name)
);

DROP TABLE IF EXISTS signature;
CREATE TABLE signature (
  name              varchar(64) NOT NULL,
  PRIMARY KEY       (name)
);

DROP TABLE IF EXISTS signature_group_reference;
CREATE TABLE signature_group_reference (
    sig_group_name    varchar(64) NOT NULL,
    sig_name          varchar(64) NOT NULL,
    PRIMARY KEY      (sig_group_name, sig_name)
);

/* ======== ports ======== */
DROP TABLE IF EXISTS port_group;
CREATE TABLE port_group (
    name            varchar(64) NOT NULL,
    descr           varchar(255),
    PRIMARY KEY     (name)
);

DROP TABLE IF EXISTS port;
CREATE TABLE port (
  port_number       int NOT NULL,
  protocol_name     varchar(12) NOT NULL,
  service           varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (port_number,protocol_name)
);


DROP TABLE IF EXISTS port_group_reference;
CREATE TABLE port_group_reference (
    port_group_name varchar(64) NOT NULL,
    port_number     int NOT NULL,
    protocol_name   varchar(12) NOT NULL,
    PRIMARY KEY     (port_group_name, port_number, protocol_name)
);


INSERT INTO port_group (name, descr) VALUES ('ANY', 'Any port');
INSERT INTO port_group_reference (port_group_name, port_number, protocol_name) VALUES ('ANY', 0, 'tcp');
INSERT INTO port_group_reference (port_group_name, port_number, protocol_name) VALUES ('ANY', 0, 'udp');
INSERT INTO port_group_reference (port_group_name, port_number, protocol_name) VALUES ('ANY', 0, 'icmp');


DROP TABLE IF EXISTS protocol;
CREATE TABLE protocol (
  id                int NOT NULL,
  name              varchar(24) NOT NULL,
  alias             varchar(24),
  descr             varchar(255) NOT NULL,
  PRIMARY KEY       (id)
);


/* ======== sensors ======== */
DROP TABLE IF EXISTS sensor;
CREATE TABLE sensor (
    name            varchar(64) NOT NULL,
    ip              varchar(15) NOT NULL,
    priority        smallint NOT NULL,
    port            int NOT NULL,
    connect         smallint NOT NULL,
/*    sig_group_id    int  NOT NULL, */
    descr           varchar(255) NOT NULL,
    PRIMARY KEY     (name)
);

DROP TABLE IF EXISTS sensor_interfaces;
CREATE TABLE sensor_interfaces (
    sensor  varchar(64) NOT NULL,
    interface varchar(64) NOT NULL,
    name    varchar(255) NOT NULL,
    main    int NOT NULL,
    PRIMARY KEY (sensor, interface)
);


DROP TABLE IF EXISTS host_sensor_reference;
CREATE TABLE host_sensor_reference (
    host_ip         varchar(15) NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (host_ip, sensor_name)
);

DROP TABLE IF EXISTS net_sensor_reference;
CREATE TABLE net_sensor_reference (
    net_name        varchar(128) NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (net_name, sensor_name)
);


/* ======== policy ======== */
DROP TABLE IF EXISTS policy;
CREATE TABLE policy (
    id              int NOT NULL auto_increment,
    priority        smallint NOT NULL,
    descr           varchar(255),
    PRIMARY KEY     (id)
);

DROP TABLE IF EXISTS policy_port_reference;
CREATE TABLE policy_port_reference (
    policy_id       int NOT NULL,
    port_group_name varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, port_group_name)
);

DROP TABLE IF EXISTS policy_host_reference;
CREATE TABLE policy_host_reference (
    policy_id       int NOT NULL,
    host_ip         varchar(15) NOT NULL,
    direction       enum ('source', 'dest') NOT NULL,
    PRIMARY KEY (policy_id, host_ip, direction)
);

DROP TABLE IF EXISTS policy_net_reference;
CREATE TABLE policy_net_reference (
    policy_id       int NOT NULL,
    net_name        varchar(128) NOT NULL,
    direction       enum ('source', 'dest') NOT NULL,
    PRIMARY KEY (policy_id, net_name, direction)
);

DROP TABLE IF EXISTS policy_sensor_reference;
CREATE TABLE policy_sensor_reference (
    policy_id       int NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, sensor_name)
);

DROP TABLE IF EXISTS policy_sig_reference;
CREATE TABLE policy_sig_reference (
    policy_id       int NOT NULL,
    sig_group_name  varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, sig_group_name)
);

DROP TABLE IF EXISTS policy_time;
CREATE TABLE policy_time (
    policy_id       int NOT NULL,
    begin_hour      smallint NOT NULL,
    end_hour        smallint NOT NULL,
    begin_day       smallint NOT NULL,
    end_day         smallint NOT NULL,
    PRIMARY KEY     (policy_id)
);


/* ======== qualification ======== */
DROP TABLE IF EXISTS host_qualification;
CREATE TABLE host_qualification (
    host_ip         varchar(15) NOT NULL,
    compromise      int NOT NULL DEFAULT 1,
    attack          int NOT NULL DEFAULT 1,
    PRIMARY KEY     (host_ip)
);

DROP TABLE IF EXISTS net_qualification;
CREATE TABLE net_qualification (
    net_name        varchar(128) NOT NULL,
    compromise      int NOT NULL DEFAULT 1,
    attack          int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net_name)
);

DROP TABLE IF EXISTS host_vulnerability;
CREATE TABLE host_vulnerability (
    ip              varchar(15) NOT NULL,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (ip)
);

DROP TABLE IF EXISTS net_vulnerability;
CREATE TABLE net_vulnerability (
    net             varchar(128) NOT NULL,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net)
);

DROP TABLE IF EXISTS control_panel;
CREATE TABLE control_panel (
    id              varchar(128) NOT NULL,
    rrd_type        varchar(6) NOT NULL DEFAULT 'host',
    time_range      varchar(5) NOT NULL DEFAULT 'day',
    max_c           int NOT NULL,
    max_a           int NOT NULL,
    max_c_date      datetime,
    max_a_date      datetime,
    c_sec_level     float NOT NULL,
    a_sec_level     float NOT NULL,
    PRIMARY KEY     (id, rrd_type, time_range)
);

--
-- Table: Host Mac. 
--
DROP TABLE IF EXISTS host_mac;
CREATE TABLE host_mac (
	ip		INTEGER UNSIGNED NOT NULL,
	mac	        VARCHAR(255) NOT NULL,
	previous	VARCHAR(255) NOT NULL,
	date            DATETIME NOT NULL,
	vendor		VARCHAR(255),
    anom        int NOT NULL DEFAULT 0,
	PRIMARY KEY     (ip)
);

--
-- Table: Host OS.
--
DROP TABLE IF EXISTS host_os;
CREATE TABLE host_os (
	ip		INTEGER UNSIGNED NOT NULL,
	os		VARCHAR(255) NOT NULL,
	previous	VARCHAR(255) NOT NULL,
	date		DATETIME NOT NULL,
    anom        int NOT NULL DEFAULT 0,
	PRIMARY KEY	(ip)
);

DROP TABLE IF EXISTS host_services;
CREATE TABLE host_services (
	ip		INTEGER UNSIGNED NOT NULL,
    port    int NOT NULL,
    protocol int NOT NULL,
    service varchar(128),
    service_type varchar(128),
    version varchar(255) NOT NULL DEFAULT "unknown",
	date		DATETIME NOT NULL,
    origin  int NOT NULL DEFAULT 0,
    PRIMARY KEY (ip, port, protocol, version, date)
);

DROP TABLE IF EXISTS host_netbios;
CREATE TABLE host_netbios (
    ip      varchar(15) NOT NULL,
    name    varchar(128) NOT NULL,
    wgroup  varchar(128),
    PRIMARY KEY (ip)
);

DROP TABLE IF EXISTS rrd_config;
CREATE TABLE rrd_config (
    profile     VARCHAR(64) NOT NULL,
    rrd_attrib  VARCHAR(60) NOT NULL,
    threshold   INTEGER UNSIGNED NOT NULL,
    priority    INTEGER UNSIGNED NOT NULL,
    alpha       FLOAT UNSIGNED  NOT NULL,
    beta        FLOAT UNSIGNED NOT NULL,
    persistence INTEGER UNSIGNED NOT NULL,
    enable      TINYINT DEFAULT 1,
    description TEXT,
    PRIMARY KEY (profile, rrd_attrib)
);


DROP TABLE IF EXISTS rrd_anomalies;
CREATE TABLE rrd_anomalies (
    ip                      varchar(15) NOT NULL,
    what                    varchar(100) NOT NULL,
    count                   int NOT NULL,
    anomaly_time            varchar(40) NOT NULL,
    range                   varchar(30) NOT NULL,
    over                    int NOT NULL,
    acked                   int DEFAULT 0
);


DROP TABLE IF EXISTS rrd_anomalies_global;
CREATE TABLE rrd_anomalies_global (
    what                    varchar(100) NOT NULL,
    count                   int NOT NULL,
    anomaly_time            varchar(40) NOT NULL,
    range                   varchar(30) NOT NULL,
    over                    int NOT NULL,
    acked                   int DEFAULT 0
);

--
-- Table: Category
--
DROP TABLE IF EXISTS category;
CREATE TABLE category (
	id		INTEGER NOT NULL,
	name		VARCHAR (100) NOT NULL,
	PRIMARY KEY (id)
);

--
-- Table: Classification
--
DROP TABLE IF EXISTS classification;
CREATE TABLE classification (
	id		INTEGER NOT NULL,
	name		VARCHAR (100) NOT NULL,
	description	TEXT,
	priority	INTEGER,
	PRIMARY KEY (id)
);

--
-- Table: Plugin
--
DROP TABLE IF EXISTS plugin;
CREATE TABLE plugin (
	id		INTEGER NOT NULL,
	type		SMALLINT NOT NULL,
	name		VARCHAR (100) NOT NULL,
	description	TEXT,
	PRIMARY KEY (id)
);

--
-- Table: Plugin Sid
--
DROP TABLE IF EXISTS plugin_sid;
CREATE TABLE plugin_sid (
	plugin_id	INTEGER NOT NULL,
	sid		INTEGER NOT NULL,
	category_id	INTEGER,
	class_id	INTEGER,
	reliability	INTEGER DEFAULT 1,
	priority	INTEGER DEFAULT 1,
	name		VARCHAR (255) NOT NULL,
	PRIMARY KEY (plugin_id, sid)
);

--
-- Table: Alert
--

DROP TABLE IF EXISTS alert;
CREATE TABLE alert (
        id              BIGINT NOT NULL AUTO_INCREMENT,
        timestamp       TIMESTAMP NOT NULL,
        sensor          TEXT NOT NULL,
        interface       TEXT NOT NULL,
        type            INTEGER NOT NULL,
        plugin_id       INTEGER NOT NULL,
        plugin_sid      INTEGER NOT NULL,
        protocol        INTEGER,
        src_ip          INTEGER UNSIGNED,
        dst_ip          INTEGER UNSIGNED,
        src_port        INTEGER,
        dst_port        INTEGER,
        alert_condition       INTEGER,
        value           TEXT,
        time_interval   INTEGER,
        absolute        TINYINT,
        priority        INTEGER DEFAULT 1,
        reliability     INTEGER DEFAULT 1,
        asset_src       INTEGER DEFAULT 1,
        asset_dst       INTEGER DEFAULT 1,
        risk_a          INTEGER DEFAULT 1,
        risk_c          INTEGER DEFAULT 1,
        alarm           TINYINT DEFAULT 1,
        snort_sid       INTEGER UNSIGNED,
        snort_cid       INTEGER UNSIGNED,
        PRIMARY KEY (id)
);

--
-- Table: Backlog
--
DROP TABLE IF EXISTS backlog;
CREATE TABLE backlog (
	id		BIGINT NOT NULL AUTO_INCREMENT,
	directive_id	INTEGER NOT NULL,
	timestamp	TIMESTAMP NOT NULL,
	matched		TINYINT,
	PRIMARY KEY (id)
);

--
-- Table: Backlog Alert
--
DROP TABLE IF EXISTS backlog_alert;
CREATE TABLE backlog_alert (
	backlog_id	BIGINT NOT NULL,
	alert_id	BIGINT NOT NULL,
	time_out	INTEGER,
	occurrence	INTEGER,
	rule_level	INTEGER,
	matched		TINYINT,
	PRIMARY KEY (backlog_id, alert_id)
);

--
-- Table: Alarm
--
DROP TABLE IF EXISTS alarm;
CREATE TABLE alarm (
        backlog_id      BIGINT NOT NULL,
        alert_id        BIGINT NOT NULL,
        timestamp       TIMESTAMP NOT NULL,
        plugin_id       INTEGER NOT NULL,
        plugin_sid      INTEGER NOT NULL,
        protocol        INTEGER,
        src_ip          INTEGER UNSIGNED,
        dst_ip          INTEGER UNSIGNED,
        src_port        INTEGER,
        dst_port        INTEGER,
        risk            INTEGER,
        snort_sid       INTEGER UNSIGNED,
        snort_cid       INTEGER UNSIGNED,
        PRIMARY KEY (backlog_id, alert_id)
);

--
-- Table: plugin_reference
--
DROP TABLE IF EXISTS plugin_reference;
CREATE TABLE plugin_reference (
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	reference_id	INTEGER NOT NULL,
	reference_sid	INTEGER NOT NULL,
	PRIMARY KEY (plugin_id, plugin_sid, reference_id, reference_sid)
);

--
-- Table: Host plugin sid
--
DROP TABLE IF EXISTS host_plugin_sid;
CREATE TABLE host_plugin_sid (
	host_ip         INTEGER UNSIGNED NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	PRIMARY KEY (host_ip, plugin_id, plugin_sid)
);

--
-- Table: Host scan
--
DROP TABLE IF EXISTS host_scan;
CREATE TABLE host_scan (
	host_ip         INTEGER UNSIGNED NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	PRIMARY KEY (host_ip, plugin_id, plugin_sid)
);

--
-- Table: Net scan
--
DROP TABLE IF EXISTS net_scan;
CREATE TABLE net_scan (
    net_name               varchar(128) NOT NULL,
      plugin_id       INTEGER NOT NULL,
      plugin_sid      INTEGER NOT NULL,
      PRIMARY KEY (net_name, plugin_id, plugin_sid)
);



---
--- Table: Users
---
DROP TABLE IF EXISTS users;
CREATE TABLE users (
    login   varchar(64)  NOT NULL,
    name    varchar(128) NOT NULL,
    pass    varchar(41)  NOT NULL,
    allowed_nets    varchar(255) DEFAULT '' NOT NULL,
    PRIMARY KEY (login)
);

--
-- Data: User
--
INSERT INTO users (login, name, pass) VALUES ('admin', 'OSSIM admin', '21232f297a57a5a743894a0e4a801fc3');



--
-- Table: incident
--
DROP TABLE IF EXISTS incident;
CREATE TABLE incident (
    id          INTEGER NOT NULL AUTO_INCREMENT,
    title       VARCHAR(128) NOT NULL,
    date        TIMESTAMP NOT NULL,
    ref         ENUM ('Alarm', 'Metric', 'Hardware', 'Install') NOT NULL DEFAULT 'Alarm',
    family      ENUM ('OSSIM', 'Hardware', 'Install') NOT NULL DEFAULT 'OSSIM',
    priority    INTEGER NOT NULL,
    PRIMARY KEY (id)
);

--
-- Table: incident ticket
--
DROP TABLE IF EXISTS incident_ticket;
CREATE TABLE incident_ticket (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    date            TIMESTAMP NOT NULL,
    status          ENUM ('Open', 'Closed') NOT NULL DEFAULT 'Open',
    priority        INTEGER NOT NULL,
    users            VARCHAR(64) NOT NULL,
    description     TEXT,
    action          TEXT,
    in_charge       VARCHAR(64),
    transferred     VARCHAR(64),
    copy            VARCHAR(64),
    PRIMARY KEY (id, incident_id)
);

--
-- Table: incident alarm
--
DROP TABLE IF EXISTS incident_alarm;
CREATE TABLE incident_alarm (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    src_ips         VARCHAR(255) NOT NULL,
    src_ports       VARCHAR(255) NOT NULL,
    dst_ips         VARCHAR(255) NOT NULL,
    dst_ports       VARCHAR(255) NOT NULL,
    PRIMARY KEY (id, incident_id)
);

--
-- Table: incident metric
--
DROP TABLE IF EXISTS incident_metric;
CREATE TABLE incident_metric (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    target          VARCHAR(255) NOT NULL,
    metric_type     ENUM ('Compromise', 'Attack') NOT NULL DEFAULT 'Compromise',
    metric_value    INTEGER NOT NULL,
    PRIMARY KEY (id, incident_id)
);

DROP TABLE IF EXISTS incident_file;
CREATE TABLE incident_file (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    incident_ticket INTEGER NOT NULL,
    name            VARCHAR(50),
    type            VARCHAR(50),
    content         mediumblob, /* 16Mb */
    PRIMARY KEY (id, incident_id, incident_ticket)
);

--
-- Table: restoredb
--
DROP TABLE IF EXISTS restoredb_log;
CREATE TABLE restoredb_log (
	id		INTEGER NOT NULL AUTO_INCREMENT,
	date		TIMESTAMP,
	pid		INTEGER,
	users		VARCHAR(64),
	data		TEXT,
	status		SMALLINT,
	percent		SMALLINT,
	PRIMARY KEY (id)
);

--
-- HIDS (Osiris) Support
--

DROP TABLE IF EXISTS host_ids;
CREATE TABLE host_ids(
ip              INTEGER UNSIGNED NOT NULL,
date            DATETIME NOT NULL,
hostname        VARCHAR(255) NOT NULL,
sensor          VARCHAR(255) NOT NULL,
sid             INTEGER UNSIGNED NOT NULL,
event_type            VARCHAR(255) NOT NULL,
what            VARCHAR(255) NOT NULL,
target          VARCHAR(255) NOT NULL,
extra_data      VARCHAR(255) NOT NULL,
PRIMARY KEY     (ip,target,sid,date)
);
