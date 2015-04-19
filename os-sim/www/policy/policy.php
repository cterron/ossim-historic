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
Session::logcheck("MenuPolicy", "PolicyPolicy");
session_start();
// load column layout
require_once ('../conf/layout.php');
$category = "policy";
$name_layout = "policy_layout";
$layout = load_layout($name_layout, $category);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
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

	<table class="noborder">
<?php
//create one grid per policy group
require_once 'ossim_db.inc';
require_once 'classes/Policy.inc';
$db = new ossim_db();
$conn = $db->connect();
$policy_groups = Policy::get_policy_groups($conn);
$i = 0;
$refresh = "";
foreach($policy_groups as $group) {
    $refresh.= "$(\"#flextable" . $i . "\").flexReload();\n"
?>
	<tr><td valign="top" id="group<?php echo $group->get_group_id() ?>">
		<table id="flextable<?php echo $i++ ?>" style="display:none"></table>
	</td><tr>
<?php
} ?>
	</table>

	<!-- Right Click Menu -->
	<ul id="myMenu" class="contextMenu">
	    <li class="insertbefore"><a href="#insertbefore">Add Policy Before</a></li>
	    <li class="insertafter"><a href="#insertafter">Add Policy After</a></li>
	    <li class="enabledisable"><a href="#enabledisable">Enable/Disable</a></li>
	</ul>
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
			return document.getElementById(id).offsetWidth-20;
		else
			return 700;
	}
	function action(com,grid,fg,fp) {
		var items = $('.trSelected', grid);
		if (com=='Delete selected') {
			//Delete host by ajax
			if (typeof(items[0]) != 'undefined') {
				var ids='';
				for (var i=0;i<items.length;i++) {
					ids = ids + (ids!='' ? ',' : '') + items[i].id.substr(3);
				}
				//$('.pPageStat',fg.pDiv).html('Deleting host...');
				$.ajax({
						type: "GET",
						url: "deletepolicy.php?confirm=yes&id="+urlencode(ids),
						data: "",
						success: function(msg) {
							fg.populate();
						}
				});
			}
			else alert('You must select a policy');
		}
		else if (com=='Modify') {
			if (typeof(items[0]) != 'undefined') document.location.href = 'newpolicyform.php?id='+urlencode(items[0].id.substr(3))
			else alert('You must select a policy');
		}
		else if (com=='Insert new policy') {
			document.location.href = 'newpolicyform.php?group='+fp.idGroup;
		}
		else if (com=='Reload') {
			document.location.href = '../conf/reload.php?what=policies&back=<?php echo urlencode($_SERVER["REQUEST_URI"]); ?>'
		}
		else if (com=='<b>Enable/Disable</b> policy' || com=='enabledisable') {
			//Activate/Deactivate selected items or all by default via ajax
			if (typeof(items[0]) == 'undefined') items = $('tbody tr', grid);
			var ids='';
			for (var i=0;i<items.length;i++) {
				ids = ids + (ids!='' ? ',' : '') + items[i].id.substr(3);
			}
			$.ajax({
					type: "GET",
					url: "deletepolicy.php?activate=change&id="+urlencode(ids),
					data: "",
					success: function(msg) {
						fg.populate();
					}
			});
		}
	}
	function save_layout(clayout,fg,stat) {
		$.ajax({
				type: "POST",
				url: "../conf/layout.php",
				data: { name: '<?php echo $name_layout ?>', category: '<?php echo $category ?>', layout:serialize(clayout) },
				success: function(msg) {}
		});
	}
	function save_state(p,state) {
		$.ajax({
				type: "POST",
				url: "../conf/layout.php",
				data: { name: 'group'+p.idGroup, category: '<?php echo $category ?>', layout:serialize(state) },
				success: function(msg) {}
		});
	}
	function toggle_group_order(p,state) {
		$.ajax({
				type: "GET",
				url: "changepolicygroup.php",
				data: { group: p.idGroup, order: state },
				success: function(msg) {
					document.location.reload();
				}
		});
	}
	function swap_rows(fg) {
		$.ajax({
				type: "GET",
				url: "changepolicy.php",
				data: { src: fg.drow, dst: fg.hrow },
				success: function(msg) {
					fg.populate();
				}
		});
	}
	function swap_rows_grid(s,d) {
		$.ajax({
				type: "GET",
				url: "changepolicy.php",
				data: { src: s, dst: d },
				success: function(msg) {
					refresh_all();
				}
		});
	}
	function menu_action(com,id,fg,fp) {
		if (com=='enabledisable') {
			//Activate/Deactivate by ajax
			$.ajax({
					type: "GET",
					url: "deletepolicy.php?activate=change&id="+urlencode(id),
					data: "",
					success: function(msg) {
						fg.populate();
					}
			});
		} else if (com=='insertafter') {
			// new policy after selected
			document.location.href = "newpolicyform.php?insertafter="+urlencode(id)+'&group='+fp.userdata1;
		} else if (com=='insertbefore') {
			// new policy before selected
			document.location.href = "newpolicyform.php?insertbefore="+urlencode(id)+'&group='+fp.userdata1;
		}
	}

	$(document).ready(function() {
		var h1w = get_width('headerh1');
<?php
$i = 0;
foreach($policy_groups as $group) {
?>
	$("#flextable<?php echo $i
?>").flexigrid({
		url: 'getpolicy.php?group=<?php echo $group->get_group_id() ?>',
		dataType: 'xml',
		colModel : [
		<?php
    $default = array(
        "active" => array(
            'Act',
            30,
            'true',
            'center',
            false
        ) ,
        "order" => array(
            'Ord',
            30,
            'true',
            'center',
            false
        ) ,
        "priority" => array(
            'Priority',
            40,
            'true',
            'center',
            false
        ) ,
        "source" => array(
            ' <b>Source</b> <img src="../pixmaps/tables/bullet_prev.png" border=0 align=absmiddle>',
            150,
            'false',
            'left',
            false
        ) ,
        "dest" => array(
            ' <b>Destination</b> <img src="../pixmaps/tables/bullet_next.png" border=0 align=absmiddle>',
            150,
            'false',
            'left',
            false
        ) ,
        "port_group" => array(
            'Port Group',
            50,
            'false',
            'center',
            false
        ) ,
        "plugin_group" => array(
            'Plugin Group',
            90,
            'false',
            'center',
            false
        ) ,
        "sensors" => array(
            'Sensors',
            80,
            'false',
            'center',
            false
        ) ,
        "time_range" => array(
            'Time Range',
            100,
            'false',
            'center',
            false
        ) ,
        "targets" => array(
            'Targets',
            70,
            'false',
            'center',
            false
        ) ,
        "desc" => array(
            'Description',
            200,
            'false',
            'left',
            true
        ) ,
        "correlate" => array(
            'Correlate',
            30,
            'false',
            'center',
            false
        ) ,
        "cross correlate" => array(
            'Cross Correlate',
            30,
            'false',
            'center',
            false
        ) ,
        "store" => array(
            'Store',
            30,
            'false',
            'center',
            false
        ) ,
        "qualify" => array(
            'Qualify',
            30,
            'false',
            'center',
            false
        ) ,
        "resend_alarms" => array(
            'Resend Alarms',
            30,
            'false',
            'center',
            false
        ) ,
        "resend_events" => array(
            'Resend Events',
            30,
            'false',
            'center',
            false
        ) ,
        "SIM" => array(
            'Sim',
            25,
            'false',
            'center',
            false
        ) ,
        "SEM" => array(
            'Sem',
            25,
            'false',
            'center',
            false
        ) ,
        "Sign" => array(
            'Sign',
            25,
            'false',
            'center',
            false
        )
    );
    list($colModel, $sortname, $sortorder, $height) = print_layout($layout, $default, "order", "asc", 150);
    echo "$colModel\n";
?>
			],
		buttons : [
			{name: 'Insert new policy', bclass: 'add', onpress : action},
			{separator: true},
			{name: 'Delete selected', bclass: 'delete', onpress : action},
			{separator: true},
			{name: 'Modify', bclass: 'modify', onpress : action},
			{separator: true},
			{name: 'Reload', bclass: '<?php echo (WebIndicator::is_on("Reload_policies")) ? "reload_red" : "reload" ?>', onpress : action},
			{separator: true},
			{name: '<b>Enable/Disable</b> policy', bclass: 'various', onpress : action},
			{separator: true}
			],
		sortname: "<?php echo $sortname ?>",
		sortorder: "<?php echo $sortorder ?>",
		usepager: false,
		title: '<?php echo $group->get_name() . ": <font style=\"font-weight:normal;font-style:italic\">" . $group->get_descr() . "</font>" ?>',
		idGroup: '<?php echo $group->get_group_id() ?>',
		nameGroup: '<?php echo $group->get_name() ?>',
		titleClass: 'mhDiv',
		contextMenu: 'myMenu',
		onContextMenuClick: menu_action,
		pagestat: 'Displaying {from} to {to} of {total} policies',
		nomsg: 'No policies',
		useRp: false,
		showTableToggleBtn: true,
		<?php
    // singleSelect: true,
     ?>
		width: h1w,
		height: <?php echo $height ?>,
		onToggleRow: swap_rows,
		onToggleGrid: swap_rows_grid,
		onTableToggle: save_state,
		<?php
    if ($group->get_group_id() > 0) { ?>
		onUpDown: toggle_group_order,
		uptxt: 'Prioritize policy group: <?php echo $group->get_name() ?>',
		downtxt: 'De-prioritize policy group: <?php echo $group->get_name() ?>',
		<?php
    } ?>
		onColumnChange: save_layout,
		onEndResize: save_layout
	});   
	
<?php
    // load state from user_config
    $state = load_layout("group" . $group->get_group_id() , $category);
    if ($state != "" && !is_array($state)) {
        if ($state == "close") echo "	$(\"#flextable" . $i . "\").viewTableToggle();\n";
    } elseif ($i > 0) echo "	$(\"#flextable" . $i . "\").viewTableToggle();\n";
    $i++;
}
$db->close($conn);
?>
	});
	function refresh_all() {
		<?php echo $refresh
?>
	}
	</script>

</body>
</html>

