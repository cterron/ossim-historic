-- -----------------------------------------------------
-- Table `extra_data`
-- -----------------------------------------------------
RENAME TABLE extra_data TO _extra_data;
CREATE TABLE `extra_data` (
  `event_id` BINARY(16) NOT NULL,
  `filename` VARCHAR(256) NULL DEFAULT NULL,
  `username` VARCHAR(64) NULL DEFAULT NULL,
  `password` VARCHAR(64) NULL DEFAULT NULL,
  `userdata1` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata2` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata3` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata4` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata5` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata6` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata7` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata8` VARCHAR(1024) NULL DEFAULT NULL,
  `userdata9` VARCHAR(1024) NULL DEFAULT NULL,
  `data_payload` TEXT NULL DEFAULT NULL,
  `binary_data` BLOB NULL DEFAULT NULL,
  PRIMARY KEY (`event_id`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `reputation_data`
-- -----------------------------------------------------
RENAME TABLE reputation_data TO _reputation_data;
CREATE TABLE `reputation_data` (
  `event_id` BINARY(16) NOT NULL,
  `rep_ip_src` VARBINARY(16) NULL DEFAULT NULL,
  `rep_ip_dst` VARBINARY(16) NULL DEFAULT NULL,
  `rep_prio_src` TINYINT UNSIGNED NULL DEFAULT NULL,
  `rep_prio_dst` TINYINT UNSIGNED NULL DEFAULT NULL,
  `rep_rel_src` TINYINT UNSIGNED NULL DEFAULT NULL,
  `rep_rel_dst` TINYINT UNSIGNED NULL DEFAULT NULL,
  `rep_act_src` VARCHAR(64) NULL DEFAULT NULL,
  `rep_act_dst` VARCHAR(64) NULL DEFAULT NULL,
  PRIMARY KEY (`event_id`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


-- -----------------------------------------------------
-- Table `idm_data`
-- -----------------------------------------------------
RENAME TABLE idm_data TO _idm_data;
CREATE TABLE `idm_data` (
  `event_id` BINARY(16) NOT NULL,
  `username` VARCHAR(64) NULL DEFAULT NULL,
  `domain` VARCHAR(64) NULL DEFAULT NULL,
  `from_src` TINYINT(1) NULL DEFAULT NULL,
  INDEX `event_id` (`event_id` ASC),
  INDEX `usrdmn` (`username` ASC, `domain` ASC),
  INDEX `domain` (`domain` ASC))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


CREATE TABLE IF NOT EXISTS tmp_events (id binary(16) NOT NULL, PRIMARY KEY (`id`)) ENGINE = InnoDB;

SELECT sleep(10) into @sleep;


-- -----------------------------------------------------
-- Table `ac_acid_event`
-- -----------------------------------------------------
RENAME TABLE ac_acid_event TO _ac_acid_event;
CREATE TABLE `ac_acid_event` (
  `ctx` BINARY(16) NOT NULL,
  `device_id` INT UNSIGNED NOT NULL,
  `plugin_id` INT UNSIGNED NOT NULL,
  `plugin_sid` INT UNSIGNED NOT NULL,
  `day` DATE NOT NULL,
  `src_host` BINARY(16) NOT NULL DEFAULT 0x0,
  `dst_host` BINARY(16) NOT NULL DEFAULT 0x0,
  `src_net` BINARY(16) NOT NULL DEFAULT 0x0,
  `dst_net` BINARY(16) NOT NULL DEFAULT 0x0,
  `cnt` INT UNSIGNED NOT NULL DEFAULT 0,
  INDEX `day` (`day` ASC),
  INDEX `plugin_id` (`plugin_id` ASC),
  PRIMARY KEY (`ctx`, `device_id`, `plugin_id`, `plugin_sid`, `day`, `src_host`, `dst_net`, `dst_host`, `src_net`),
  INDEX `src_host` (`src_host` ASC),
  INDEX `dst_host` (`dst_host` ASC))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

-- -----------------------------------------------------
-- Table `ah_acid_event`
-- -----------------------------------------------------
RENAME TABLE ah_acid_event TO _ah_acid_event;
CREATE TABLE IF NOT EXISTS `ah_acid_event` (
  `ctx` BINARY(16) NOT NULL,
  `device_id` INT UNSIGNED NOT NULL,
  `timestamp` DATETIME NOT NULL,
  `src_host` BINARY(16) NOT NULL DEFAULT 0x0,
  `dst_host` BINARY(16) NOT NULL DEFAULT 0x0,
  `src_net` BINARY(16) NOT NULL DEFAULT 0x0,
  `dst_net` BINARY(16) NOT NULL DEFAULT 0x0,
  `cnt` INT UNSIGNED NOT NULL DEFAULT 0,
  INDEX `day` (`timestamp` ASC),
  PRIMARY KEY (`ctx`, `device_id`, `timestamp`, `src_host`, `dst_net`, `dst_host`, `src_net`),
  INDEX `src_host` (`src_host` ASC),
  INDEX `dst_host` (`dst_host` ASC))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

-- -----------------------------------------------------
-- Table `acid_event`
-- -----------------------------------------------------
RENAME TABLE acid_event TO _acid_event;
CREATE TABLE `acid_event` (
  `id` BINARY(16) NOT NULL,
  `device_id` INT UNSIGNED NOT NULL,
  `ctx` BINARY(16) NOT NULL DEFAULT 0x0,
  `timestamp` DATETIME NOT NULL,
  `ip_src` VARBINARY(16) NULL DEFAULT NULL,
  `ip_dst` VARBINARY(16) NULL DEFAULT NULL,
  `ip_proto` INT NULL DEFAULT NULL,
  `layer4_sport` SMALLINT UNSIGNED NULL DEFAULT NULL,
  `layer4_dport` SMALLINT UNSIGNED NULL DEFAULT NULL,
  `ossim_priority` TINYINT NULL DEFAULT '1',
  `ossim_reliability` TINYINT NULL DEFAULT '1',
  `ossim_asset_src` TINYINT NULL DEFAULT '1',
  `ossim_asset_dst` TINYINT NULL DEFAULT '1',
  `ossim_risk_c` TINYINT NULL DEFAULT '1',
  `ossim_risk_a` TINYINT NULL DEFAULT '1',
  `plugin_id` INT UNSIGNED NULL DEFAULT NULL,
  `plugin_sid` INT UNSIGNED NULL DEFAULT NULL,
  `tzone` FLOAT NOT NULL DEFAULT '0',
  `ossim_correlation` TINYINT NULL DEFAULT '0',
  `src_hostname` VARCHAR(64) NULL DEFAULT NULL,
  `dst_hostname` VARCHAR(64) NULL DEFAULT NULL,
  `src_mac` BINARY(6) NULL DEFAULT NULL,
  `dst_mac` BINARY(6) NULL DEFAULT NULL,
  `src_host` BINARY(16) NULL DEFAULT NULL,
  `dst_host` BINARY(16) NULL DEFAULT NULL,
  `src_net` BINARY(16) NULL DEFAULT NULL,
  `dst_net` BINARY(16) NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  INDEX `timestamp` (`timestamp` ASC),
  INDEX `layer4_sport` (`layer4_sport` ASC),
  INDEX `layer4_dport` (`layer4_dport` ASC),
  INDEX `ip_src` (`ip_src` ASC),
  INDEX `ip_dst` (`ip_dst` ASC),
  INDEX `acid_event_ossim_priority` (`ossim_priority` ASC),
  INDEX `acid_event_ossim_risk_a` (`ossim_risk_a` ASC),
  INDEX `acid_event_ossim_reliability` (`ossim_reliability` ASC),
  INDEX `acid_event_ossim_risk_c` (`ossim_risk_c` ASC),
  INDEX `plugin` (`plugin_id` ASC, `plugin_sid` ASC))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;


DELIMITER $$

DROP TRIGGER IF EXISTS `count_acid_event`$$
CREATE TRIGGER `count_acid_event` AFTER INSERT ON `acid_event` FOR EACH ROW
-- Edit trigger body code below this line. Do not edit lines above this one
BEGIN

  INSERT IGNORE INTO ac_acid_event (ctx, device_id, plugin_id, plugin_sid, day, src_host, dst_host, src_net, dst_net, cnt)
  VALUES (NEW.ctx, NEW.device_id, NEW.plugin_id, NEW.plugin_sid, DATE(NEW.timestamp), NEW.src_host, NEW.dst_host, NEW.src_net, NEW.dst_net, 1)
  ON DUPLICATE KEY UPDATE cnt = cnt + 1;

  INSERT IGNORE INTO ah_acid_event (ctx, device_id, timestamp, src_host, dst_host, src_net, dst_net, cnt)
  VALUES (NEW.ctx, NEW.device_id, DATE_FORMAT(NEW.timestamp, '%Y-%m-%d %H:00:00'), NEW.src_host, NEW.dst_host, NEW.src_net, NEW.dst_net, 1)
  ON DUPLICATE KEY UPDATE cnt = cnt + 1;

END
$$

DROP TRIGGER IF EXISTS `del_count_acid_event`$$
CREATE TRIGGER `del_count_acid_event` AFTER DELETE ON `acid_event` FOR EACH ROW
-- Edit trigger body code below this line. Do not edit lines above this one
BEGIN

  UPDATE ac_acid_event SET cnt = cnt - 1
  WHERE ctx = OLD.ctx AND device_id = OLD.device_id AND plugin_id = OLD.plugin_id AND plugin_sid = OLD.plugin_sid
     AND day = DATE(OLD.timestamp)
     AND src_host = IFNULL(OLD.src_host, 0x00000000000000000000000000000000)
     AND dst_host = IFNULL(OLD.dst_host, 0x00000000000000000000000000000000)
     AND src_net = IFNULL(OLD.src_net, 0x00000000000000000000000000000000)
     AND dst_net = IFNULL(OLD.dst_net, 0x00000000000000000000000000000000) AND cnt > 0;

  UPDATE ah_acid_event SET cnt = cnt - 1
  WHERE ctx = OLD.ctx AND device_id = OLD.device_id
     AND timestamp = DATE_FORMAT(OLD.timestamp, '%Y-%m-%d %H:00:00')
     AND src_host = IFNULL(OLD.src_host, 0x00000000000000000000000000000000)
     AND dst_host = IFNULL(OLD.dst_host, 0x00000000000000000000000000000000)
     AND src_net = IFNULL(OLD.src_net, 0x00000000000000000000000000000000)
     AND dst_net = IFNULL(OLD.dst_net, 0x00000000000000000000000000000000) AND cnt > 0;

END
$$

DROP PROCEDURE IF EXISTS _clean_devices$$
CREATE PROCEDURE _clean_devices()
BEGIN
    ALTER TABLE `device` CHANGE `interface` `interface` TEXT DEFAULT NULL;
    SELECT ".";
    ALTER TABLE _acid_event ADD INDEX device (device_id);
    SELECT ".";
    DELETE d FROM device d LEFT JOIN _acid_event a on a.device_id=d.id LEFT JOIN acid_event ae on ae.device_id=d.id WHERE a.id IS NULL AND ae.id IS NULL;
END$$

DROP PROCEDURE IF EXISTS _delete_orphans$$
CREATE PROCEDURE _delete_orphans()
BEGIN
    DECLARE num_events INT;

    -- Select valid events
    TRUNCATE TABLE alienvault_siem.tmp_events;
    INSERT INTO alienvault_siem.tmp_events SELECT id FROM alienvault_siem._acid_event;

    CREATE TEMPORARY TABLE _ttmp_events (id binary(16) NOT NULL, PRIMARY KEY (`id`)) ENGINE=MEMORY;

    SELECT count(id) FROM alienvault_siem.tmp_events INTO @num_events;
    SELECT ".";
    
    WHILE @num_events > 0 DO
       INSERT IGNORE INTO _ttmp_events SELECT id FROM alienvault_siem.tmp_events LIMIT 100000;
       INSERT IGNORE INTO alienvault_siem.reputation_data SELECT aux.* FROM alienvault_siem._reputation_data aux, _ttmp_events t WHERE aux.event_id=t.id;
       INSERT IGNORE INTO alienvault_siem.idm_data SELECT aux.* FROM alienvault_siem._idm_data aux, _ttmp_events t WHERE aux.event_id=t.id;
       INSERT IGNORE INTO alienvault_siem.extra_data SELECT aux.* FROM alienvault_siem._extra_data aux, _ttmp_events t WHERE aux.event_id=t.id;
       INSERT IGNORE INTO alienvault_siem.acid_event SELECT aux.* FROM alienvault_siem._acid_event aux, _ttmp_events t WHERE aux.id=t.id;
       DELETE tt FROM alienvault_siem.tmp_events tt LEFT JOIN _ttmp_events tmp ON tmp.id=tt.id WHERE tmp.id IS NOT NULL;
       TRUNCATE TABLE _ttmp_events;
       SELECT ".";
       SET @num_events = @num_events - 100000;
    END WHILE;
END$$

DELIMITER ;
