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
	PRIMARY KEY (sid, cid)
);

--
-- Alter Tables acid_event
--
ALTER TABLE acid_event ADD COLUMN ossim_type INT DEFAULT 0;
ALTER TABLE acid_event ADD COLUMN ossim_priority INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_reliability INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_asset_src INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_asset_dst INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_risk_c INT DEFAULT 1;
ALTER TABLE acid_event ADD COLUMN ossim_risk_a INT DEFAULT 1;
