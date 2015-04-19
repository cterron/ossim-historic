-- postfix
-- type: detector
-- plugin_id: 1521
--
-- $Id: postfix.sql,v 1.2 2007/03/26 18:36:15 juanmals Exp $
--
DELETE FROM plugin WHERE id = "1521";
DELETE FROM plugin_sid where plugin_id = "1521";


INSERT INTO plugin (id, type, name, description) VALUES (1521, 1, 'postfix', 'Postfix mailer');

INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1521, 1, NULL, NULL, 'Postfix: relaying denied');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1521, 2, NULL, NULL, 'Postfix: sender domain not found');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1521, 3, NULL, NULL, 'Postfix: recipient user unknown');
INSERT INTO plugin_sid (plugin_id, sid, category_id, class_id, name) VALUES (1521, 5000, NULL, NULL, 'Postfix: blocked using a list');

