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

CREATE TABLE acid_event   ( sid                 INT UNSIGNED NOT NULL,
                            cid                 INT UNSIGNED NOT NULL,     
                            signature           INT UNSIGNED NOT NULL,
                            sig_name            VARCHAR(255),
                            sig_class_id        INT UNSIGNED,
                            sig_priority        INT UNSIGNED,
                            timestamp           DATETIME NOT NULL,
                            ip_src              INT UNSIGNED,
                            ip_dst              INT UNSIGNED,
                            ip_proto            INT,
                            layer4_sport        INT UNSIGNED,
                            layer4_dport        INT UNSIGNED,

                            PRIMARY KEY         (sid,cid),
                            INDEX               (signature),
                            INDEX               (sig_name),
                            INDEX               (sig_class_id),
                            INDEX               (sig_priority),
                            INDEX               (timestamp),
                            INDEX               (ip_src),
                            INDEX               (ip_dst),
                            INDEX               (ip_proto),
                            INDEX               (layer4_sport),
                            INDEX               (layer4_dport)
                          );
 

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
	asset_src	INT DEFAULT 1,
	asset_dst	INT DEFAULT 1,
	risk_c		INT DEFAULT 1,
	risk_a		INT DEFAULT 1,
	PRIMARY KEY (sid, cid),
	INDEX		(type),
        INDEX           (priority),
        INDEX           (reliability),
        INDEX           (asset_src),
        INDEX           (asset_dst),
        INDEX           (risk_c),
        INDEX           (risk_a)
);

--
-- Alter Tables acid_event
--
ALTER TABLE acid_event ADD COLUMN ossim_type INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_priority INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_reliability INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_asset_src INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_asset_dst INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_risk_c INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_risk_a INT DEFAULT 1;

CREATE INDEX acid_event_ossim_type ON acid_event (ossim_type);
CREATE INDEX acid_event_ossim_priority ON acid_event (ossim_priority);
CREATE INDEX acid_event_ossim_reliability ON acid_event (ossim_reliability);
CREATE INDEX acid_event_ossim_asset_src ON acid_event (ossim_asset_src);
CREATE INDEX acid_event_ossim_asset_dst ON acid_event (ossim_asset_dst);
CREATE INDEX acid_event_ossim_risk_c ON acid_event (ossim_risk_c);
CREATE INDEX acid_event_ossim_risk_a ON acid_event (ossim_risk_a);

