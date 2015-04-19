-- Functions inet_ntoa and inet_aton for Postgres
CREATE OR REPLACE FUNCTION inet_ntoa(bigint) RETURNS text AS '
       SELECT
       ($1/(256*256*256))::text
                ||''.''||
       ($1/(256*256) - $1/(256*256*256)*256)::text
                ||''.''||
       ($1/256 - $1/(256*256)*256)::text
                ||''.''||
       ($1 - $1/256*256)::text;
    ' LANGUAGE SQL;

CREATE OR REPLACE FUNCTION inet_aton(text) RETURNS bigint AS '
    SELECT
        split_part($1,''.'',1)::int8*(256*256*256)+
        split_part($1,''.'',2)::int8*(256*256)+
        split_part($1,''.'',3)::int8*256+
        split_part($1,''.'',4)::int8;
    ' LANGUAGE SQL;

-- Function curdate() for Postgres
CREATE or REPLACE FUNCTION curdate() RETURNS date AS '
    SELECT current_date;'
LANGUAGE SQL;

--  config
DROP TABLE config;
CREATE TABLE config (
    conf    varchar(255) NOT NULL,
    value   varchar(255),
    PRIMARY KEY (conf)
);


--  hosts 
DROP TABLE host;
CREATE TABLE host (
  ip                varchar(15) UNIQUE NOT NULL, 
  hostname          varchar(128) NOT NULL,
  asset             smallint NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL,
  persistence       int NOT NULL,
  nat               varchar(15),
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY (ip)
);


-- net
DROP TABLE net;
CREATE TABLE net (
  name              varchar(128) UNIQUE NOT NULL,
  ips               varchar(255) NOT NULL,
  priority          int NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL DEFAULT 0,
  persistence       int NOT NULL DEFAULT 0,
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (name)
);

-- net_group
DROP TABLE net_group;
CREATE TABLE net_group (
  name              varchar(128) UNIQUE NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (name)
);

--net_group_scan
DROP TABLE net_group_scan;
CREATE TABLE net_group_scan (
   net_group_name  varchar(128) NOT NULL,
   plugin_id       INTEGER NOT NULL,
   plugin_sid      INTEGER NOT NULL,
   PRIMARY KEY (net_group_name, plugin_id, plugin_sid)
);        

--net_group_reference
DROP TABLE net_group_reference;
CREATE TABLE net_group_reference (
   net_group_name       varchar(128) NOT NULL,
   net_name		varchar(128) NOT NULL,
   PRIMARY KEY (net_group_name, net_name)
);


-- /* ======== signatures ======== */
DROP TABLE signature_group;
CREATE TABLE signature_group (
  name              varchar(64) NOT NULL,
  descr             varchar(255),
  PRIMARY KEY       (name)
);

DROP TABLE signature;
CREATE TABLE signature (
  name              varchar(64) NOT NULL,
  PRIMARY KEY       (name)
);

DROP TABLE signature_group_reference;
CREATE TABLE signature_group_reference (
    sig_group_name    varchar(64) NOT NULL,
    sig_name          varchar(64) NOT NULL,
    PRIMARY KEY      (sig_group_name, sig_name)
);

-- /* ======== ports ======== */
DROP TABLE port_group;
CREATE TABLE port_group (
    name            varchar(64) NOT NULL,
    descr           varchar(255),
    PRIMARY KEY     (name)
);

DROP TABLE port;
CREATE TABLE port (
  port_number       int NOT NULL,
  protocol_name     varchar(12) NOT NULL,
  service           varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (port_number,protocol_name)
);


DROP TABLE port_group_reference;
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


DROP TABLE protocol;
CREATE TABLE protocol (
  id                int NOT NULL,
  name              varchar(24) NOT NULL,
  alias             varchar(24),
  descr             varchar(255) NOT NULL,
  PRIMARY KEY       (id)
);


-- /* ======== sensors ======== */
DROP TABLE sensor;
CREATE TABLE sensor (
    name            varchar(64) NOT NULL,
    ip              varchar(15) NOT NULL,
    priority        smallint NOT NULL,
    port            int NOT NULL,
    connect         smallint NOT NULL,
-- /*    sig_group_id    int  NOT NULL, */
    descr           varchar(255) NOT NULL,
    PRIMARY KEY     (name)
);

DROP TABLE sensor_interfaces;
CREATE TABLE sensor_interfaces (
    sensor  varchar(64) NOT NULL,
    interface varchar(64) NOT NULL,
    name    varchar(255) NOT NULL,
    main    int NOT NULL,
    PRIMARY KEY (sensor, interface)
);

DROP TABLE host_sensor_reference;
CREATE TABLE host_sensor_reference (
    host_ip         varchar(15) NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (host_ip, sensor_name)
);

DROP TABLE net_sensor_reference;
CREATE TABLE net_sensor_reference (
    net_name        varchar(128) NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (net_name, sensor_name)
);


-- /* ======== policy ======== */
DROP TABLE policy;
CREATE TABLE policy (
    id              SERIAL,
    priority        smallint NOT NULL,
    descr           varchar(255),
    PRIMARY KEY     (id)
);

DROP TABLE policy_port_reference;
CREATE TABLE policy_port_reference (
    policy_id       int NOT NULL,
    port_group_name varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, port_group_name)
);

DROP TABLE policy_host_reference;
CREATE TABLE policy_host_reference (
    policy_id       int NOT NULL,
    host_ip         varchar(15) NOT NULL,
    direction       varchar(7) check (direction in ('source', 'dest')) NOT NULL,
    PRIMARY KEY (policy_id, host_ip, direction)
);

DROP TABLE policy_net_reference;
CREATE TABLE policy_net_reference (
    policy_id       int NOT NULL,
    net_name        varchar(64) NOT NULL,
    direction       varchar(7) check (direction in ('source', 'dest')) NOT NULL,
    PRIMARY KEY (policy_id, net_name, direction)
);

DROP TABLE policy_sensor_reference;
CREATE TABLE policy_sensor_reference (
    policy_id       int NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, sensor_name)
);

DROP TABLE policy_sig_reference;
CREATE TABLE policy_sig_reference (
    policy_id       int NOT NULL,
    sig_group_name  varchar(64) NOT NULL,
    PRIMARY KEY     (policy_id, sig_group_name)
);

DROP TABLE policy_time;
CREATE TABLE policy_time (
    policy_id       int NOT NULL,
    begin_hour      smallint NOT NULL,
    end_hour        smallint NOT NULL,
    begin_day       smallint NOT NULL,
    end_day         smallint NOT NULL,
    PRIMARY KEY     (policy_id)
);


DROP TABLE policy_plugin_reference;
CREATE TABLE policy_plugin_reference (
    policy_id       int NOT NULL,
    plugin_id		INTEGER NOT NULL,
    PRIMARY KEY     (policy_id, plugin_id)
);

DROP TABLE policy_plugin_sid_reference;
CREATE TABLE policy_plugin_sid_reference (
    policy_id       int NOT NULL,
    plugin_sid      INTEGER NOT NULL,
    PRIMARY KEY     (policy_id, plugin_sid)
);



/* ======== actions ======== */
DROP SEQUENCE action_id_seq;
CREATE SEQUENCE action_id_seq;

DROP TABLE action;
CREATE TABLE action (
    id              BIGINT DEFAULT nextval('action_id_seq') NOT NULL, 
    action_type     varchar(100) NOT NULL,
    descr           varchar(255) NOT NULL,
    PRIMARY KEY     (id)
);

DROP TABLE action_type;
CREATE TABLE action_type (
    _type            varchar (100) NOT NULL,
    descr           varchar (255) NOT NULL,
    PRIMARY KEY     (_type)
);

INSERT INTO action_type (_type, descr) VALUES ('email', 'send an email message');
INSERT INTO action_type (_type, descr) VALUES ('exec', 'execute an external program');


DROP TABLE action_email;
CREATE TABLE action_email (
   action_id       int NOT NULL,
   _from           varchar(100) NOT NULL,
   _to             varchar(100) NOT NULL,
   subject         varchar(255) NOT NULL,
   message         varchar(255) NOT NULL,
   PRIMARY KEY     (action_id)
);

DROP TABLE action_exec;
CREATE TABLE action_exec (
   action_id       int NOT NULL,
   command         varchar(255) NOT NULL,
   PRIMARY KEY     (action_id)
);

/* ======== response ========== */
DROP SEQUENCE response_id_seq;
CREATE SEQUENCE response_id_seq;

DROP TABLE response;
CREATE TABLE response (
    id          BIGINT DEFAULT nextval('response_id_seq') NOT NULL,
    descr       varchar(255),
    PRIMARY KEY (id)
);


--
-- Table: response_host
--
DROP TABLE response_host;
CREATE TABLE response_host (
    response_id int NOT NULL,
    host        varchar(15),
    _type       VARCHAR(6) check (_type in ('source', 'dest', 'sensor')) NOT NULL DEFAULT 'source',
    PRIMARY KEY (response_id, host, _type)
);

--
-- Table: response_net
--
DROP TABLE response_net;
CREATE TABLE response_net (
    response_id int NOT NULL,
    net         varchar(255),
    _type       VARCHAR(6) check (_type in ('source', 'dest')) NOT NULL DEFAULT 'source',
    PRIMARY KEY (response_id, net, _type)
);


--
-- Table: response_action
--
DROP TABLE response_action;
CREATE TABLE response_action (
	response_id		INTEGER NOT NULL,
     	action_id		INTEGER NOT NULL,
	PRIMARY KEY (response_id, action_id)
);

--
-- Table: response_plugin
--
DROP TABLE response_plugin;
CREATE TABLE response_plugin (
	response_id	INTEGER NOT NULL,
	plugin_id	INTEGER NOT NULL,
	PRIMARY KEY (response_id, plugin_id)
);

--
-- Table: response_port
--
DROP TABLE response_port;
CREATE TABLE response_port (
	response_id	INTEGER NOT NULL,
	port		INTEGER NOT NULL,
	_type		VARCHAR(6) check (_type in ('source', 'dest')) NOT NULL DEFAULT 'source',
	PRIMARY KEY (response_id, port, _type)
);


-- /* ======== qualification ======== */
DROP TABLE host_qualification;
CREATE TABLE host_qualification (
    host_ip         varchar(15) NOT NULL,
    compromise      int NOT NULL DEFAULT 1,
    attack          int NOT NULL DEFAULT 1,
    PRIMARY KEY     (host_ip)
);

DROP TABLE net_qualification;
CREATE TABLE net_qualification (
    net_name        varchar(128) NOT NULL,
    compromise      int NOT NULL DEFAULT 1,
    attack          int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net_name)
);

DROP TABLE host_vulnerability;
CREATE TABLE host_vulnerability (
    ip              varchar(15) NOT NULL,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (ip)
);

DROP TABLE net_vulnerability;
CREATE TABLE net_vulnerability (
    net             varchar(128) NOT NULL,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net)
);
DROP TABLE control_panel;
CREATE TABLE control_panel (
    id              varchar(128) NOT NULL,
    rrd_type        varchar(6) NOT NULL DEFAULT 'host',
    time_range      varchar(5) NOT NULL DEFAULT 'day',
    max_c           int NOT NULL,
    max_a           int NOT NULL,
    max_c_date      TIMESTAMP,
    max_a_date      TIMESTAMP,
    c_sec_level     float NOT NULL,
    a_sec_level     float NOT NULL,
    PRIMARY KEY     (id, rrd_type, time_range)
);

--
-- Table: Host Mac. 
--
DROP TABLE host_mac;
CREATE TABLE host_mac (
	ip		INT8 NOT NULL,
	mac	        VARCHAR(255) NOT NULL,
	previous	VARCHAR(255) NOT NULL,
	date            TIMESTAMP NOT NULL,
	vendor		VARCHAR(255),
        anom        	int NOT NULL DEFAULT 0,
	PRIMARY KEY     (ip)
);

--
-- Table: Host OS.
--
DROP TABLE host_os;
CREATE TABLE host_os (
	ip		INT8 NOT NULL,
	os		VARCHAR(255) NOT NULL,
	previous	VARCHAR(255) NOT NULL,
	date		TIMESTAMP NOT NULL,
    	anom        int NOT NULL DEFAULT 0,
	PRIMARY KEY	(ip)
);

DROP TABLE host_services;
CREATE TABLE host_services (
    ip		INT8 NOT NULL,
    port    int NOT NULL,
    protocol   int NOT NULL,
    service varchar(128),
    service_type varchar(128),
    version varchar(255) NOT NULL DEFAULT 'unknown',
    date    TIMESTAMP NOT NULL,
    origin INT8 NOT NULL DEFAULT 0,
    PRIMARY KEY (ip, port, protocol, version, date)
);

DROP TABLE host_netbios;
CREATE TABLE host_netbios (
    ip      varchar(15) NOT NULL,
    name    varchar(128) NOT NULL,
    wgroup  varchar(128),
    PRIMARY KEY (ip)
);

DROP TABLE rrd_config;
CREATE TABLE rrd_config (
    profile     VARCHAR(64) NOT NULL,
    rrd_attrib  VARCHAR(60) NOT NULL,
    threshold   INT8 NOT NULL,
    priority    INT8 NOT NULL,
    alpha       FLOAT  NOT NULL,
    beta        FLOAT NOT NULL,
    persistence INT8 NOT NULL,
    enable  INT8 DEFAULT 1,
    description TEXT,
    PRIMARY KEY (profile, rrd_attrib)
);

DROP TABLE rrd_anomalies;
CREATE TABLE rrd_anomalies (
    ip                      varchar(15) NOT NULL,
    what                    varchar(100) NOT NULL,
    count                   int NOT NULL,
    anomaly_time            varchar(40) NOT NULL,
    range                   varchar(30) NOT NULL,
    over                    int NOT NULL,
    acked                   int DEFAULT 0
);

DROP TABLE rrd_anomalies_global;
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
DROP TABLE category;
CREATE TABLE category (
	id		INTEGER NOT NULL,
	name		VARCHAR (100) NOT NULL,
	PRIMARY KEY (id)
);

--
-- Table: Classification
--
DROP TABLE classification;
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
DROP TABLE plugin;
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
DROP TABLE plugin_sid;
CREATE TABLE plugin_sid (
	plugin_id	INTEGER NOT NULL,
	sid		INTEGER NOT NULL,
	category_id	INTEGER,
	class_id	INTEGER,
	reliability	INTEGER DEFAULT 1,
	priority	INTEGER DEFAULT 1,
	name		VARCHAR (1500) NOT NULL,
	PRIMARY KEY (plugin_id, sid)
);

--
-- Table: Alert
--
DROP SEQUENCE alert_id_seq;
CREATE SEQUENCE alert_id_seq;

DROP TABLE alert;
CREATE TABLE alert (
	id		BIGINT PRIMARY KEY DEFAULT nextval('alert_id_seq'),
	timestamp	TIMESTAMP NOT NULL,
	sensor		TEXT NOT NULL,
	interface	TEXT NOT NULL,
	type		INTEGER NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	protocol	INTEGER,
	src_ip		INT8,
	dst_ip		INT8,
	src_port	INTEGER,
	dst_port	INTEGER,
	alert_condition	INTEGER,
	value		TEXT,
	time_interval	INTEGER,
	absolute	BOOLEAN,
	priority	INTEGER DEFAULT 1,
	reliability	INTEGER DEFAULT 1,
	asset_src	INTEGER DEFAULT 1,
	asset_dst	INTEGER DEFAULT 1,
	risk_a		INTEGER DEFAULT 1,
	risk_c		INTEGER DEFAULT 1,
	alarm       INTEGER DEFAULT 1,
	snort_sid	INT8,
	snort_cid       INT8
);

--
-- Table: Backlog
--
DROP SEQUENCE backlog_id_seq;
CREATE SEQUENCE backlog_id_seq;

DROP TABLE backlog;
CREATE TABLE backlog (
	id		BIGINT PRIMARY KEY DEFAULT nextval('backlog_id_seq'),
	directive_id	INTEGER NOT NULL,
	timestamp	TIMESTAMP NOT NULL,
	matched		BOOLEAN
);

--
-- Table: Backlog Alert
--
DROP TABLE backlog_alert;
CREATE TABLE backlog_alert (
	backlog_id	BIGINT NOT NULL,
	alert_id	BIGINT NOT NULL,
	time_out	INTEGER,
	occurrence	INTEGER,
	rule_level	INTEGER,
	matched		BOOLEAN,
	PRIMARY KEY (backlog_id, alert_id)
);

--
-- Table: Alarm
--

DROP TABLE alarm;
CREATE TABLE alarm (
	backlog_id	BIGINT NOT NULL,
	alert_id	BIGINT NOT NULL,
	timestamp	TIMESTAMP NOT NULL,
	status		VARCHAR(7) check (status in ('open', 'closed')) DEFAULT 'open', 
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	protocol	INTEGER,
	src_ip		INT8,
	dst_ip		INT8,
	src_port	INTEGER,
	dst_port	INTEGER,
	risk		INTEGER,
	snort_sid	INT8,
	snort_cid	INT8,
	PRIMARY KEY (backlog_id, alert_id)
);

--
-- Table: plugin_reference
--
DROP TABLE plugin_reference;
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
DROP TABLE host_plugin_sid;
CREATE TABLE host_plugin_sid (
	host_ip         INT8 NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	PRIMARY KEY (host_ip, plugin_id, plugin_sid)
);

--
-- Table: Host scan
--
DROP TABLE host_scan;
CREATE TABLE host_scan (
	host_ip         INT8 NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	PRIMARY KEY (host_ip, plugin_id, plugin_sid)
);

--
-- Table: Net scan
--
DROP TABLE net_scan;
CREATE TABLE net_scan (
    net_name    VARCHAR(128) NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER NOT NULL,
	PRIMARY KEY (net_name, plugin_id, plugin_sid)
);


---
--- Table: User
---
DROP TABLE users;
CREATE TABLE users (
    login		VARCHAR(64)  NOT NULL,
    name		VARCHAR(128) NOT NULL,
    pass    		VARCHAR(41)  NOT NULL,
    allowed_nets	VARCHAR(255) NOT NULL,
    email		VARCHAR(64),	
    PRIMARY KEY (login)
);

--
-- Data: User
--
INSERT INTO users (login, name, pass, allowed_nets) VALUES ('admin', 'OSSIM admin', '21232f297a57a5a743894a0e4a801fc3', '');

DROP SEQUENCE incident_id_seq;
CREATE SEQUENCE incident_id_seq;

--
-- Table: incident
--
DROP TABLE incident;
CREATE TABLE incident (
    id          BIGINT DEFAULT nextval('incident_id_seq'),
    title       VARCHAR(128) NOT NULL,
    date        TIMESTAMP DEFAULT now() NOT NULL,
    ref         VARCHAR(9) check (ref in ('Alarm', 'Alert', 'Metric', 'Hardware', 'Install')) NOT NULL DEFAULT 'Alarm',
    family      VARCHAR(9) check (family in ('OSSIM', 'Hardware', 'Install')) NOT NULL DEFAULT 'OSSIM',
    priority    INTEGER NOT NULL,
    PRIMARY KEY (id)
);

DROP SEQUENCE incident_ticket_id_seq;
CREATE SEQUENCE incident_ticket_id_seq;

--
-- Table: incident ticket
--
DROP TABLE incident_ticket;
CREATE TABLE incident_ticket (
    id              BIGINT DEFAULT nextval('incident_ticket_id_seq'),
    incident_id     INTEGER NOT NULL,
    date            TIMESTAMP DEFAULT now() NOT NULL,
    status          VARCHAR(8) check (status in ('Open', 'Closed')) NOT NULL DEFAULT 'Open',
    priority        INTEGER NOT NULL,
    users           VARCHAR(64) NOT NULL,
    description     VARCHAR(255),
    action          VARCHAR(255),
    in_charge       VARCHAR(64),
    transferred     VARCHAR(64),
    copy            VARCHAR(64),
    PRIMARY KEY (id, incident_id)
);

DROP SEQUENCE incident_alarm_id_seq;
CREATE SEQUENCE incident_alarm_id_seq;

--
-- Table: incident alarm
--
DROP TABLE incident_alarm;
CREATE TABLE incident_alarm (
    id              BIGINT DEFAULT nextval('incident_alarm_id_seq'),
    incident_id     INTEGER NOT NULL,
    src_ips         VARCHAR(255) NOT NULL,
    src_ports       VARCHAR(255) NOT NULL,
    dst_ips         VARCHAR(255) NOT NULL,
    dst_ports       VARCHAR(255) NOT NULL,
    PRIMARY KEY (id, incident_id)
);

DROP SEQUENCE incident_metric_id_seq;
CREATE SEQUENCE incident_metric_id_seq;

--
-- Table: incident metric
--
DROP TABLE incident_metric;
CREATE TABLE incident_metric (
    id              BIGINT DEFAULT nextval('incident_metric_id_seq'),
    incident_id     INTEGER NOT NULL,
    target          VARCHAR(255) NOT NULL,
    metric_type     VARCHAR(16) check (metric_type in ('Compromise', 'Attack')) NOT NULL DEFAULT 'Compromise',
    metric_value    INTEGER NOT NULL,
    PRIMARY KEY (id, incident_id)
);

DROP SEQUENCE incident_file_id_seq;
CREATE SEQUENCE incident_file_id_seq;

--
-- Table: incident_file
--
DROP TABLE incident_file;
CREATE TABLE incident_file (
	id		BIGINT DEFAULT nextval('incident_file_id_seq'),
	incident_id	INTEGER NOT NULL,
	incident_ticket	INTEGER NOT NULL,
	name		VARCHAR(50),
	type		VARCHAR(50),
	content		bytea, /* 16Mb */		
	PRIMARY KEY (id, incident_id, incident_ticket)
);

DROP SEQUENCE incident_alert_id_seq;
CREATE SEQUENCE incident_alert_id_seq;

--
-- Table: incident_alert
--
DROP TABLE incident_alert;
CREATE TABLE incident_alert (
	id		BIGINT DEFAULT nextval('incident_alert_id_seq'),
	incident_id	INTEGER NOT NULL,
	src_ips		VARCHAR(255) NOT NULL,
	src_ports	VARCHAR(255) NOT NULL,
	dst_ips		VARCHAR(255) NOT NULL,
	dst_ports	VARCHAR(255) NOT NULL,
	PRIMARY KEY (id, incident_id)
);



DROP SEQUENCE restoredb_log_id_seq;
CREATE SEQUENCE restoredb_log_id_seq;

--
-- Table: restoredb
--
DROP TABLE restoredb_log;
CREATE TABLE restoredb_log (
	id              BIGINT DEFAULT nextval('restoredb_log_id_seq'),
	date		TIMESTAMP,
	pid		INTEGER,
	users		VARCHAR(64),
	data		TEXT,
	status		INT8,
	percent		INT8,
	PRIMARY KEY (id)
);

DROP TABLE host_ids;
CREATE TABLE host_ids(
ip              INTEGER NOT NULL CONSTRAINT ip_not_negative_constraint CHECK (ip >= 0),
date            TIMESTAMP,
hostname        VARCHAR(255) NOT NULL,
sensor          VARCHAR(255) NOT NULL,
sid             INT8 NOT NULL,
event_type      VARCHAR(255) NOT NULL,
what            VARCHAR(255) NOT NULL,
target          VARCHAR(255) NOT NULL,
extra_data      VARCHAR(255) NOT NULL,
PRIMARY KEY     (ip,target,sid,date)
);
