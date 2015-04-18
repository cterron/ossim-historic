
/* ======== config ======== */
DROP TABLE IF EXISTS conf;
CREATE TABLE conf (
    recovery        int NOT NULL,
    threshold       int NOT NULL,
    graph_threshold int NOT NULL,
    bar_length_left int NOT NULL,
    bar_length_right int NOT NULL,
    PRIMARY KEY (recovery, threshold, graph_threshold, 
                 bar_length_left, bar_length_right)
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
  descr             varchar(255),
  PRIMARY KEY       (ip)
);

DROP TABLE IF EXISTS scan;
CREATE TABLE scan (
    ip              varchar(15) UNIQUE NOT NULL,
    active          int NOT NULL,
    PRIMARY KEY     (ip)
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
  descr             varchar(255),
  PRIMARY KEY       (name)
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
    id              varchar(15) NOT NULL,
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
    ip      varchar(15) NOT NULL,
    service varchar(128) NOT NULL,
    version varchar(255) NOT NULL,
    PRIMARY KEY (ip, service, version)
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
    ip          INTEGER UNSIGNED NOT NULL,
    rrd_attrib  VARCHAR(60) NOT NULL,
    threshold   INTEGER UNSIGNED NOT NULL,
    priority    INTEGER UNSIGNED NOT NULL,
    alpha       FLOAT UNSIGNED  NOT NULL,
    beta        FLOAT UNSIGNED NOT NULL,
    persistence INTEGER UNSIGNED NOT NULL,
    descripcion TEXT,
    PRIMARY KEY (ip, rrd_attrib)
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
        condition       INTEGER,
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
        alert_id        BIGINT NOT NULL,
        backlog_id      BIGINT NOT NULL,
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
        PRIMARY KEY (alert_id)
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



