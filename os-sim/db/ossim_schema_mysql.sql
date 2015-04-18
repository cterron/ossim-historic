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
