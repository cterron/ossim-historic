DROP TABLE IF EXISTS host_os;
CREATE TABLE host_os (
  ip                        varchar(15) UNIQUE NOT NULL,
  os	                    varchar(255) NOT NULL,	
  previous	                varchar(255) NOT NULL,	
  anom                      int NOT NULL,
  os_time                 varchar(100) NOT NULL,
  PRIMARY KEY       (ip)
);
