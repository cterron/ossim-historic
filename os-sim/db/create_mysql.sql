
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


DROP TABLE IF EXISTS net_host_reference;
CREATE TABLE net_host_reference (
  net_name          varchar(128) NOT NULL,
  host_ip           varchar(15) NOT NULL,
  PRIMARY KEY       (net_name,host_ip)
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
    net_name        varchar(15) NOT NULL,
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
    net_name        varchar(64) NOT NULL,
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
    net_name        varchar(64) NOT NULL,
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
    net             varchar(15) NOT NULL,
    vulnerability   int NOT NULL DEFAULT 1,
    PRIMARY KEY     (net)
);

DROP TABLE IF EXISTS control_panel_host;
CREATE TABLE control_panel_host (
    host_ip         varchar(15) NOT NULL,
    time_range      varchar(5) NOT NULL DEFAULT 'day',
    max_c           int NOT NULL,
    max_a           int NOT NULL,
    max_c_date      datetime,
    max_a_date      datetime,
    avg_c           int NOT NULL,
    avg_a           int NOT NULL,
    PRIMARY KEY     (host_ip, time_range)
);

DROP TABLE IF EXISTS control_panel_net;
CREATE TABLE control_panel_net (
    net_name        varchar(15) NOT NULL,
    time_range      varchar(5) NOT NULL DEFAULT 'day',
    max_c           int NOT NULL,
    max_a           int NOT NULL,
    max_c_date      datetime,
    max_a_date      datetime,
    avg_c           int NOT NULL,
    avg_a           int NOT NULL,
    PRIMARY KEY     (net_name, time_range)
);

DROP TABLE IF EXISTS host_mac;
CREATE TABLE host_mac (
  ip                        varchar(15) UNIQUE NOT NULL,
  mac	                    varchar(255) NOT NULL,	
  previous	                varchar(255) NOT NULL,	
  anom                      int NOT NULL,
  mac_time                 varchar(100) NOT NULL,
  PRIMARY KEY       (ip)
);

DROP TABLE IF EXISTS host_os;
CREATE TABLE host_os (
  ip                        varchar(15) UNIQUE NOT NULL,
  os	                    varchar(255) NOT NULL,	
  previous	                varchar(255) NOT NULL,	
  anom                      int NOT NULL,
  os_time                 varchar(100) NOT NULL,
  PRIMARY KEY       (ip)
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

DROP TABLE IF EXISTS rrd_conf;
CREATE TABLE rrd_conf (
  ip                        varchar(15) UNIQUE NOT NULL,
  pkt_sent	                varchar(60) NOT NULL,	
  pkt_rcvd       	        varchar(60) NOT NULL,	
  bytes_sent	            varchar(60) NOT NULL,	
  bytes_rcvd	            varchar(60) NOT NULL,	
  tot_contacted_sent_peers	varchar(60) NOT NULL,	
  tot_contacted_rcvd_peers	varchar(60) NOT NULL,	
  ip_dns_sent_bytes	        varchar(60) NOT NULL,	
  ip_dns_rcvd_bytes	        varchar(60) NOT NULL,	
  ip_nbios_ip_sent_bytes	varchar(60) NOT NULL,
  ip_nbios_ip_rcvd_bytes	varchar(60) NOT NULL,
  ip_mail_sent_bytes	    varchar(60) NOT NULL,
  ip_mail_rcvd_bytes	    varchar(60) NOT NULL,
  mrtg_a	                varchar(60) NOT NULL,
  mrtg_c	                varchar(60) NOT NULL,
  PRIMARY KEY       (ip)
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

DROP TABLE IF EXISTS rrd_conf_global;
CREATE TABLE rrd_conf_global (
active_host_senders_num VARCHAR(60) NOT NULL,
arp_rarp_bytes    VARCHAR(60) NOT NULL,
broadcast_pkts    VARCHAR(60) NOT NULL,
ethernet_bytes    VARCHAR(60) NOT NULL, 
ethernet_pkts     VARCHAR(60) NOT NULL, 
icmp_bytes        VARCHAR(60) NOT NULL, 
igmp_bytes        VARCHAR(60) NOT NULL, 
ip_bytes          VARCHAR(60) NOT NULL, 
ip_dhcp_bootp_bytes VARCHAR(60) NOT NULL, 
ip_dns_bytes      VARCHAR(60) NOT NULL,
ip_edonkey_bytes  VARCHAR(60) NOT NULL, 
ip_ftp_bytes      VARCHAR(60) NOT NULL, 
ip_gnutella_bytes VARCHAR(60) NOT NULL, 
ip_http_bytes     VARCHAR(60) NOT NULL, 
ip_kazaa_bytes    VARCHAR(60) NOT NULL, 
ip_mail_bytes     VARCHAR(60) NOT NULL, 
ip_messenger_bytes VARCHAR(60) NOT NULL,
ip_nbios_ip_bytes VARCHAR(60) NOT NULL, 
ip_nfs_bytes      VARCHAR(60) NOT NULL, 
ip_nttp_bytes     VARCHAR(60) NOT NULL, 
ip_snmp_bytes     VARCHAR(60) NOT NULL, 
ip_ssh_bytes      VARCHAR(60) NOT NULL, 
ip_telnet_bytes   VARCHAR(60) NOT NULL, 
ip_winmx_bytes    VARCHAR(60) NOT NULL, 
ip_x11_bytes      VARCHAR(60) NOT NULL, 
ipx_bytes         VARCHAR(60) NOT NULL,
known_hosts_num   VARCHAR(60) NOT NULL,
multicast_pkts    VARCHAR(60) NOT NULL,
ospf_bytes        VARCHAR(60) NOT NULL,
other_bytes       VARCHAR(60) NOT NULL,
tcp_bytes         VARCHAR(60) NOT NULL,
udp_bytes         VARCHAR(60) NOT NULL,
up_to_1024_pkts   VARCHAR(60) NOT NULL,
up_to_128_pkts    VARCHAR(60) NOT NULL,
up_to_1518_pkts   VARCHAR(60) NOT NULL,
up_to_512_pkts    VARCHAR(60) NOT NULL,
up_to_64_pkts     VARCHAR(60) NOT NULL
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
	id		BIGINT NOT NULL AUTO_INCREMENT,
	timestamp	TIMESTAMP,
	sensor		TEXT NOT NULL,
	interface	TEXT NOT NULL,
	type		INTEGER NOT NULL,
	plugin_id	INTEGER NOT NULL,
	plugin_sid	INTEGER,
	protocol	INTEGER,
	src_ip		INTEGER UNSIGNED,
	dst_ip		INTEGER UNSIGNED,
	src_port	INTEGER,
	dst_port	INTEGER,
	condition	INTEGER,
	value		TEXT,
	time_interval	INTEGER,
	absolute	TINYINT,
	priority	INTEGER DEFAULT 1,
	reliability	INTEGER DEFAULT 1,
	asset_src	INTEGER DEFAULT 1,
	asset_dst	INTEGER DEFAULT 1,
	risk_a		INTEGER DEFAULT 1,
	risk_c		INTEGER DEFAULT 1,
	alarm           TINYINT DEFAULT 1,
	PRIMARY KEY (id)
);

--
-- Table: Backlog
--
DROP TABLE IF EXISTS backlog;
CREATE TABLE backlog (
	utime		BIGINT NOT NULL,
	id		INTEGER NOT NULL,
	name		TEXT,
	rule_level	INTEGER,
	rule_type	TINYINT,
	rule_name	TEXT,
	occurrence      INTEGER,
	time_out	INTEGER,
	matched		TINYINT,
	plugin_id	INTEGER,
	plugin_sid	INTEGER,
	src_ip		INTEGER UNSIGNED,
	dst_ip		INTEGER UNSIGNED,
	src_port	INTEGER,
	dst_port	INTEGER,
	condition       INTEGER,
	value		TEXT,
	time_interval	INTEGER,
	absolute	TINYINT,
	priority	INTEGER,
	reliability     INTEGER,
	PRIMARY KEY (utime, id)
);
