--
-- Table: Conf
--

DROP TABLE conf;
CREATE TABLE conf (
	recovery		INTEGER,
	threshold		INTEGER,
	graph_threshold		INTEGER,
	bar_length_left		INTEGER,
	bar_length_right	INTEGER,
	CONSTRAINT conf_pky PRIMARY KEY (recovery, threshold, graph_threshold, bar_length_left, bar_length_right)
);

--
-- Table: host
--
DROP TABLE host;
CREATE TABLE host (
  ip                varchar(15) PRIMARY KEY,
  hostname          varchar(128) NOT NULL,
  asset             int(6) NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL,
  persistence       int NOT NULL,
  nat               varchar(15),
  descr             varchar(255)
);

--
-- Table: scan
--
DROP TABLE IF EXISTS scan;
CREATE TABLE scan (
    ip              varchar(15) PRIMARY KEY,
    active          int NOT NULL
);

--
-- Table: Net
--
DROP TABLE IF EXISTS net;
CREATE TABLE net (
  name              varchar(128) PRIMARY KEY,
  ips               TEXT NOT NULL,
  priority          int NOT NULL,
  threshold_c       int NOT NULL,
  threshold_a       int NOT NULL,
  alert             int NOT NULL,
  persistence       int NOT NULL,
  descr             TEXT
);

--
-- Table: net_host_reference;
--
DROP TABLE IF EXISTS net_host_reference;
CREATE TABLE net_host_reference (
  net_name          varchar(128) NOT NULL,
  host_ip           varchar(15) NOT NULL,
  PRIMARY KEY       (net_name, host_ip)
);

--
-- Table: Category
--
DROP TABLE category;
CREATE TABLE category (
	id		INTEGER PRIMARY KEY,
	name		VARCHAR (100) NOT NULL,
	CONSTRAINT category_name_unq UNIQUE (name)
);

--
-- Table: Classification
--
DROP TABLE classification;
CREATE TABLE classification (
	id		INTEGER PRIMARY KEY,
	name		VARCHAR (100) NOT NULL,
	description	TEXT,
	priority	INTEGER,
	CONSTRAINT class_name_unq UNIQUE (name)
);

--
-- Table: Plugin
--
DROP TABLE plugin;
CREATE TABLE plugin (
	id		INTEGER PRIMARY KEY,
	type		INTEGER NOT NULL,
	name		VARCHAR (100) NOT NULL,
	description	TEXT,
	CONSTRAINT plugin_name_unq UNIQUE (name)
);

--
-- Table: Plugin Sid
--
DROP TABLE IF EXISTS plugin_sid;
CREATE TABLE plugin_sid (
	plugin_id	INTEGER,
	sid		INTEGER,
	category_id	INTEGER,
	class_id	INTEGER,
	name		TEXT NOT NULL,
	CONSTRAINT pluing_sid_pky PRIMARY KEY (plugin_id, sid),
	CONSTRAINT plugin_sid_plugin_id_fky FOREIGN KEY (plugin_id) REFERENCES plugin (id) ON DELETE CASCADE,
	CONSTRAINT plugin_sid_category_id_fky FOREIGN KEY (category_id) REFERENCES category (id) ON DELETE SET NULL,
	CONSTRAINT plugin_sid_class_id_fky FOREIGN KEY (class_id) REFERENCES classification (id) ON DELETE SET NULL,
);
