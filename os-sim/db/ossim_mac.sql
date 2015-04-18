DROP TABLE IF EXISTS host_mac;
CREATE TABLE host_mac (
  ip                        varchar(15) UNIQUE NOT NULL,
  mac	                    varchar(255) NOT NULL,	
  previous	                varchar(255) NOT NULL,	
  anom                      int NOT NULL,
  mac_time                 varchar(100) NOT NULL,
  PRIMARY KEY       (ip)
);
