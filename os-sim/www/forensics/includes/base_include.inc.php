<?php
/**
* Class and Function List:
* Function list:
* Classes list:
*/
/*******************************************************************************
** OSSIM Forensics Console
** Copyright (C) 2009 OSSIM/AlienVault
** Copyright (C) 2004 BASE Project Team
** Copyright (C) 2000 Carnegie Mellon University
**
** (see the file 'base_main.php' for license details)
**
** Built upon work by Roman Danyliw <rdd@cert.org>, <roman@danyliw.com>
** Built upon work by the BASE Project Team <kjohnson@secureideas.net>
**/
defined('_BASE_INC') or die('Accessing this file directly is not allowed.');
include_once ("$BASE_path/includes/base_db.inc.php");
// Get Host names to translate IP -> Host Name
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
require_once ("$BASE_path/includes/SnortHost.inc");
//require_once("$BASE_path/includes/Server.inc");
$sensors = $hosts = $ossim_servers = array();
list($sensors, $hosts) = SnortHost::get_ips_and_hostname($conn);
//$ossim_servers = OServer::get_list($conn);
//$plugins = SnortHost::get_plugin_list($conn);
$db->close($conn);
include_once ("$BASE_path/includes/base_output_html.inc.php");
include_once ("$BASE_path/includes/base_state_common.inc.php");
include_once ("$BASE_path/includes/base_auth.inc.php");
include_once ("$BASE_path/includes/base_user.inc.php");
include_once ("$BASE_path/includes/base_state_query.inc.php");
include_once ("$BASE_path/includes/base_state_criteria.inc.php");
include_once ("$BASE_path/includes/base_output_query.inc.php");
include_once ("$BASE_path/includes/base_log_error.inc.php");
include_once ("$BASE_path/includes/base_log_timing.inc.php");
include_once ("$BASE_path/includes/base_action.inc.php");
include_once ("$BASE_path/base_common.php");
include_once ("$BASE_path/includes/base_cache.inc.php");
include_once ("$BASE_path/includes/base_net.inc.php");
include_once ("$BASE_path/includes/base_signature.inc.php");
?>
