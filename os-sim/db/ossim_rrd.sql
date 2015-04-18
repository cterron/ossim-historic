DROP TABLE IF EXISTS rrd_conf;
CREATE TABLE rrd_conf (
  ip                        varchar(15) UNIQUE NOT NULL,
  pkt_sent	                varchar(40) NOT NULL,	
  pkt_rcvd       	        varchar(40) NOT NULL,	
  bytes_sent	            varchar(40) NOT NULL,	
  bytes_rcvd	            varchar(40) NOT NULL,	
  tot_contacted_sent_peers	varchar(40) NOT NULL,	
  tot_contacted_rcvd_peers	varchar(40) NOT NULL,	
  ip_dns_sent_bytes	        varchar(40) NOT NULL,	
  ip_dns_rcvd_bytes	        varchar(40) NOT NULL,	
  ip_nbios_ip_sent_bytes	varchar(40) NOT NULL,
  ip_nbios_ip_rcvd_bytes	varchar(40) NOT NULL,
  ip_mail_sent_bytes	    varchar(40) NOT NULL,
  ip_mail_rcvd_bytes	    varchar(40) NOT NULL,
  mrtg_a	                varchar(40) NOT NULL,
  mrtg_c	                varchar(40) NOT NULL,
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
