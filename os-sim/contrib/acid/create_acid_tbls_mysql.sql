# Copyright (C) 2000 Carnegie Mellon University
#
# Author: Roman Danyliw <roman@danyliw.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# - Purpose:
#   Creates the MySQL tables in the Snort database neccessary to support
#   ACID.
# 
#   TABLE acid_event: cache of signature, IP, port, and classification
#                     information
# 
#   TABLE acid_ag: stores the description of an Alert Group (AG)
# 
#   TABLE acid_ag_alert: stores the IDs of the alerts in an Alert Group (AG)
#
#   TABLE acid_ip_cache: caches DNS and whois information
#
#   TABLE base_roles: Stores the User roles available for the
#                     Authentication System
#
#   TABLE base_users: Stores the user names and passwords

CREATE TABLE IF NOT EXISTS `acid_event` (
  `sid` int(10) unsigned NOT NULL,
  `cid` int(10) unsigned NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  `sig_name` varchar(255) default NULL,
  `sig_class_id` int(10) unsigned default NULL,
  `sig_priority` int(10) unsigned default NULL,
  `timestamp` datetime NOT NULL,
  `ip_src` int(10) unsigned default NULL,
  `ip_dst` int(10) unsigned default NULL,
  `ip_proto` int(11) default NULL,
  `layer4_sport` int(10) unsigned default NULL,
  `layer4_dport` int(10) unsigned default NULL,
  `ossim_type` int(11) default '1',
  `ossim_priority` int(11) default '1',
  `ossim_reliability` int(11) default '1',
  `ossim_asset_src` int(11) default '1',
  `ossim_asset_dst` int(11) default '1',
  `ossim_risk_c` int(11) default '1',
  `ossim_risk_a` int(11) default '1',
  PRIMARY KEY  (`sid`,`cid`,`timestamp`),
  KEY `signature` (`signature`),
  KEY `sig_class_id` (`sig_class_id`),
  KEY `sig_priority` (`sig_priority`),
  KEY `timestamp` (`timestamp`),
  KEY `layer4_sport` (`layer4_sport`),
  KEY `layer4_dport` (`layer4_dport`),
  KEY `acid_event_ossim_type` (`ossim_type`),
  KEY `acid_event_ossim_asset_src` (`ossim_asset_src`),
  KEY `acid_event_ossim_risk_c` (`ossim_risk_c`),
  KEY `cid` (`cid`),
  KEY `ip_src` (`ip_src`,`timestamp`),
  KEY `sig_name` (`sig_name`,`timestamp`),
  KEY `ip_dst` (`ip_dst`,`timestamp`),
  KEY `acid_event_ossim_asset_dst` (`ossim_asset_dst`,`timestamp`),
  KEY `acid_event_ossim_priority` (`ossim_priority`,`timestamp`),
  KEY `acid_event_ossim_reliability` (`ossim_reliability`,`timestamp`),
  KEY `acid_event_ossim_risk_a` (`ossim_risk_a`,`timestamp`),
  KEY `ip_proto` (`ip_proto`,`timestamp`),
  KEY `sid` (`sid`,`timestamp`),
  FULLTEXT KEY `name` (`sig_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE acid_ag      ( ag_id               INT           UNSIGNED NOT NULL AUTO_INCREMENT,
                            ag_name             VARCHAR(40),
                            ag_desc             TEXT, 
                            ag_ctime            DATETIME,
                            ag_ltime            DATETIME,

                            PRIMARY KEY         (ag_id),
                            INDEX               (ag_id));

CREATE TABLE acid_ag_alert( ag_id               INT           UNSIGNED NOT NULL,
                            ag_sid              INT           UNSIGNED NOT NULL,
                            ag_cid              INT           UNSIGNED NOT NULL, 

                            PRIMARY KEY         (ag_id, ag_sid, ag_cid),
                            INDEX               (ag_id),
                            INDEX               (ag_sid, ag_cid));

CREATE TABLE acid_ip_cache( ipc_ip                  INT           UNSIGNED NOT NULL,
                            ipc_fqdn                VARCHAR(50),
                            ipc_dns_timestamp       DATETIME,
                            ipc_whois               TEXT,
                            ipc_whois_timestamp     DATETIME,

                            PRIMARY KEY         (ipc_ip),
                            INDEX               (ipc_ip) );

--
-- BASE tables
--
CREATE TABLE `base_roles` ( `role_id`           int(11)         NOT NULL,
                            `role_name`         varchar(20)     NOT NULL,
                            `role_desc`         varchar(75)     NOT NULL,
                            PRIMARY KEY         (`role_id`));
CREATE TABLE `base_users` ( `usr_id`            int(11)         NOT NULL,
                            `usr_login`         varchar(25)     NOT NULL,
                            `usr_pwd`           varchar(32)     NOT NULL,
                            `usr_name`          varchar(75)     NOT NULL,
                            `role_id`           int(11)         NOT NULL,
                            `usr_enabled`       int(11)         NOT NULL,
                            PRIMARY KEY         (`usr_id`),
                            INDEX               (`usr_login`));

INSERT INTO `base_roles` (`role_id`, `role_name`, `role_desc`) VALUES (1, 'Admin', 'Administrator'),
(10, 'User', 'Authenticated User'),
(10000, 'Anonymous', 'Anonymous User'),
(50, 'ag_editor', 'Alert Group Editor');



--
-- OSSIM Patch
--

--
-- Table: ossim_event
--
DROP TABLE IF EXISTS ossim_event;
CREATE TABLE ossim_event (
	sid		INT NOT NULL,
	cid		INT NOT NULL,
	type            INT NOT NULL,
	priority	INT DEFAULT 1,
	reliability	INT DEFAULT 1,
	asset_src	 INT DEFAULT 1,
	asset_dst	 INT DEFAULT 1,
	risk_c		 INT DEFAULT 1,
	risk_a		 INT DEFAULT 1,
	plugin_id	 INTEGER NOT NULL,
	plugin_sid INTEGER NOT NULL,
	PRIMARY KEY (sid, cid),
	INDEX		(type),
        INDEX           (priority),
        INDEX           (reliability),
        INDEX           (asset_src),
        INDEX           (asset_dst),
        INDEX           (risk_c),
        INDEX           (risk_a),
				INDEX           (plugin_id),
				INDEX           (plugin_sid)
);

DROP TABLE IF EXISTS extra_data;
CREATE TABLE extra_data (
        sid             INT8 NOT NULL,
        cid             INT8 NOT NULL,
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
				PRIMARY KEY (sid, cid)
);


CREATE TABLE `event_stats` (
  `timestamp` datetime NOT NULL,
  `sensors` int(10) unsigned NOT NULL,
  `sensors_total` int(10) unsigned NOT NULL,
  `uniq_events` int(10) unsigned NOT NULL,
  `categories` int(10) unsigned NOT NULL,
  `total_events` int(10) unsigned NOT NULL,
  `src_ips` int(10) unsigned NOT NULL,
  `dst_ips` int(10) unsigned NOT NULL,
  `uniq_ip_links` int(10) unsigned NOT NULL,
  `source_ports` int(10) unsigned NOT NULL,
  `dest_ports` int(10) unsigned NOT NULL,
  `source_ports_udp` int(10) unsigned NOT NULL,
  `source_ports_tcp` int(10) unsigned NOT NULL,
  `dest_ports_udp` int(10) unsigned NOT NULL,
  `dest_ports_tcp` int(10) unsigned NOT NULL,
  `tcp_events` int(10) unsigned NOT NULL,
  `udp_events` int(10) unsigned NOT NULL,
  `icmp_events` int(10) unsigned NOT NULL,
  `portscan_events` int(10) unsigned NOT NULL,
  PRIMARY KEY (`timestamp`),
  KEY `sensors_idx` (`sensors`),
  KEY `sensors_total_idx` (`sensors_total`),
  KEY `uniq_events_idx` (`uniq_events`),
  KEY `categories_idx` (`categories`),
  KEY `total_events_idx` (`total_events`),
  KEY `src_ips_idx` (`src_ips`),
  KEY `dst_ips_idx` (`dst_ips`),
  KEY `uniq_ip_links_idx` (`uniq_ip_links`),
  KEY `source_ports_idx` (`source_ports`),
  KEY `dest_ports_idx` (`dest_ports`),
  KEY `source_ports_udp_idx` (`source_ports_udp`),
  KEY `source_ports_tcp_idx` (`source_ports_tcp`),
  KEY `dest_ports_udp_idx` (`dest_ports_udp`),
  KEY `dest_ports_tcp_idx` (`dest_ports_tcp`),
  KEY `tcp_events_idx` (`tcp_events`),
  KEY `udp_events_idx` (`udp_events`),
  KEY `icmp_events_idx` (`icmp_events`),
  KEY `portscan_events_idx` (`portscan_events`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;



--
-- Table structure for table `ac_alerts_ipdst`
--

DROP TABLE IF EXISTS `ac_alerts_ipdst`;
CREATE TABLE `ac_alerts_ipdst` (
  `signature` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`signature`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alerts_ipsrc`
--

DROP TABLE IF EXISTS `ac_alerts_ipsrc`;
CREATE TABLE `ac_alerts_ipsrc` (
  `signature` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`signature`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alerts_sid`
--

DROP TABLE IF EXISTS `ac_alerts_sid`;
CREATE TABLE `ac_alerts_sid` (
  `signature` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`signature`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alerts_signature`
--

DROP TABLE IF EXISTS `ac_alerts_signature`;
CREATE TABLE `ac_alerts_signature` (
  `signature` int(10) unsigned NOT NULL,
  `sig_name` varchar(255) NOT NULL default '',
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sig_cnt` int(11) NOT NULL,
  `first_timestamp` datetime NOT NULL,
  `last_timestamp` datetime NOT NULL,
  PRIMARY KEY  (`signature`,`sig_name`,`sig_class_id`,`day`),
  KEY `day` (`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alertsclas_classid`
--

DROP TABLE IF EXISTS `ac_alertsclas_classid`;
CREATE TABLE `ac_alertsclas_classid` (
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(11) NOT NULL,
  `first_timestamp` datetime NOT NULL,
  `last_timestamp` datetime NOT NULL,
  PRIMARY KEY  (`sig_class_id`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alertsclas_ipdst`
--

DROP TABLE IF EXISTS `ac_alertsclas_ipdst`;
CREATE TABLE `ac_alertsclas_ipdst` (
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sig_class_id`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alertsclas_ipsrc`
--

DROP TABLE IF EXISTS `ac_alertsclas_ipsrc`;
CREATE TABLE `ac_alertsclas_ipsrc` (
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sig_class_id`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alertsclas_sid`
--

DROP TABLE IF EXISTS `ac_alertsclas_sid`;
CREATE TABLE `ac_alertsclas_sid` (
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sig_class_id`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_alertsclas_signature`
--

DROP TABLE IF EXISTS `ac_alertsclas_signature`;
CREATE TABLE `ac_alertsclas_signature` (
  `sig_class_id` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sig_class_id`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_dstaddr_ipdst`
--

DROP TABLE IF EXISTS `ac_dstaddr_ipdst`;
CREATE TABLE `ac_dstaddr_ipdst` (
  `ip_dst` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(11) NOT NULL,
  PRIMARY KEY  (`ip_dst`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_dstaddr_ipsrc`
--

DROP TABLE IF EXISTS `ac_dstaddr_ipsrc`;
CREATE TABLE `ac_dstaddr_ipsrc` (
  `ip_dst` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_dst`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_dstaddr_sid`
--

DROP TABLE IF EXISTS `ac_dstaddr_sid`;
CREATE TABLE `ac_dstaddr_sid` (
  `ip_dst` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_dst`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_dstaddr_signature`
--

DROP TABLE IF EXISTS `ac_dstaddr_signature`;
CREATE TABLE `ac_dstaddr_signature` (
  `ip_dst` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_dst`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_dport`
--

DROP TABLE IF EXISTS `ac_layer4_dport`;
CREATE TABLE `ac_layer4_dport` (
  `layer4_dport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(10) unsigned NOT NULL,
  `first_timestamp` datetime NOT NULL,
  `last_timestamp` datetime NOT NULL,
  PRIMARY KEY  (`layer4_dport`,`ip_proto`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_dport_ipdst`
--

DROP TABLE IF EXISTS `ac_layer4_dport_ipdst`;
CREATE TABLE `ac_layer4_dport_ipdst` (
  `layer4_dport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_dport`,`ip_proto`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_dport_ipsrc`
--

DROP TABLE IF EXISTS `ac_layer4_dport_ipsrc`;
CREATE TABLE `ac_layer4_dport_ipsrc` (
  `layer4_dport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_dport`,`ip_proto`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_dport_sid`
--

DROP TABLE IF EXISTS `ac_layer4_dport_sid`;
CREATE TABLE `ac_layer4_dport_sid` (
  `layer4_dport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_dport`,`ip_proto`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_dport_signature`
--

DROP TABLE IF EXISTS `ac_layer4_dport_signature`;
CREATE TABLE `ac_layer4_dport_signature` (
  `layer4_dport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_dport`,`ip_proto`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_sport`
--

DROP TABLE IF EXISTS `ac_layer4_sport`;
CREATE TABLE `ac_layer4_sport` (
  `layer4_sport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(10) unsigned NOT NULL,
  `first_timestamp` datetime NOT NULL,
  `last_timestamp` datetime NOT NULL,
  PRIMARY KEY  (`layer4_sport`,`ip_proto`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_sport_ipdst`
--

DROP TABLE IF EXISTS `ac_layer4_sport_ipdst`;
CREATE TABLE `ac_layer4_sport_ipdst` (
  `layer4_sport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_sport`,`ip_proto`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_sport_ipsrc`
--

DROP TABLE IF EXISTS `ac_layer4_sport_ipsrc`;
CREATE TABLE `ac_layer4_sport_ipsrc` (
  `layer4_sport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_sport`,`ip_proto`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_sport_sid`
--

DROP TABLE IF EXISTS `ac_layer4_sport_sid`;
CREATE TABLE `ac_layer4_sport_sid` (
  `layer4_sport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_sport`,`ip_proto`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_layer4_sport_signature`
--

DROP TABLE IF EXISTS `ac_layer4_sport_signature`;
CREATE TABLE `ac_layer4_sport_signature` (
  `layer4_sport` int(10) unsigned NOT NULL,
  `ip_proto` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`layer4_sport`,`ip_proto`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_sensor_ipdst`
--

DROP TABLE IF EXISTS `ac_sensor_ipdst`;
CREATE TABLE `ac_sensor_ipdst` (
  `sid` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sid`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_sensor_ipsrc`
--

DROP TABLE IF EXISTS `ac_sensor_ipsrc`;
CREATE TABLE `ac_sensor_ipsrc` (
  `sid` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_src` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sid`,`day`,`ip_src`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_sensor_sid`
--

DROP TABLE IF EXISTS `ac_sensor_sid`;
CREATE TABLE `ac_sensor_sid` (
  `sid` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(10) unsigned NOT NULL,
  `first_timestamp` datetime NOT NULL,
  `last_timestamp` datetime NOT NULL,
  PRIMARY KEY  (`sid`,`day`),
  KEY `day` (`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_sensor_signature`
--

DROP TABLE IF EXISTS `ac_sensor_signature`;
CREATE TABLE `ac_sensor_signature` (
  `sid` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`sid`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_srcaddr_ipdst`
--

DROP TABLE IF EXISTS `ac_srcaddr_ipdst`;
CREATE TABLE `ac_srcaddr_ipdst` (
  `ip_src` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `ip_dst` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_src`,`day`,`ip_dst`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_srcaddr_ipsrc`
--

DROP TABLE IF EXISTS `ac_srcaddr_ipsrc`;
CREATE TABLE `ac_srcaddr_ipsrc` (
  `ip_src` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `cid` int(11) NOT NULL,
  PRIMARY KEY  (`ip_src`,`day`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_srcaddr_sid`
--

DROP TABLE IF EXISTS `ac_srcaddr_sid`;
CREATE TABLE `ac_srcaddr_sid` (
  `ip_src` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `sid` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_src`,`day`,`sid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Table structure for table `ac_srcaddr_signature`
--

DROP TABLE IF EXISTS `ac_srcaddr_signature`;
CREATE TABLE `ac_srcaddr_signature` (
  `ip_src` int(10) unsigned NOT NULL,
  `day` date NOT NULL,
  `signature` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`ip_src`,`day`,`signature`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;


