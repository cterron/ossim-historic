
/* ======== config ======== */
DROP TABLE IF EXISTS config;
CREATE TABLE config (
    conf    varchar(255) NOT NULL,
    value   TEXT,
    PRIMARY KEY (conf)
);

DROP TABLE IF EXISTS user_config;
CREATE TABLE user_config (
    login VARCHAR(64)  NOT NULL REFERENCES users (login),
    category VARCHAR(64) NOT NULL DEFAULT 'main',
    name VARCHAR(64) NOT NULL,
    value MEDIUMTEXT,
    PRIMARY KEY (login, category, name)
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
  lat                varchar(255) DEFAULT 0,
  lon                varchar(255) DEFAULT 0,
  PRIMARY KEY       (ip)
);

DROP TABLE IF EXISTS host_group;
CREATE TABLE host_group (
  name              varchar(128) UNIQUE NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  rrd_profile       varchar(64),
  descr             varchar(255),
  PRIMARY KEY       (name)
);

DROP TABLE IF EXISTS host_group_scan;
CREATE TABLE host_group_scan (
  host_group_name               varchar(128) NOT NULL,
  plugin_id       INTEGER NOT NULL,
  plugin_sid      INTEGER NOT NULL,
  PRIMARY KEY (host_group_name, plugin_id, plugin_sid)
);

DROP TABLE IF EXISTS host_group_reference;
CREATE TABLE host_group_reference (
  host_group_name        varchar(128) NOT NULL,
  host_ip                varchar(15) NOT NULL,
  PRIMARY KEY     (host_group_name, host_ip)
);


DROP TABLE IF EXISTS host_group_sensor_reference;
CREATE TABLE host_group_sensor_reference (
    group_name      varchar(128) NOT NULL,
    sensor_name     varchar(64) NOT NULL,
    PRIMARY KEY     (group_name, sensor_name)
);

DROP TABLE IF EXISTS net;
CREATE TABLE net (
  name              varchar(128) UNIQUE NOT NULL,
  ips               varchar(255) NOT NULL,
  asset             int NOT NULL,
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

/*This table is necessary to give a name to each interface in the sensor. i.e. used in ntop */
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

/*Used to store how much events arrive to the server in a specific time, from a
 * specific sensor*/
DROP TABLE IF EXISTS sensor_stats;
CREATE TABLE sensor_stats (
    name            varchar(64) NOT NULL,
    events          int NOT NULL DEFAULT 0,
    os_events       int NOT NULL DEFAULT 0,
    mac_events      int NOT NULL DEFAULT 0,
    service_events  int NOT NULL DEFAULT 0,
    ids_events      int NOT NULL DEFAULT 0,
    PRIMARY KEY     (name)
);

/* ======== policy ======== */
DROP TABLE IF EXISTS policy;
CREATE TABLE policy (
  `id` int(11) NOT NULL auto_increment,
  `priority` smallint(6) NOT NULL,
  `active` int(11) NOT NULL,
  `group` int(11) NOT NULL,
  `order` int(11) NOT NULL,
  `descr` varchar(255) default NULL,
  PRIMARY KEY  (`id`),
  KEY `group` (`group`),
  KEY `order` (`order`)
);

DROP TABLE IF EXISTS policy_group;
CREATE TABLE `policy_group` (
  `group_id` int(11) unsigned NOT NULL auto_increment,
  `name` varchar(100) NOT NULL,
  `descr` varchar(255) NOT NULL,
  `order` INT(11) NOT NULL,
  PRIMARY KEY  (`group_id`)
);

DROP TABLE IF EXISTS policy_seq;
CREATE TABLE policy_seq (
    id INT NOT NULL
);
INSERT INTO policy_seq (id) VALUES (0);

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

DROP TABLE IF EXISTS policy_plugin_reference;
CREATE TABLE policy_plugin_reference (
    policy_id       int NOT NULL,
    plugin_id        INTEGER NOT NULL,
    PRIMARY KEY     (policy_id, plugin_id)
);

DROP TABLE IF EXISTS policy_plugin_sid_reference;
CREATE TABLE policy_plugin_sid_reference (
    policy_id       int NOT NULL,
    plugin_sid      INTEGER NOT NULL,
    PRIMARY KEY     (policy_id, plugin_sid)
);

DROP TABLE IF EXISTS policy_plugin_group_reference;
CREATE TABLE policy_plugin_group_reference (
    policy_id       INTEGER NOT NULL REFERENCES policy(id),
    group_id        INTEGER NOT NULL REFERENCES plugin_group_descr(group_id),
    PRIMARY KEY (policy_id, group_id)
);

DROP TABLE IF EXISTS policy_role_reference;
CREATE TABLE policy_role_reference (
    policy_id       INTEGER NOT NULL REFERENCES policy(id),
    correlate       BOOLEAN    NOT NULL DEFAULT '1',
    cross_correlate BOOLEAN    NOT NULL DEFAULT '1',
    store           BOOLEAN    NOT NULL DEFAULT '1',
    qualify         BOOLEAN    NOT NULL DEFAULT '1',
    resend_alarm    BOOLEAN    NOT NULL DEFAULT '1',
    resend_event    BOOLEAN    NOT NULL DEFAULT '1',
    sign            INT(10) unsigned NOT NULL default '0',
    sem             TINYINT(1) NOT NULL default '1',
    sim             TINYINT(1) NOT NULL default '1',
    PRIMARY KEY (policy_id)
);

DROP TABLE IF EXISTS policy_target_reference;
CREATE TABLE policy_target_reference (
    policy_id       int NOT NULL,
    target_name     varchar(64),   /*this is the target to wich applies the policy, it can be server or sensor names*/
    PRIMARY KEY     (policy_id, target_name)
);

DROP TABLE IF EXISTS policy_host_group_reference;
CREATE TABLE IF NOT EXISTS policy_host_group_reference (
    policy_id int(11) NOT NULL,
    host_group_name varchar(128) NOT NULL,
    direction enum('source','dest') NOT NULL,
    PRIMARY KEY (policy_id,host_group_name,direction)
);

DROP TABLE IF EXISTS policy_net_group_reference;
CREATE TABLE IF NOT EXISTS policy_net_group_reference (
    policy_id int(11) NOT NULL,
    net_group_name varchar(128) NOT NULL,
    direction enum('source','dest') NOT NULL,
    PRIMARY KEY (policy_id,net_group_name,direction)
);

/* ======== servers ======== */

/* This table is needed only in multi-level architecture in the upper master
 * server. This is filled with
 * the information in server's config.xml, and from the children servers wich
 * connects into the master */

DROP TABLE IF EXISTS server_role;
CREATE TABLE server_role (
    name            varchar(64) NOT NULL,
    correlate       BOOLEAN    NOT NULL DEFAULT '1',
    cross_correlate BOOLEAN    NOT NULL DEFAULT '1',
    store           BOOLEAN    NOT NULL DEFAULT '1',
    qualify         BOOLEAN    NOT NULL DEFAULT '1',
    resend_alarm    BOOLEAN    NOT NULL DEFAULT '1',
    resend_event    BOOLEAN    NOT NULL DEFAULT '1',
    sign            INT(10) unsigned NOT NULL default '0',
    sim             TINYINT(1) NOT NULL default '1',
    sem             TINYINT(1) NOT NULL default '1',
    PRIMARY KEY     (name)
);

DROP TABLE IF EXISTS server;
CREATE TABLE server (
    name            varchar(64) NOT NULL,
    ip              varchar(15) NOT NULL,
    port            int NOT NULL,
    descr           varchar(255) NOT NULL,
    PRIMARY KEY     (name)
);


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
    scan_date       datetime,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (ip, scan_date)
);

DROP TABLE IF EXISTS net_vulnerability;
CREATE TABLE net_vulnerability (
    net             varchar(128) NOT NULL,
    scan_date       datetime,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net, scan_date)
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
    c_sec_level     float,
    a_sec_level     float,
    PRIMARY KEY     (id, rrd_type, time_range)
);
CREATE INDEX type_time ON control_panel(rrd_type,time_range);

--
-- Table: Host Mac.
--
DROP TABLE IF EXISTS host_mac;
CREATE TABLE host_mac (
    ip        INTEGER UNSIGNED NOT NULL,
    mac            VARCHAR(255) NOT NULL,
    date            DATETIME NOT NULL,
    vendor        VARCHAR(255),
    sensor        INTEGER UNSIGNED NOT NULL,
    interface   VARCHAR(64) NOT NULL,
    anom        INT DEFAULT 1,
    PRIMARY KEY     (ip, date, sensor)
);

DROP TABLE IF EXISTS host_mac_vendors;
CREATE TABLE IF NOT EXISTS host_mac_vendors (
  mac varchar(8) NOT NULL,
  vendor varchar(255) NOT NULL,
  PRIMARY KEY (mac)
);

--
-- Table: Host OS.
--
DROP TABLE IF EXISTS host_os;
CREATE TABLE host_os (
    ip        INTEGER UNSIGNED NOT NULL,
    os        VARCHAR(255) NOT NULL,
    date        DATETIME NOT NULL,
    sensor        INTEGER UNSIGNED NOT NULL,
    interface VARCHAR(64) NOT NULL,
    anom        INT DEFAULT 1,
    PRIMARY KEY    (ip,date,sensor)
);

DROP TABLE IF EXISTS host_services;
CREATE TABLE host_services (
    ip        INTEGER UNSIGNED NOT NULL,
    port    int NOT NULL,
    protocol int NOT NULL,
    service varchar(128),
    service_type varchar(128),
    version varchar(255) NOT NULL DEFAULT "unknown",
    date        DATETIME NOT NULL,
    origin  int NOT NULL DEFAULT 0,
    sensor        INTEGER UNSIGNED NOT NULL,
    interface VARCHAR(64) NOT NULL,
    anom    INT DEFAULT 1,
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
    `range`                   varchar(30) NOT NULL,
    over                    int NOT NULL,
    acked                   int DEFAULT 0
);


DROP TABLE IF EXISTS rrd_anomalies_global;
CREATE TABLE rrd_anomalies_global (
    what                    varchar(100) NOT NULL,
    count                   int NOT NULL,
    anomaly_time            varchar(40) NOT NULL,
    `range`                   varchar(30) NOT NULL,
    over                    int NOT NULL,
    acked                   int DEFAULT 0
);

--
-- Table: Category
--
DROP TABLE IF EXISTS category;
CREATE TABLE category (
    id        INTEGER NOT NULL,
    name        VARCHAR (100) NOT NULL,
    PRIMARY KEY (id)
);

--
-- Table: Classification
--
DROP TABLE IF EXISTS classification;
CREATE TABLE classification (
    id        INTEGER NOT NULL,
    name        VARCHAR (100) NOT NULL,
    description    TEXT,
    priority    INTEGER,
    PRIMARY KEY (id)
);

--
-- Table: Plugin
--
DROP TABLE IF EXISTS plugin;
CREATE TABLE plugin (
    id        INTEGER NOT NULL,
    type        SMALLINT NOT NULL,
    name        VARCHAR (100) NOT NULL,
    description    TEXT,
    PRIMARY KEY (id)
);

--
-- Table: Plugin Sid
--
DROP TABLE IF EXISTS plugin_sid;
CREATE TABLE plugin_sid (
    plugin_id    INTEGER NOT NULL,
    sid            INTEGER NOT NULL,
    category_id    INTEGER,
    class_id    INTEGER,
    reliability    INTEGER DEFAULT 1,
    priority    INTEGER DEFAULT 1,
    name        VARCHAR (255) NOT NULL,
    aro         DECIMAL (11,4) NOT NULL DEFAULT 0,
    PRIMARY KEY (plugin_id, sid)
);

--
-- Tables for the Policy Groups
--

-- Table: Plugin Group Descr: store the name and description of the plugin group.
DROP TABLE IF EXISTS plugin_group_descr;
CREATE TABLE plugin_group_descr (
    group_id    INTEGER NOT NULL ,
    name        VARCHAR(125) NOT NULL,
    descr       VARCHAR(255) NOT NULL,
    PRIMARY KEY (group_id, name)
);

DROP TABLE IF EXISTS plugin_group_descr_seq;
CREATE TABLE plugin_group_descr_seq (
    id INT NOT NULL
);
INSERT INTO plugin_group_descr_seq VALUES (0);

-- Table: Plugin group: used to have a relationship between plugin's and it sids
DROP TABLE IF EXISTS plugin_group;
CREATE TABLE plugin_group (
    group_id    INTEGER NOT NULL REFERENCES plugin_group_descr(group_id),
    plugin_id   INTEGER NOT NULL REFERENCES plugin(id),
    plugin_sid  TEXT NOT NULL,
    PRIMARY KEY (group_id, plugin_id)
);

--
-- Table: Event
--

DROP TABLE IF EXISTS event;
CREATE TABLE event (
        id              BIGINT NOT NULL,
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
        event_condition       INTEGER,
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
CREATE INDEX event_idx ON event (timestamp);

--
-- Table: Backlog
--
DROP TABLE IF EXISTS backlog;
CREATE TABLE backlog (
    id        BIGINT NOT NULL DEFAULT 0,
    directive_id    INTEGER NOT NULL,
    timestamp    TIMESTAMP NOT NULL,
    matched        TINYINT,
    PRIMARY KEY (id)
);

--
-- Table: Backlog Event
--
DROP TABLE IF EXISTS backlog_event;
CREATE TABLE backlog_event (
    backlog_id    BIGINT NOT NULL,
    event_id    BIGINT NOT NULL,
    time_out    INTEGER,
    occurrence    INTEGER,
    rule_level    INTEGER,
    matched        TINYINT,
    PRIMARY KEY (backlog_id, event_id)
);

CREATE INDEX event_idx ON backlog_event(event_id);


--
-- Table: Temporary Event Tables
--

DROP TABLE IF EXISTS event_tmp;
CREATE TABLE event_tmp (
        id              BIGINT NOT NULL,
        timestamp       TIMESTAMP NOT NULL,
        sensor          TEXT NOT NULL,
        interface       TEXT NOT NULL,
        type            INTEGER NOT NULL,
        plugin_id       INTEGER NOT NULL,
        plugin_sid      INTEGER NOT NULL,
        plugin_sid_name varchar(255),
        protocol        INTEGER,
        src_ip          INTEGER UNSIGNED,
        dst_ip          INTEGER UNSIGNED,
        src_port        INTEGER,
        dst_port        INTEGER,
        priority        INTEGER DEFAULT 1,
        reliability     INTEGER DEFAULT 1,
        asset_src       INTEGER DEFAULT 1,
        asset_dst       INTEGER DEFAULT 1,
        risk_a          INTEGER DEFAULT 1,
        risk_c          INTEGER DEFAULT 1,
        alarm           TINYINT DEFAULT 1,
        filename        varchar(255),
        username        varchar(255),
        password        varchar(255),
        userdata1       varchar(255),
        userdata2       varchar(255),
        userdata3       varchar(255),
        userdata4       varchar(255),
        userdata5       varchar(255),
        userdata6       TEXT,
        userdata7       TEXT,
        userdata8       TEXT,
        userdata9       TEXT,
        PRIMARY KEY (id)
);

DROP TABLE IF EXISTS event_tmp_filter;
CREATE TABLE event_tmp_filter (
        id              BIGINT NOT NULL,
        login           varchar(255),
        timestamp       TIMESTAMP NOT NULL,
        sensor          TEXT NOT NULL,
        interface       TEXT NOT NULL,
        type            INTEGER NOT NULL,
        plugin_id       INTEGER NOT NULL,
        plugin_sid      INTEGER NOT NULL,
        event_name      varchar(255),
        protocol        INTEGER,
        src_ip          INTEGER UNSIGNED,
        dst_ip          INTEGER UNSIGNED,
        src_port        INTEGER,
        dst_port        INTEGER,
        priority        INTEGER DEFAULT 1,
        reliability     INTEGER DEFAULT 1,
        asset_src       INTEGER DEFAULT 1,
        asset_dst       INTEGER DEFAULT 1,
        risk_a          INTEGER DEFAULT 1,
        risk_c          INTEGER DEFAULT 1,
        alarm           TINYINT DEFAULT 1,
        filename        varchar(255),
        username        varchar(255),
        password        varchar(255),
        userdata1       varchar(255),
        userdata2       varchar(255),
        userdata3       varchar(255),
        userdata4       varchar(255),
        userdata5       varchar(255),
        userdata6       TEXT,
        userdata7       TEXT,
        userdata8       TEXT,
        userdata9       TEXT,
        PRIMARY KEY (id, login)
);

--
-- Sequences
--

DROP TABLE IF EXISTS event_seq;
CREATE TABLE event_seq (
         id INTEGER UNSIGNED NOT NULL
);
INSERT INTO event_seq VALUES (0);

DROP TABLE IF EXISTS backlog_seq;
CREATE TABLE backlog_seq (
         id INTEGER UNSIGNED NOT NULL
);
INSERT INTO backlog_seq VALUES (0);

DROP TABLE IF EXISTS backlog_event_seq;
CREATE TABLE backlog_event_seq (
         id INTEGER UNSIGNED NOT NULL
);
INSERT INTO backlog_event_seq VALUES (0);

DROP TABLE IF EXISTS event_tmp_seq;
CREATE TABLE event_tmp_seq (
         id INTEGER UNSIGNED NOT NULL
);
INSERT INTO event_tmp_seq (id) VALUES (0);


--
-- Table: Alarm
--
DROP TABLE IF EXISTS alarm;
CREATE TABLE alarm (
        backlog_id      BIGINT NOT NULL,
        event_id        BIGINT NOT NULL,
        timestamp       TIMESTAMP NOT NULL,
        status          ENUM ("open", "closed") DEFAULT "open",
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
        efr             INTEGER (11) NOT NULL DEFAULT 0,
        PRIMARY KEY (backlog_id),
        KEY `timestamp` (`timestamp`),
        KEY `src_ip` (`src_ip`),
        KEY `dst_ip` (`dst_ip`)
);

--
-- Alarmgroups
--
DROP TABLE IF EXISTS alarm_group;
CREATE TABLE alarm_group (
        id              BIGINT(20) NOT NULL auto_increment,
        status          ENUM("open","closed") DEFAULT "open",
        timestamp       TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP on UPDATE CURRENT_TIMESTAMP,
        owner           VARCHAR(64) DEFAULT NULL,
        descr           VARCHAR(255) NOT NULL,
        PRIMARY KEY (id)
);

DROP TABLE IF EXISTS alarm_group_members;
CREATE TABLE alarm_group_members (
        group_id        BIGINT(20) NOT NULL,
        backlog_id      BIGINT(20) NOT NULL,
        event_id        BIGINT(20) NOT NULL,
        PRIMARY KEY (backlog_id, event_id)
);

--
-- Table: plugin_reference
--
DROP TABLE IF EXISTS plugin_reference;
CREATE TABLE plugin_reference (
    plugin_id    INTEGER NOT NULL,
    plugin_sid    INTEGER NOT NULL,
    reference_id    INTEGER NOT NULL,
    reference_sid    INTEGER NOT NULL,
    PRIMARY KEY (plugin_id, plugin_sid, reference_id, reference_sid)
);

--
-- Table: Host plugin sid
--
DROP TABLE IF EXISTS host_plugin_sid;
CREATE TABLE host_plugin_sid (
    host_ip         INTEGER UNSIGNED NOT NULL,
    plugin_id    INTEGER NOT NULL,
    plugin_sid    INTEGER NOT NULL,
    PRIMARY KEY (host_ip, plugin_id, plugin_sid)
);

--
-- Table: Host scan
--
DROP TABLE IF EXISTS host_scan;
CREATE TABLE host_scan (
    host_ip         INTEGER UNSIGNED NOT NULL,
    plugin_id    INTEGER NOT NULL,
    plugin_sid    INTEGER NOT NULL,
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
    email   varchar(64),
    company varchar(128),
    department varchar(128),
    language varchar(12) DEFAULT 'en_GB' NOT NULL,
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
    date        DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',
    ref         ENUM ('Alarm', 'Alert', 'Event', 'Metric', 'Anomaly', 'Vulnerability') NOT NULL DEFAULT 'Alarm',
    type_id     VARCHAR(64) NOT NULL DEFAULT "Generic",
    priority    INTEGER NOT NULL,
    status      ENUM ('Open', 'Closed') NOT NULL DEFAULT 'Open',
    last_update DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',
    in_charge     VARCHAR(64) NOT NULL,
    submitter   VARCHAR(64) NOT NULL,
    event_start DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',
    event_end   DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',
    PRIMARY KEY (id)
);

DROP TABLE IF EXISTS incident_type;
CREATE TABLE incident_type (
    id          VARCHAR(64) NOT NULL,
    descr       VARCHAR(255) NOT NULL DEFAULT "",
    `keywords` varchar(255) NOT NULL,
    PRIMARY KEY (id)
);

INSERT INTO incident_type (id, descr, keywords) VALUES ("Generic", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Expansion Virus", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Corporative Nets Attack", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Policy Violation", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Security Weakness", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Net Performance", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Applications and Systems Failures", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ("Anomalies", "", "");
INSERT INTO incident_type (id, descr, keywords) VALUES ('Nessus Vulnerability',"", "");

--
-- Table: incident ticket
--
DROP TABLE IF EXISTS incident_ticket;
CREATE TABLE incident_ticket (
    id              INTEGER NOT NULL,
    incident_id     INTEGER NOT NULL,
    date            DATETIME NOT NULL DEFAULT '0000-00-00 00:00:00',
    status          ENUM ('Open', 'Closed') NOT NULL DEFAULT 'Open',
    priority        INTEGER NOT NULL,
    users           VARCHAR(64) NOT NULL,
    description     TEXT,
    action          TEXT,
    in_charge       VARCHAR(64),
    transferred     VARCHAR(64),
    PRIMARY KEY (id, incident_id)
);

DROP TABLE IF EXISTS incident_ticket_seq;
CREATE TABLE incident_ticket_seq (
    id INT NOT NULL
);
INSERT INTO incident_ticket_seq VALUES (0);

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
-- Table: incident event
--
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

--
-- Table: incident metric
--
DROP TABLE IF EXISTS incident_metric;
CREATE TABLE incident_metric (
    id              INTEGER NOT NULL AUTO_INCREMENT,
    incident_id     INTEGER NOT NULL,
    target          VARCHAR(255) NOT NULL,
    metric_type     ENUM ('Compromise', 'Attack', 'Level') NOT NULL DEFAULT 'Compromise',
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
-- Table: incident anomaly
--
DROP TABLE IF EXISTS incident_anomaly;
CREATE TABLE incident_anomaly (
     id              INTEGER NOT NULL AUTO_INCREMENT,
     incident_id     INTEGER NOT NULL,
     anom_type       ENUM ('mac', 'service', 'os') NOT NULL  DEFAULT 'mac',
     ip              VARCHAR(255) NOT NULL,
     data_orig       VARCHAR(255) NOT NULL,
     data_new        VARCHAR(255) NOT NULL,
     PRIMARY KEY (id, incident_id)
);

--
-- Table: incident vulnerabilities
--

DROP TABLE IF EXISTS incident_vulns;
CREATE TABLE incident_vulns (
    id int(11) NOT NULL,
    incident_id int(11) NOT NULL,
    ip varchar(255) NOT NULL,
    port varchar(255) NOT NULL,
    nessus_id varchar(255) NOT NULL,
    risk varchar(255) NOT NULL,
    description text default NULL,
    PRIMARY KEY (id,incident_id)
);

DROP TABLE IF EXISTS incident_vulns_seq;
CREATE TABLE incident_vulns_seq (
    id int(11) NOT NULL
);
INSERT INTO incident_vulns_seq VALUES(0);


--
-- Table: incident TAGs
--
DROP TABLE IF EXISTS incident_tag_descr;
CREATE TABLE incident_tag_descr (
        id INT(11) NOT NULL,
        name VARCHAR(64),
        descr TEXT,
        PRIMARY KEY(id)
);

DROP TABLE IF EXISTS incident_tag;
CREATE TABLE incident_tag (
        tag_id INT(11) NOT NULL REFERENCES incident_tags_descr(id),
        incident_id INT(11) NOT NULL REFERENCES incident(id),
        PRIMARY KEY (tag_id, incident_id)
);

DROP TABLE IF EXISTS incident_tag_descr_seq;
CREATE TABLE incident_tag_descr_seq (
        id INT NOT NULL
);
INSERT INTO incident_tag_descr_seq VALUES (0);
INSERT INTO incident_tag_descr VALUES(65001,'OSSIM_INTERNAL_PENDING','DONT DELETE');
INSERT INTO incident_tag_descr VALUES(65002,'OSSIM_INTERNAL_FALSE_POSITIVE','DONT DELETE');


--
-- Table: incident_subscrip
--
DROP TABLE IF EXISTS incident_subscrip;
CREATE TABLE incident_subscrip (
    login VARCHAR(64) NOT NULL REFERENCES users(login),
    incident_id INT(11) NOT NULL REFERENCES incident(id),
    PRIMARY KEY (login, incident_id)
);

--
-- Table: restoredb
--
DROP TABLE IF EXISTS restoredb_log;
CREATE TABLE restoredb_log (
    id        INTEGER NOT NULL AUTO_INCREMENT,
    date        TIMESTAMP,
    pid        INTEGER,
    users        VARCHAR(64),
    data        TEXT,
    status        SMALLINT,
    percent        SMALLINT,
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
    plugin_sid             INTEGER UNSIGNED NOT NULL,
    event_type      VARCHAR(255) NOT NULL,
    what            VARCHAR(255) NOT NULL,
    target          VARCHAR(255) NOT NULL,
    extra_data      VARCHAR(255) NOT NULL,
    cid             INTEGER UNSIGNED NOT NULL,
    sid             INTEGER UNSIGNED NOT NULL,
    PRIMARY KEY     (ip,target,plugin_sid,date)
);

--
-- User action logging
--

DROP TABLE IF EXISTS log_config;
CREATE TABLE log_config(
    code        INTEGER UNSIGNED NOT NULL,
    log         BOOL    DEFAULT "0",
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

---------------------------------------------------------------
---- Business Processes
---------------------------------------------------------------

DROP TABLE IF EXISTS bp_process;
CREATE TABLE bp_process (
    id          INT NOT NULL,
    name        VARCHAR(255) NOT NULL,
    description TEXT,
    valuation   DECIMAL (11,2) NOT NULL DEFAULT 0,
    PRIMARY KEY (id)
);

--
-- List of assets conforming a bussiness process
--
DROP TABLE IF EXISTS bp_asset;
CREATE TABLE bp_asset (
    id          INT NOT NULL,
    name        VARCHAR(255) NOT NULL,
    description TEXT,
    PRIMARY KEY (id)
);

--
-- List of members that conform an asset
-- "member" is for example: 10.10.10.10, /etc/passwd
-- "member_type" is for example: host, file
--
DROP TABLE IF EXISTS bp_asset_member;
CREATE TABLE bp_asset_member (
    asset_id    INT NOT NULL REFERENCES bp_asset(id),
    member      TEXT NOT NULL,
    member_type VARCHAR(255) NOT NULL REFERENCES bp_asset_member_type(type_name)
);

--
-- List of supported member types
--
DROP TABLE IF EXISTS bp_asset_member_type;
CREATE TABLE bp_asset_member_type (
    type_name  VARCHAR(255) NOT NULL UNIQUE,
    PRIMARY KEY (type_name)
);
INSERT INTO bp_asset_member_type (type_name) VALUES ('host');
INSERT INTO bp_asset_member_type (type_name) VALUES ('net');
INSERT INTO bp_asset_member_type (type_name) VALUES ('file');
INSERT INTO bp_asset_member_type (type_name) VALUES ('host_group');
INSERT INTO bp_asset_member_type (type_name) VALUES ('net_group');

--
-- Which assets belongs to which business process (and its relevance)
-- Note: the same asset could belong to many processes
--
DROP TABLE IF EXISTS bp_process_asset_reference;
CREATE TABLE bp_process_asset_reference (
    process_id  INT NOT NULL REFERENCES bp_process(id),
    asset_id    INT NOT NULL REFERENCES bp_asset(id),
    severity    INT(2) NOT NULL, /* How important is that asset (0 - low, 1 - medium, 2 - high) */
    PRIMARY KEY (process_id, asset_id)
);

--
-- Lists persons responsible of that asset
--
DROP TABLE IF EXISTS bp_asset_responsible;
CREATE TABLE bp_asset_responsible (
    asset_id     INT NOT NULL REFERENCES bp_asset(id),
    login         VARCHAR(64) NOT NULL REFERENCES users(login),
    PRIMARY KEY (asset_id, login)
);

--
-- Status of the diferent members
-- (frameworkd fills that table)
--
-- col "measure_type" is for example: alarm, vulnerability, incident, metric
--
DROP TABLE IF EXISTS bp_member_status;
CREATE TABLE bp_member_status (
    member        TEXT NOT NULL REFERENCES bp_asset_member(member),
    status_date   DATETIME NOT NULL,
    measure_type  VARCHAR(255) NOT NULL,
    severity      INT(2) NOT NULL /* number between 0-10: 0 = ok, 2 = low, 5 = med, 7 = high */
);

--
-- Sequence used for the business process related tables
--
DROP TABLE IF EXISTS bp_seq;
CREATE TABLE bp_seq (
    id INT NOT NULL
);
INSERT INTO bp_seq (id) VALUES (0);


--
-- Plugin Scheduler
--
DROP TABLE IF EXISTS plugin_scheduler;
CREATE TABLE plugin_scheduler(
    id          INT NOT NULL,
    plugin VARCHAR(255) NOT NULL,
    plugin_minute VARCHAR(255) NOT NULL,
    plugin_hour VARCHAR(255) NOT NULL,
    plugin_day_month VARCHAR(255) NOT NULL,
    plugin_month VARCHAR(255) NOT NULL,
    plugin_day_week VARCHAR(255) NOT NULL,
    type_scan VARCHAR(255) NOT NULL,
    PRIMARY KEY (id)
);

DROP TABLE IF EXISTS plugin_scheduler_sensor_reference;
CREATE TABLE plugin_scheduler_sensor_reference (
    plugin_scheduler_id          INT NOT NULL,
    sensor_name VARCHAR(255) NOT NULL,
    PRIMARY KEY     (plugin_scheduler_id, sensor_name)
);

DROP TABLE IF EXISTS plugin_scheduler_netgroup_reference;
CREATE TABLE plugin_scheduler_netgroup_reference (
    plugin_scheduler_id          INT NOT NULL,
    netgroup_name VARCHAR(255) NOT NULL,
    PRIMARY KEY     (plugin_scheduler_id, netgroup_name)
);

DROP TABLE IF EXISTS plugin_scheduler_hostgroup_reference;
CREATE TABLE plugin_scheduler_hostgroup_reference (
    plugin_scheduler_id          INT NOT NULL,
    hostgroup_name VARCHAR(255) NOT NULL,
    PRIMARY KEY     (plugin_scheduler_id, hostgroup_name)
);

DROP TABLE IF EXISTS plugin_scheduler_net_reference;
CREATE TABLE plugin_scheduler_net_reference (
    plugin_scheduler_id          INT NOT NULL,
    net_name VARCHAR(255) NOT NULL,
    PRIMARY KEY     (plugin_scheduler_id, net_name)
);

DROP TABLE IF EXISTS plugin_scheduler_host_reference;
CREATE TABLE plugin_scheduler_host_reference (
    plugin_scheduler_id          INT NOT NULL,
    ip  varchar(15) NOT NULL,
    PRIMARY KEY     (plugin_scheduler_id, ip)
);

DROP TABLE IF EXISTS plugin_scheduler_seq;
CREATE TABLE plugin_scheduler_seq (
    id INT NOT NULL
);
INSERT INTO plugin_scheduler_seq (id) VALUES (0);

--
-- Maps
--
DROP TABLE IF EXISTS map;
CREATE TABLE map (
    id INT NOT NULL,
    name VARCHAR(255) NOT NULL,
    engine ENUM('openlayers_op', 'openlayers_ve', 'openlayers_yahoo', 'openlayers_image'),
    engine_data1 MEDIUMTEXT,
    engine_data2 TEXT,
    engine_data3 TEXT,
    engine_data4 TEXT,
    center_x VARCHAR(255),
    center_y VARCHAR(255),
    zoom INT,
    show_controls BOOL DEFAULT 1,
    PRIMARY KEY (id)
);
DROP TABLE IF EXISTS map_seq;
CREATE TABLE map_seq (
     id INTEGER UNSIGNED NOT NULL
);
INSERT INTO map_seq VALUES (0);

DROP TABLE IF EXISTS map_element;
CREATE TABLE map_element (
    id INT NOT NULL,
    type ENUM('host', 'sensor', 'network', 'server'),
    ossim_element_key VARCHAR(255),
    map_id INT NOT NULL REFERENCES map(id),
    x VARCHAR(255),
    y VARCHAR(255),
    PRIMARY KEY (id)
);
DROP TABLE IF EXISTS map_element_seq;
CREATE TABLE map_element_seq (
     id INTEGER UNSIGNED NOT NULL
);
INSERT INTO map_element_seq VALUES (0);

DROP TABLE IF EXISTS `risk_indicators`;
CREATE TABLE `risk_indicators` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(100) collate utf8_unicode_ci NOT NULL default '',
  `map` int(11) NOT NULL default '0',
  `url` varchar(255) collate utf8_unicode_ci NOT NULL default '',
  `type` varchar(100) collate utf8_unicode_ci NOT NULL default '',
  `type_name` varchar(255) collate utf8_unicode_ci NOT NULL default '',
  `icon` varchar(255) collate utf8_unicode_ci NOT NULL default '',
  `x` int(11) NOT NULL default '0',
  `y` int(11) NOT NULL default '0',
  `w` int(11) NOT NULL default '0',
  `h` int(11) NOT NULL default '0',
  PRIMARY KEY  (`id`)
);

DROP TABLE IF EXISTS `repository`;
 CREATE TABLE IF NOT EXISTS `repository` (
  `id` int(11) NOT NULL auto_increment,
  `title` varchar(256) NOT NULL,
  `text` text NOT NULL,
  `date` date NOT NULL,
  `user` varchar(64) NOT NULL,
  `keywords` varchar(256) NOT NULL COMMENT 'Comma separated',
  PRIMARY KEY  (`id`),
  KEY `title` (`title`),
  KEY `keywords` (`keywords`),
  FULLTEXT KEY `text` (`text`)
);

DROP TABLE IF EXISTS `repository_attachments`;
CREATE TABLE IF NOT EXISTS `repository_attachments` (
  `id` int(11) NOT NULL auto_increment,
  `id_document` int(11) NOT NULL,
  `name` varchar(256) NOT NULL,
  `type` varchar(4) NOT NULL,
  PRIMARY KEY  (`id`)
);

DROP TABLE IF EXISTS `repository_relationships`;
CREATE TABLE IF NOT EXISTS `repository_relationships` (
  `id` int(11) NOT NULL auto_increment,
  `id_document` int(11) NOT NULL,
  `name` varchar(128) NOT NULL,
  `type` varchar(16) NOT NULL,
  `keyname` varchar(128) NOT NULL,
  PRIMARY KEY  (`id`)
); 

DROP TABLE IF EXISTS `sensor_agent_info`;
CREATE TABLE IF NOT EXISTS `sensor_agent_info` (
  `ip` varchar(64) NOT NULL,
    `version` varchar(64) NOT NULL,
    PRIMARY KEY  (`ip`)
);


-- vim:ts=4 sts=4 tw=79 expandtab:
