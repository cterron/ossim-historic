INSERT INTO config (conf, value) VALUES ('ossim_link', '/ossim/');
INSERT INTO config (conf, value) VALUES ('report_graph_type', 'images');
INSERT INTO config (conf, value) VALUES ('have_scanmap3d', '0');
INSERT INTO config (conf, value) VALUES ('locale_dir', '/usr/share/locale');
INSERT INTO config (conf, value) VALUES ('language', 'en_GB');

DROP TABLE IF EXISTS host_ids;
CREATE TABLE host_ids(
ip              INTEGER UNSIGNED NOT NULL,
date            DATETIME NOT NULL,
hostname        VARCHAR(255) NOT NULL,
sensor          VARCHAR(255) NOT NULL,
sid             INTEGER UNSIGNED NOT NULL,
event_type            VARCHAR(255) NOT NULL,
what            VARCHAR(255) NOT NULL,
target          VARCHAR(255) NOT NULL,
extra_data      VARCHAR(255) NOT NULL,
PRIMARY KEY     (ip,target,sid,date)
);

DROP TABLE IF EXISTS sensor_interfaces;
CREATE TABLE sensor_interfaces (
    sensor  varchar(64) NOT NULL,
    interface varchar(64) NOT NULL,
    name    varchar(255) NOT NULL,
    main    int NOT NULL,
    PRIMARY KEY (sensor, interface)
);

--
-- FW1
--

UPDATE `plugin_sid` SET `category_id` = '203' WHERE `plugin_id` = '1504' AND `sid` = '3';
UPDATE `plugin_sid` SET `category_id` = '204' WHERE `plugin_id` = '1504' AND `sid` = '2';

--
-- IPtables
--

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1503, 4, NULL, NULL, 'iptables: traffic inbound');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1503, 5, NULL, NULL, 'iptables: traffic outbound');


--
-- Snare for Windows plugin
--

INSERT INTO plugin (id, type, name, description) VALUES (1518, 1, 'snarewindows', 'Snare Agent for Windows');

-- 
-- Snare for Windows Sids
--

-- Audit Privilege Use
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 576, NULL, NULL, "Snare Agent for Windows: Special privileges assigned to new loggon");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 577, NULL, NULL, "Snare Agent for Windows: Privileged Service Called");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 578, NULL, NULL, "Snare Agent for Windows: Privileged object operation");

-- Audit Process Tracking
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 592, NULL, NULL, "Snare Agent for Windows: A new process has been created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 593, NULL, NULL, "Snare Agent for Windows: A process has exited");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 594, NULL, NULL, "Snare Agent for Windows: A handle to an object has been duplicated");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 595, NULL, NULL, "Snare Agent for Windows: Indirect access to an object has been obtained");

-- Audit System Events
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 512, NULL, NULL, "Snare Agent for Windows: Windows NT is starting up");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 513, NULL, NULL, "Snare Agent for Windows: Windows NT is shutting down");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 514, NULL, NULL, "Snare Agent for Windows: An authentication package has been loaded");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 515, NULL, NULL, "Snare Agent for Windows: A trusted logon process has registered");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 516, NULL, NULL, "Snare Agent for Windows: Loss of some audits");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 517, NULL, NULL, "Snare Agent for Windows: The audit log was cleared");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 518, NULL, NULL, "Snare Agent for Windows: A notification package has been loaded");

-- Audit Logon Events
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 528, NULL, NULL, "Snare Agent for Windows: A user successfully logged on to a computer");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 529, NULL, NULL, "Snare Agent for Windows: The logon attempt was made with an unknown user name or bad password");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 530, NULL, NULL, "Snare Agent for Windows: The user account tried to log on outside of the allowed time");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 531, NULL, NULL, "Snare Agent for Windows: A logon attempt was made using a disabled account");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 532, NULL, NULL, "Snare Agent for Windows: A logon attempt was made using an expired account");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 533, NULL, NULL, "Snare Agent for Windows: The user is not allowed to log on at this computer");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 534, NULL, NULL, "Snare Agent for Windows: The user attempted to log on with a logon type that is not allowed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 535, NULL, NULL, "Snare Agent for Windows: The password for the specified account has expired");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 536, NULL, NULL, "Snare Agent for Windows: The Net Logon service is not active");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 537, NULL, NULL, "Snare Agent for Windows: The logon attempt failed for other reasons");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 538, NULL, NULL, "Snare Agent for Windows: A user logged off");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 539, NULL, NULL, "Snare Agent for Windows: The account was locked out at the time the logon attempt was made");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 540, NULL, NULL, "Snare Agent for Windows: Successful Network Logon");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 541, NULL, NULL, "Snare Agent for Windows: IPSec security association established");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 542, NULL, NULL, "Snare Agent for Windows: IPSec security association ended");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 543, NULL, NULL, "Snare Agent for Windows: IPSec security association ended");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 544, NULL, NULL, "Snare Agent for Windows: IPSec security association establishment failed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 545, NULL, NULL, "Snare Agent for Windows: IPSec peer authentication failed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 546, NULL, NULL, "Snare Agent for Windows: IPSec security association establishment failed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 547, NULL, NULL, "Snare Agent for Windows: IPSec security association negotiation failed");

-- Audit Account Logon Events
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 672, NULL, NULL, "Snare Agent for Windows: An authentication service (AS) ticket was successfully issued and validated");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 673, NULL, NULL, "Snare Agent for Windows: A ticket granting service (TGS) ticket was granted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 674, NULL, NULL, "Snare Agent for Windows: A security principal renewed an AS ticket or TGS ticket");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 675, NULL, NULL, "Snare Agent for Windows: Pre-authentication failed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 676, NULL, NULL, "Snare Agent for Windows: Authentication Ticket Request Failed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 677, NULL, NULL, "Snare Agent for Windows: A TGS ticket was not granted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 678, NULL, NULL, "Snare Agent for Windows: An account was successfully mapped to a domain account");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 680, NULL, NULL, "Snare Agent for Windows: Identifies the account used for the successful logon attempt");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 681, NULL, NULL, "Snare Agent for Windows: A domain account log on was attempted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 682, NULL, NULL, "Snare Agent for Windows: A user has reconnected to a disconnected Terminal Services session");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 683, NULL, NULL, "Snare Agent for Windows: A user disconnected a Terminal Services session without logging off");

-- Audit Account Management Events
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 624, NULL, NULL, "Snare Agent for Windows: User Account Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 625, NULL, NULL, "Snare Agent for Windows: User Account Type Change");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 626, NULL, NULL, "Snare Agent for Windows: User Account Enabled");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 627, NULL, NULL, "Snare Agent for Windows: Password Change Attempted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 628, NULL, NULL, "Snare Agent for Windows: User Account Password Set");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 629, NULL, NULL, "Snare Agent for Windows: User Account Disabled");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 630, NULL, NULL, "Snare Agent for Windows: User Account Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 631, NULL, NULL, "Snare Agent for Windows: Security Enabled Global Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 632, NULL, NULL, "Snare Agent for Windows: Security Enabled Global Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 633, NULL, NULL, "Snare Agent for Windows: Security Enabled Global Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 634, NULL, NULL, "Snare Agent for Windows: Security Enabled Global Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 635, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 636, NULL, NULL, "Snare Agent for Windows: Security Enabled Local Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 637, NULL, NULL, "Snare Agent for Windows: Security Enabled Local Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 638, NULL, NULL, "Snare Agent for Windows: Security Enabled Local Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 639, NULL, NULL, "Snare Agent for Windows: Security Enabled Local Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 640, NULL, NULL, "Snare Agent for Windows: General Account Database Change");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 641, NULL, NULL, "Snare Agent for Windows: Security Enabled Global Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 642, NULL, NULL, "Snare Agent for Windows: User Account Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 643, NULL, NULL, "Snare Agent for Windows: Domain Policy Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 644, NULL, NULL, "Snare Agent for Windows: User Account Locked Out");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 645, NULL, NULL, "Snare Agent for Windows: Computer object added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 646, NULL, NULL, "Snare Agent for Windows: Computer object changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 647, NULL, NULL, "Snare Agent for Windows: Computer object deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 648, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 649, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 650, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 651, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 652, NULL, NULL, "Snare Agent for Windows: Security Disabled Local Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 653, NULL, NULL, "Snare Agent for Windows: Security Disabled Global Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 654, NULL, NULL, "Snare Agent for Windows: Security Disabled Global Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 655, NULL, NULL, "Snare Agent for Windows: Security Disabled Global Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 656, NULL, NULL, "Snare Agent for Windows: Security Disabled Global Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 657, NULL, NULL, "Snare Agent for Windows: Security Disabled Global Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 658, NULL, NULL, "Snare Agent for Windows: Security Enabled Universal Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 659, NULL, NULL, "Snare Agent for Windows: Security Enabled Universal Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 660, NULL, NULL, "Snare Agent for Windows: Security Enabled Universal Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 661, NULL, NULL, "Snare Agent for Windows: Security Enabled Universal Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 662, NULL, NULL, "Snare Agent for Windows: Security Enabled Universal Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 663, NULL, NULL, "Snare Agent for Windows: Security Disabled Universal Group Created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 664, NULL, NULL, "Snare Agent for Windows: Security Disabled Universal Group Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 665, NULL, NULL, "Snare Agent for Windows: Security Disabled Universal Group Member Added");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 666, NULL, NULL, "Snare Agent for Windows: Security Disabled Universal Group Member Removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 667, NULL, NULL, "Snare Agent for Windows: Security Disabled Universal Group Deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 668, NULL, NULL, "Snare Agent for Windows: Group Type Changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 669, NULL, NULL, "Snare Agent for Windows: Add SID History (Success)");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 670, NULL, NULL, "Snare Agent for Windows: Add SID History (Failure)");

-- Audit Object Access
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 560, NULL, NULL, "Snare Agent for Windows: Access was granted to an already existing object");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 561, NULL, NULL, "Snare Agent for Windows: A handle to an object was allocated");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 562, NULL, NULL, "Snare Agent for Windows: A handle to an object was closed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 563, NULL, NULL, "Snare Agent for Windows: An attempt was made to open an object with the intent to delete it");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 564, NULL, NULL, "Snare Agent for Windows: A protected object was deleted");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 565, NULL, NULL, "Snare Agent for Windows: Access was granted to an already existing object type");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 566, NULL, NULL, "Snare Agent for Windows: Object Operation");

-- Audit Policy Change
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 608, NULL, NULL, "Snare Agent for Windows: A user right was assigned");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 609, NULL, NULL, "Snare Agent for Windows: A user right was removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 610, NULL, NULL, "Snare Agent for Windows: A trust relationship with another domain was created");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 611, NULL, NULL, "Snare Agent for Windows: A trust relationship with another domain was removed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 612, NULL, NULL, "Snare Agent for Windows: An audit policy was changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 613, NULL, NULL, "Snare Agent for Windows: IPSec policy agent started");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 614, NULL, NULL, "Snare Agent for Windows: IPSec policy agent disabled");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 615, NULL, NULL, "Snare Agent for Windows: IPSec policy changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 616, NULL, NULL, "Snare Agent for Windows: IPSec policy agent encountered a potentially serious failure");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 617, NULL, NULL, "Snare Agent for Windows: Kerberos policy changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 618, NULL, NULL, "Snare Agent for Windows: Encrypted data recovery policy changed");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 620, NULL, NULL, "Snare Agent for Windows: Trusted domain information modified");
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1518, 768, NULL, NULL, "Snare Agent for Windows: A collision was detected between a namespace element in two forests");


