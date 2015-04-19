<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* Classes list:
*/
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
// load column layout
require_once ('../conf/layout.php');
$category = "policy";
$name_layout = "sensors_layout";
$layout = load_layout($name_layout, $category);
// data
require_once 'ossim_db.inc';
require_once 'get_sensors.php';
require_once 'classes/Sensor.inc';
$active_sensors = 0;
$total_sensors = 0;
$sensor_stack = array();
$sensor_stack_on = array();
$sensor_stack_off = array();
$sensor_configured_stack = array();
$db = new ossim_db();
$conn = $db->connect();
list($sensor_list, $err) = server_get_sensors($conn);
if ($err != "") echo $err;
foreach($sensor_list as $sensor_status) {
    if ($sensor_status["state"] = "on") {
        array_push($sensor_stack_on, $sensor_status["sensor"]);
        $sensor_stack[$sensor_status["sensor"]] = 1;
    } else {
        array_push($sensor_stack_off, $sensor_status["sensor"]);
    }
}
if ($sensor_list = Sensor::get_list($conn, "")) {
    $total_sensors = count($sensor_list);
    foreach($sensor_list as $sensor) {
        if ($sensor_stack[$sensor->get_ip() ] == 1) {
            $active_sensors++;
            array_push($sensor_configured_stack, $sensor->get_ip());
        }
    }
}
$active_sensors = ($active_sensors == 0) ? "<font color=red><b>$active_sensors</b></font>" : "<font color=green><b>$active_sensors</b></font>";
$total_sensors = "<b>$total_sensors</b>";
$db->close($conn);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <meta http-equiv="X-UA-Compatible" content="IE=7" />
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <link rel="stylesheet" type="text/css" href="../style/flexigrid.css"/>
  <script type="text/javascript" src="../js/jquery-1.3.2.min.js"></script>
  <script type="text/javascript" src="../js/jquery.flexigrid.js"></script>
  <script type="text/javascript" src="../js/urlencode.js"></script>
</head>
<body>
                                                                                
	<?php
include ("../hmenu.php"); ?>
	<div  id="headerh1" style="width:100%;height:1px">&nbsp;</div>

<?php
//$sensor_stack_on[] = "192.168.1.2";
$diff_arr = array_diff($sensor_stack_on, $sensor_configured_stack);
if ($diff_arr) {
?>
	<table class="noborder"><tr>
	<td><font color="red"><b> <?php
    echo gettext("Warning"); ?> </b></font>:
		<?php
    echo gettext("the following sensor(s) are being reported as enabled by the server but aren't configured"); ?> .
	</td>
	</tr></table>

	<table class="noborder">
	<?php
    foreach($diff_arr as $ip_diff) { ?>
	<tr>
	<td nowrap><img src="../pixmaps/theme/host.png" border=0 align="absmiddle"><a href="sensor_plugins.php?sensor=<?php
        echo $ip_diff ?>"><b><?php
        echo $ip_diff ?></b></a>&nbsp;</td>
	<td nowrap style="background:#E8E8E8;border:1px solid #D7D7D7">&nbsp;<a href="newsensorform.php?ip=<?php
        echo $ip_diff ?>"><img src="../pixmaps/tables/table_row_insert.png" border=0 align="absmiddle"> <?php
        echo gettext("Insert"); ?> </a>&nbsp;</td>
	</tr>
	<tr><td colspan="2"></td></tr>
	<?php
    } ?>
	</table>
<?php
} ?>

  
	<table class="noborder">
	<tr><td valign="top">
		<table id="flextable" style="display:none"></table>
	</td><tr>
	</table>
	<style>
		table, th, tr, td {
			background:transparent;
			border-radius: 0px;
			-moz-border-radius: 0px;
			-webkit-border-radius: 0px;
			border:none;
			padding:0px; margin:0px;
		}
		input, select {
			border-radius: 0px;
			-moz-border-radius: 0px;
			-webkit-border-radius: 0px;
			border: 1px solid #8F8FC6;
			font-size:12px; font-family:arial; vertical-align:middle;
			padding:0px; margin:0px;
		}
	</style>
	<script>
	function get_width(id) {
		if (typeof(document.getElementById(id).offsetWidth)!='undefined') 
			return document.getElementById(id).offsetWidth-5;
		else
			return 700;
	}
	function action(com,grid) {
		var items = $('.trSelected', grid);
		if (com=='Delete selected') {
			//Delete host by ajax
			if (typeof(items[0]) != 'undefined') {
				document.location.href = 'deletesensor.php?confirm=yes&name='+urlencode(items[0].id.substr(3))
			}
			else alert('You must select a sensor');
		}
		else if (com=='Modify') {
			if (typeof(items[0]) != 'undefined') document.location.href = 'modifysensorform.php?name='+urlencode(items[0].id.substr(3))
			else alert('You must select a sensor');
		}
		else if (com=='Insert new sensor') {
			document.location.href = 'newsensorform.php'
		}
		else if (com=='Reload') {
			document.location.href = '../conf/reload.php?what=sensors&back=<?php echo urlencode($_SERVER["REQUEST_URI"]); ?>'
		}
		else if (com=='Interfaces') {
			document.location.href = 'interfaces.php?sensor='+urlencode(items[0].id.substr(3))
		}
	}
	function save_layout(clayout) {
		$("#flextable").changeStatus('Saving column layout...',false);
		$.ajax({
				type: "POST",
				url: "../conf/layout.php",
				data: { name:"<?php echo $name_layout ?>", category:"<?php echo $category ?>", layout:serialize(clayout) },
				success: function(msg) {
					$("#flextable").changeStatus(msg,true);
				}
		});
	}
	$("#flextable").flexigrid({
		url: 'getsensor.php',
		dataType: 'xml',
		colModel : [
		<?php
$default = array(
    "ip" => array(
        'IP',
        100,
        'true',
        'center',
        false
    ) ,
    "name" => array(
        'Hostname',
        100,
        'true',
        'center',
        false
    ) ,
    "priority" => array(
        'Priority',
        60,
        'true',
        'center',
        false
    ) ,
    "port" => array(
        'Port',
        40,
        'true',
        'center',
        false
    ) ,
    "version" => array(
        'Version',
        180,
        'false',
        'center',
        false
    ) ,
    "active" => array(
        'Active',
        50,
        'false',
        'center',
        false
    ) ,
    //"munin" => array('Munin',40,'false','center',false),
    "desc" => array(
        'Description',
        280,
        'false',
        'left',
        false
    )
);
list($colModel, $sortname, $sortorder, $height) = print_layout($layout, $default, "name", "asc", 300);
echo "$colModel\n";
?>
			],
		buttons : [
			{name: 'Insert new sensor', bclass: 'add', onpress : action},
			{separator: true},
			{name: 'Delete selected', bclass: 'delete', onpress : action},
			{separator: true},
			{name: 'Modify', bclass: 'modify', onpress : action},
			{separator: true},
			{name: 'Interfaces', bclass: 'gear', onpress : action},
			{separator: true},
			{name: 'Reload', bclass: '<?php echo (WebIndicator::is_on("Reload_sensors")) ? "reload_red" : "reload" ?>', onpress : action},
			{separator: true},
			{name: 'Active Sensors: <?php echo $active_sensors ?>', bclass: 'info', iclass: 'ibutton'},
			{name: 'Total Sensors: <?php echo $total_sensors ?>', bclass: 'info', iclass: 'ibutton'}
			],
		sortname: "<?php echo $sortname ?>",
		sortorder: "<?php echo $sortorder ?>",
		usepager: true,
		title: 'SENSORS',
		pagestat: 'Displaying {from} to {to} of {total} sensors',
		nomsg: 'No sensors',
		useRp: true,
		rp: 25,
		showTableToggleBtn: true,
		singleSelect: true,
		width: get_width('headerh1'),
		height: <?php echo $height ?>,
		onColumnChange: save_layout,
		onEndResize: save_layout
	});   
	
	</script>

</body>
</html>

