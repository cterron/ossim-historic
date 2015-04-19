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
* - getEvents()
* - resize_text()
* - write_openandclose()
* - build_url()
* - write_trees()
* - create_tree()
* - create_tree_child()
* - create_alarm_table()
* Classes list:
*/
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAlarms");
require_once ('ossim_db.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/AlarmGroup.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');
require_once ('classes/Port.inc');
require_once ('classes/Util.inc');
require_once ('classes/Security.inc');
require_once 'classes/Xajax.inc';
include ("geoip.inc");
$gi = geoip_open("/usr/share/geoip/GeoIP.dat", GEOIP_STANDARD);
/* GET VARIABLES FROM URL */
//$ROWS = 100;
$ROWS = 10;
$db = new ossim_db();
$conn = $db->connect();
// Xajax . Register function getEvents
$xajax = new xajax();
$xajax->registerFunction("getEvents");
$xajax->processRequests();
$delete = GET('delete');
$delete_group = GET('delete_group');
$close = GET('close');
$delete_day = GET('delete_day');
$order = GET('order');
$src_ip = GET('src_ip');
$dst_ip = GET('dst_ip');
$backup_inf = $inf = GET('inf');
$sup = GET('sup');
$hide_closed = GET('hide_closed');
$date_from = preg_replace("/(\d\d)\/(\d\d)\/(\d\d\d\d)/", "\\3-\\1-\\2", GET('date_from'));
$date_to = preg_replace("/(\d\d)\/(\d\d)\/(\d\d\d\d)/", "\\3-\\1-\\2", GET('date_to'));
$num_alarms_page = GET('num_alarms_page');
$disp = GET('disp'); // Telefonica disponibilidad hack
$group = GET('group'); // Alarm group for change descr
$new_descr = GET('descr');
$action = GET('action');
$show_options = GET('show_options');
$refresh_time = GET('refresh_time');
$autorefresh = GET('autorefresh');
$alarm = GET('alarm');
ossim_valid($disp, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("disp"));
ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("order"));
ossim_valid($delete, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("delete"));
ossim_valid($delete_group, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("delete"));
ossim_valid($close, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("close"));
ossim_valid($open, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("open"));
ossim_valid($delete_day, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("delete_day"));
$ret1 = ossim_valid($src_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:' . _("src_ip"));
$ret2 = ossim_valid($src_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:' . _("src_ip"));
if (!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
$ret3 = ossim_valid($dst_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:' . _("dst_ip"));
$ret4 = ossim_valid($dst_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:' . _("dst_ip"));
if (!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
ossim_valid($inf, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("inf"));
ossim_valid($sup, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("order"));
ossim_valid($hide_closed, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("hide_closed"));
ossim_valid($autorefresh, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("autorefresh"));
ossim_valid($date_from, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("from date"));
ossim_valid($date_to, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("to date"));
ossim_valid($num_alarms_page, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("field number of alarms per page"));
ossim_valid($group, OSS_DIGIT, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("group"));
ossim_valid($new_descr, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, OSS_PUNC, 'illegal:' . _("descr"));
ossim_valid($show_options, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("show_options"));
ossim_valid($refresh_time, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("refresh_time"));
ossim_valid($alarm, OSS_DIGIT, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("alarm"));
//action=change_descr
ossim_valid($action, OSS_ALPHA, OSS_NULLABLE, OSS_PUNC, 'illegal:' . _("action"));
if (ossim_error()) {
    die(ossim_error());
}
if (!empty($delete)) {
    Alarm::delete($conn, $delete);
}
if (!empty($delete_group)) {
    AlarmGroup::delete_groups($conn, $delete_group);
}
if ($list = GET('delete_backlog')) {
    if (!strcmp($list, "all")) {
        $backlog_id = $list;
        $id = null;
    } else {
        list($backlog_id, $id) = split("-", $list);
    }
    Alarm::delete_from_backlog($conn, $backlog_id, $id);
}
if (empty($order)) $order = " timestamp DESC";
if ((!empty($src_ip)) && (!empty($dst_ip))) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip' 
                     OR inet_ntoa(dst_ip) = '$dst_ip'";
} elseif (!empty($src_ip)) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip'";
} elseif (!empty($dst_ip)) {
    $where = "WHERE inet_ntoa(dst_ip) = '$dst_ip'";
} else {
    $where = '';
}
if ($hide_closed == 'on') {
    $hide_closed = 1;
} else {
    $hide_closed = 0;
}
if ($autorefresh == 'on') {
    $autorefresh = 1;
} else {
    $autorefresh = 0;
}
if ($num_alarms_page) {
    $ROWS = $num_alarms_page;
}
if (empty($inf)) $inf = 0;
if (!$sup) $sup = $ROWS;
if (empty($show_options) || ($show_options < 1 || $show_options > 4)) {
    $show_options = 1;
}
if (empty($refresh_time) || ($refresh_time != 30 && $refresh_time != 60 && $refresh_time != 180 && $refresh_time != 600)) {
    $refresh_time = 60;
}
switch ($action) {
    case "change_descr":
        // Change alarm description
        if (!empty($group)) {
            AlarmGroup::change_descr($conn, $group, $new_descr);
        }
        break;

    case "take_alarm":
        // Take alarm number
        if (!empty($group)) {
            AlarmGroup::take_alarm($conn, $group, $_SESSION["_user"], 0);
        }
        break;

    case "release_alarm":
        // Take alarm number
        if (!empty($group)) {
            AlarmGroup::take_alarm($conn, $group, $_SESSION["_user"], 1);
        }
        break;

    case "ungroup_alarm":
        if (!empty($alarm)) {
            $alarm_pairs = split(',', $alarm);
            foreach($alarm_pairs as $alarm_pair) {
                list($u_backlog_id, $u_event_id) = split('-', $alarm_pair);
                AlarmGroup::ungroup_alarm($conn, $u_backlog_id, $u_event_id);
            }
        }
        break;

    case "group_alarm":
        if (!empty($group)) {
            //Add alarm to a pre-exist group
            $alarm_pairs = split(',', $alarm);
            foreach($alarm_pairs as $alarm_pair) {
                list($u_backlog_id, $u_event_id) = split('-', $alarm_pair);
                AlarmGroup::group_alarm($conn, $group, $u_backlog_id, $u_event_id);
            }
        } else {
            //print "holaaa";
            //Ungroup older alarm and group other with this
            
        }
        break;

    case "delete_alarm":
        if (!empty($alarm)) {
            $alarm_pairs = split(',', $alarm);
            foreach($alarm_pairs as $alarm_pair) {
                list($u_backlog_id, $u_event_id) = split('-', $alarm_pair);
                if ($u_backlog_id == 0) {
                    Alarm::delete($conn, $u_event_id);
                } else {
                    Alarm::delete_backlog($conn, $u_backlog_id, $u_event_id);
                }
            }
        }
        break;

    case "delete_group":
        // First test owner is empty or actual user
        if (!empty($group)) {
            $group_ids = split(',', $group);
            foreach($group_ids as $group_id) {
                $alarm_group = AlarmGroup::get_group($conn, $group_id);
                $group_user = $alarm_group->get_owner();
                if ($group_user == '' || $group_user == false) {
                    // Take unassigned alarm
                    AlarmGroup::take_alarm($conn, $group_id, $_SESSION["_user"], 0);
                    $group_user = $_SESSION["_user"];
                }
                // Delete user alarm
                if ($group_user == $_SESSION["_user"]) {
                    // Delete Group
                    AlarmGroup::delete($conn, $group_id);
                    $alarms_of_group = AlarmGroup::get_list_of_group($conn, $group_id);
                    foreach($alarms_of_group as $alarm) {
                        // Delete alarms in group
                        Alarm::delete($conn, $alarm->get_event_id());
                    }
                }
            }
        }
        break;

    case "close_alarm":
        if (!empty($alarm)) {
            $alarm_pairs = split(',', $alarm);
            foreach($alarm_pairs as $alarm_pair) {
                list($u_backlog_id, $u_event_id) = split('-', $alarm_pair);
                // Test group owner is empty or actual user
                $group_id = AlarmGroup::get_group_from_alarm($conn, $u_backlog_id, $u_event_id);
                $alarm_group = AlarmGroup::get_group($conn, $group_id);
                $group_user = $alarm_group->get_owner();
                if ($group_user == '' || $group_user == false) {
                    // Take unassigned alarm
                    AlarmGroup::take_alarm($conn, $group_id, $_SESSION["_user"], 0);
                    $group_user = $_SESSION["_user"];
                }
                // Close user alarm
                if ($group_user == $_SESSION["_user"]) {
                    // Close Alarm
                    Alarm::close($conn, $u_event_id);
                }
            }
        }
        break;

    case "open_alarm":
        if (!empty($alarm)) {
            $alarm_pairs = split(',', $alarm);
            foreach($alarm_pairs as $alarm_pair) {
                list($u_backlog_id, $u_event_id) = split('-', $alarm_pair);
                // Test group owner is empty or actual user
                $group_id = AlarmGroup::get_group_from_alarm($conn, $u_backlog_id, $u_event_id);
                $alarm_group = AlarmGroup::get_group($conn, $group_id);
                $group_user = $alarm_group->get_owner();
                if ($group_user == '' || $group_user == false) {
                    // Take unassigned alarm
                    AlarmGroup::take_alarm($conn, $group_id, $_SESSION["_user"], 0);
                    $group_user = $_SESSION["_user"];
                }
                // Close user alarm
                if ($group_user == $_SESSION["_user"]) {
                    // Open Alarm
                    Alarm::open($conn, $u_event_id);
                }
            }
        }
        break;

    case "close_group":
        // First test owner is empty or actual user
        if (!empty($group)) {
            $group_ids = split(',', $group);
            foreach($group_ids as $group_id) {
                $alarm_group = AlarmGroup::get_group($conn, $group_id);
                $group_user = $alarm_group->get_owner();
                if ($group_user == '' || $group_user == false) {
                    // Take unassigned alarm
                    AlarmGroup::take_alarm($conn, $group_id, $_SESSION["_user"], 0);
                    $group_user = $_SESSION["_user"];
                }
                // Close user alarm
                if ($group_user == $_SESSION["_user"]) {
                    // Close Group
                    AlarmGroup::close($conn, $group_id);
                    $alarms_of_group = AlarmGroup::get_list_of_group($conn, $group_id);
                    foreach($alarms_of_group as $alarm) {
                        // Close alarms in group
                        Alarm::close($conn, $alarm->get_event_id());
                    }
                }
            }
        }
        break;

    case "open_group":
        // First test owner is empty or actual user
        if (!empty($group)) {
            $group_ids = split(',', $group);
            foreach($group_ids as $group_id) {
                $alarm_group = AlarmGroup::get_group($conn, $group_id);
                $group_user = $alarm_group->get_owner();
                if ($group_user == '' || $group_user == false) {
                    // Take unassigned alarm
                    AlarmGroup::take_alarm($conn, $group_id, $_SESSION["_user"], 0);
                    $group_user = $_SESSION["_user"];
                }
                // Open user alarm
                if ($group_user == $_SESSION["_user"]) {
                    // Close Group
                    AlarmGroup::open($conn, $group_id);
                    $alarms_of_group = AlarmGroup::get_list_of_group($conn, $group_id);
                    foreach($alarms_of_group as $alarm) {
                        // Close alarms in group
                        Alarm::open($conn, $alarm->get_event_id());
                    }
                }
            }
        }
        break;
    }
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title> Control Panel </title>
  <?php
    if ($autorefresh) {
        print '<meta http-equiv="refresh" content="' . $refresh_time . ';url=' . build_url("", "") . '"/>';
    }
?>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
<!--  <link rel="StyleSheet" href="dtree.css" type="text/css" />-->
  <link rel="stylesheet" href="../style/style.css"/>
  <link rel="stylesheet" href="../style/jquery-ui-1.7.custom.css"/>
  <link rel="stylesheet" type="text/css" href="../style/greybox.css"/>

  <script type="text/javascript" src="../js/jquery-1.3.1.js"></script>
  <script type="text/javascript" src="../js/jquery-ui-1.7.custom.min.js"></script>
  <script type="text/javascript" src="../js/greybox.js"></script>
  
  <?php echo $xajax->printJavascript('', XAJAX_JS); ?>

  <script language="javascript">

	function toggle_groups()
	{
		var obj = document.getElementsByName("group");

		for ( var i=0; i < obj.length; i++)
		{
			if(obj[i].disabled == false)
				obj[i].checked = !obj[i].checked;
		}

	}

	function toggle_alarms(alarm_list)
	{
		var alarm_pairs = alarm_list.split(",");
		var i, j;
		var obj = document.getElementsByName("alarm_checkbox");

		for (i=0; i < obj.length; i++)
		{
			for (j=0; j < alarm_pairs.length; j++)
			{
				if (obj[i].value == alarm_pairs[j])
				{	
					if(obj[i].disabled == false)
						obj[i].checked = !obj[i].checked;
					break;
				}
			}
		}
	}

	function toggle_event(backlog_id, event_id)
	{
		var index = 0;
		var event_table = new Array();
		var eventbox = new Array();

		var obj = document.getElementsByName("event_table" + backlog_id + "-" + event_id);

		for(var i = 0; i < obj.length; i++)
		{
			event_table[index] = obj[i];
			index++;
		}

		obj = document.getElementById("event_table" + backlog_id + "-" + event_id);	
		if (obj) {
			if(obj.name == "event_table" + backlog_id + "-" + event_id){
				//#!@$  IE !!
				event_table[0]=document.getElementById("event_table" + backlog_id + "-" + event_id);
			}
		}

		if (event_table.length==0)
		{
			xajax_getEvents(backlog_id, event_id);
		}
		else
		{
			index = 0;
			obj = "";

			obj = document.getElementsByName("eventbox" + backlog_id + "-" + event_id);

			for(i = 0; i < obj.length; i++)
			{
				eventbox[index] = obj[i];
				index++;
			}

			if(eventbox[0].style.display=='none'){
				eventbox[0].style.display='block';
			} else {
				eventbox[0].style.display='none';
			}
		}
	
	}

        function confirm_delete_group(url) 
	{
        	if (confirm('<?php echo _("Are you sure you want to delete this group and all its contents?") ?>')) {
            		window.location=url;
        	}
    	}

	function ungroup_alarms()
	{
		var index = 0;
		var selected_alarms = new Array();

		var obj = document.getElementsByName("alarm_checkbox");

		for(var i = 0; i < obj.length; i++)
		{
			if( obj[i].checked )
			{
				selected_alarms[index] = obj[i].value;
				index++;
			}
		}

		if (selected_alarms.length==0)
		{
			alert("Please, select the alarms to ungroup.");
		}
		else
		{
			location.href="<?php
    print build_url("ungroup_alarm", "") ?>" + "&alarm=" + selected_alarms;
		}
	}

	function group_alarms()
	{
		var index = 0;
		var selected_alarms = new Array();
		var selected_group = new Array();

		var obj = document.getElementsByName("alarm_checkbox");

		for(var i = 0; i < obj.length; i++)
		{
			if( obj[i].checked )
			{
				selected_alarms[index] = obj[i].value;
				index++;
			}
		}

		if (selected_alarms.length==0)
		{
			alert("Please, select the alarms to group.");
			return;
		}
/*
		var group = document.getElementsByName("group");	
		index = 0;
		for(var i = 0; i < group.length; i++)
		{
			if( group[i].checked )
			{
				selected_group[index] = group[i].value;
				index++;
			}
		}

		if (selected_group.length != 1)
		{
			alert("Please, select a group");
			return;
		}
*/
		GB_show("Group alarms ("+index+" selected)","alarm_group_group.php" +  "?alarm=" + selected_alarms,200,'40%');
		//location.href="<?php
    print build_url("group_alarm", "") ?>" + "&alarm=" + selected_alarms + "&group=" + selected_group[0];

	}

	function close_groups()
	{
		var selected_group = new Array();
		var group = document.getElementsByName("group");	
		var index = 0;

		for(var i = 0; i < group.length; i++)
		{
			if( group[i].checked )
			{
				selected_group[index] = group[i].value;
				index++;
			}
		}

		if (selected_group.length == 0)
		{
			alert("Please, select the groups to close");
			return;
		}

		location.href="<?php
    print build_url("close_group", "") ?>" +  "&group=" + selected_group;
	}
	
	function delete_groups()
	{
		var selected_group = new Array();
		var group = document.getElementsByName("group");	
		var index = 0;

		for(var i = 0; i < group.length; i++)
		{
			if( group[i].checked )
			{
				selected_group[index] = group[i].value;
				index++;
			}
		}

		if (selected_group.length == 0)
		{
			alert("Please, select the groups to delete");
			return;
		}
		GB_show("Delete Groups","alarm_group_delete.php" +  "?group=" + selected_group,150,'40%');
		//return false;
		//location.href="<?php
    print build_url("close_group", "") ?>" +  "&group=" + selected_group;
	}

        function confirm_delete(url) 
	{alert (url);
        	if (confirm('<?php echo _("Are you sure you want to delete this Alarm and all its events?") ?>')) {
            		window.location=url;
        	}
    	}

	function change_descr(objname)
	{
		var descr;
		descr = document.getElementsByName(objname); 
		descr = descr[0];	
		location.href="<?php
    print build_url("change_descr", "") ?>" + "&group=" + objname.replace("input","") + "&descr=" + descr.value;
	}

	function send_descr(obj ,e) 
	{
		var key;

		if (window.event)
		{
			key = window.event.keyCode;
		}
		else if (e)
		{
			key = e.which;
		}
		else
		{
			return;
		}
		if (key == 13) 
		{
//			location.href="<?php
    print $_SERVER["PHP_SELF"]; ?>"+"?action=change_descr&group=" + obj.name + "&descr=" + obj.value;
			change_descr(obj.name);

		}
	}
	
	function checkall () {
	$("input[type=checkbox]").each(function() {
		if (this.id.match(/^check_\d+/)) {
			this.checked = (this.checked) ? false : true;
		}
	});
  }
  function tooglebtn() {
	$('#searchtable').toggle();
	if ($("#timg").attr('src').match(/toggle_up/)) 
		$("#timg").attr('src','../pixmaps/sem/toggle.gif');
	else
		$("#timg").attr('src','../pixmaps/sem/toggle_up.gif');
  }
  </script>

<script type="text/javascript" src="../js/dtree.js"></script>

</head>

<body>
<?php
    if (GET('withoutmenu') != "1") include ("../hmenu.php");
    /* Filter & Action Console */
    print '<form name="filters" method="GET">';
    print '<table width="90%" align="center" class="noborder"><tr><td class="nobborder left">';
    print '<a href="javascript:;" onclick="tooglebtn()"><img src="../pixmaps/sem/toggle_up.gif" border="0" id="timg" title="Toggle"> <small><font color="black">'._("Filters, Actions and Options").'</font></small></a>';
    print '</td></tr></table>';
    print '<table width="90%" align="center" id="searchtable"><tr><th colspan="2" width="60%">';
    print _("Filter") . '</th><th>' . _("Actions") . '</th><th>' . _("Options") . '</th></tr>';
    // Date filter
    print '<tr><td width="10%" style="text-align: right; border-width: 0px">';
    print '<b>' . _('Date') . '</b>:
    </td>
    <td style="text-align: left; border-width: 0px">' . _('from') . ': <input type="text" size=10 name="date_from" id="date_from" value="' . $date_from . '">&nbsp;';
    //Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_from', true));
    print _('to') . ': <input type="text" size="10" name="date_to" id="date_to" value="' . $date_to . '">&nbsp;';
    //Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_to', true));
    //print '(' . _('YY-MM-DD') . ')' .
    '</td>';
    //Actions
    print '<td rowspan="3" style="text-align: left;border-bottom:0px solid white" nowrap>
<a href=javascript:ungroup_alarms() >Ungroup</a><br/>
<a href=javascript:group_alarms() >Group</a><br/>
<a href=javascript:close_groups() >Close Groups</a><br/><br>
<a href=javascript:delete_groups()><b>Delete Groups</b></a>
</td>';
    //Options
    $selected1 = $selected2 = $selected3 = $selected4 = "";
    if ($show_options == 1) $selected1 = 'selected="true"';
    if ($show_options == 2) $selected2 = 'selected="true"';
    if ($show_options == 3) $selected3 = 'selected="true"';
    if ($show_options == 4) $selected4 = 'selected="true"';
    if ($hide_closed) {
        $hide_check = 'checked="true"';
    } else {
        $hide_check = "";
    }
    $refresh_sel1 = $refresh_sel2 = $refresh_sel3 = $refresh_sel4 = "";
    if ($refresh_time == 30) $refresh_sel1 = 'selected="true"';
    if ($refresh_time == 60) $refresh_sel2 = 'selected="true"';
    if ($refresh_time == 180) $refresh_sel3 = 'selected="true"';
    if ($refresh_time == 600) $refresh_sel4 = 'selected="true"';
    if ($autorefresh) {
        $hide_autorefresh = 'checked="true"';
        $disable_autorefresh = '';
    } else {
        $hide_autorefresh = '';
        $disable_autorefresh = 'disabled="true"';
    }
    print '<td rowspan="3" style="text-align: left;border-bottom:0px solid white"><strong>Show:</strong>&nbsp;<select name="show_options">' . '<option value="1" ' . $selected1 . '>All Groups</option>' . '<option value="2" ' . $selected2 . '>My Groups</option>' . '<option value="3" ' . $selected3 . '>Groups Without Owner</option>' . '<option value="4" ' . $selected4 . '>My Groups & Without Owner</option>' . '</select> <br/>' . '<input type="checkbox" name="hide_closed" ' . $hide_check . ' />' . gettext("Hide closed alarms") . '<br/><input type="checkbox" name="autorefresh" onclick="javascript:document.filters.refresh_time.disabled=!document.filters.refresh_time.disabled;" ' . $hide_autorefresh . ' />' . gettext("Autorefresh") . '&nbsp;<select name="refresh_time" ' . $disable_autorefresh . ' >' . '<option value="30" ' . $refresh_sel1 . ' >30 sec</options>' . '<option value="60" ' . $refresh_sel2 . ' >1 min</options>' . '<option value="180" ' . $refresh_sel3 . ' >3 min</options>' . '<option value="600" ' . $refresh_sel4 . ' >10 min</options>' . '</select>' . '&nbsp;<a href="' . build_url("", "") . '" >[Refresh]</a>' . '</td> </tr>';
    // IP filter
    print '
<tr>
    <td width="10%" style="text-align: right; border-width: 0px">
        <b>' . _("IP Address") . ' </b>:
    </td>
    <td style="text-align: left; border-width: 0px" nowrap>' . _("source") . ': <input type="text" size="15" name="src_ip" value="' . $src_ip . '"> ' . _("destination") . ': <input type="text" size="15" name="dst_ip" value="' . $dst_ip . '">
    </td>
</tr>
';
    // Num alarm page filter
    print '
<tr>
    <td width="10%" style="text-align: right; border-width: 0px" nowrap>
        <b>' . _("Num. alarms per page") . '</b>:
    </td>
    <td style="text-align: left; border-width: 0px">
        <input type="text" size=3 name="num_alarms_page" value="' . $ROWS . '">
    </td>
</tr>
';
    print '<tr ><th colspan="4" style="padding:5px"><input type="submit" class="btn" value="' . _("Go") . '"></th></tr></table>';
    print '</form><br>';
    /* Alarm Group List */
    print '<table width="100%">' . '<tr><td class="nobborder" style="text-align:center">';
    function getEvents($backlog_id, $event_id) {
        global $conn, $conf;
        ossim_valid($backlog_id, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("backlog_id"));
        ossim_valid($event_id, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("event_id"));
        $summ_event_count = 0;
        $highest_rule_level = 0;
        $default_asset = 2;
        $host_list = Host::get_list($conn);
        foreach($host_list as $host) {
            $assets[$host->get_ip() ] = $host->get_asset();
        }
        $acid_link = $conf->get_conf("acid_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $eventTable_header = '<table id="event_table' . $backlog_id . '-' . $event_id . '" name="event_table' . $backlog_id . '-' . $event_id . '" width="100%" border="0"> <tr> <th>' . gettext("Id") . '</th> <th>' . gettext("Events") . '</th> <th>' . gettext("Risk") . '</th> <th>' . gettext("Date") . '</th> <th>' . gettext("Source") . '</th> <th>' . gettext("Destination") . '</th> <th width="7%">' . gettext("Correlation Level") . '</th> </tr>';
        $eventTable_body = '';
        if ($alarm_list = Alarm::get_events($conn, $backlog_id, 1, $event_id)) {
            $count_events = 0;
            $count_alarms = 0;
            foreach($alarm_list as $alarm) {
                $id = $alarm->get_plugin_id();
                $sid = $alarm->get_plugin_sid();
                $e_backlog_id = $alarm->get_backlog_id();
                $e_event_id = $alarm->get_event_id();
                $risk = $alarm->get_risk();
                $rcolor = "";
                if ($risk > 7) {
                    $rcolor = "style='background-color:red;color:white'";
                } elseif ($risk > 4) {
                    $rcolor = "style='background-color:orange;color:black'";
                } elseif ($risk > 2) {
                    $rcolor = "style='background-color:green;color:white'";
                }
                $snort_sid = $alarm->get_snort_sid();
                $snort_cid = $alarm->get_snort_cid();
                $orig_date = $alarm->get_timestamp();
                $date = Util::timestamp2date($orig_date);
                $src_ip = $alarm->get_src_ip();
                $dst_ip = $alarm->get_dst_ip();
                $src_port = $alarm->get_src_port();
                $dst_port = $alarm->get_dst_port();
                $asset_src = array_key_exists($src_ip, $assets) ? $assets[$src_ip] : $default_asset;
                $asset_dst = array_key_exists($dst_ip, $assets) ? $assets[$dst_ip] : $default_asset;
                /*
                
                $src_port = Port::port2service($conn, $src_port);
                $dst_port = Port::port2service($conn, $dst_port);
                
                */
                $src_link = "../report/index.php?host=$src_ip&section=events";
                $dst_link = "../report/index.php?host=$dst_ip&section=events";
                /*
                $src_name = Host::ip2hostname($conn, $src_ip);
                $dst_name = Host::ip2hostname($conn, $dst_ip);
                */
                $src_name = $src_ip;
                $dst_name = $dst_ip;
                $src_img = Host_os::get_os_pixmap($conn, $src_ip);
                $dst_img = Host_os::get_os_pixmap($conn, $dst_ip);
                $sid_name = "";
                if ($plugin_sid_list = Plugin_sid::get_list($conn, "WHERE plugin_id = $id AND sid = $sid")) {
                    $sid_name = $plugin_sid_list[0]->get_name();
                    $sid_priority = $plugin_sid_list[0]->get_priority();
                } else {
                    $sid_name = "Unknown (id=$id sid=$sid)";
                    $sid_priority = "N/A";
                }
                if ($alarm->get_alarm()) $name = Util::translate_alarm($conn, $name, $alarm);
                $name = "<b>$name</b>";
                $event_acid_link = "";
                if (($snort_sid > 0) and ($snort_cid)) {
                    $event_acid_link = str_replace("//", "/", "$acid_link/" . $acid_prefix . "_qry_alert.php?submit=%230-%28" . "$snort_sid-$snort_cid%29");
                } else {
                    $event_acid_link = "";
                }
                $acid_link_date = Util::get_acid_date_link($date, $src_ip, "ip_src");
                $name = ereg_replace("directive_event: ", "", $sid_name);
                $balloon_name = "<div class='balloon'>" . $name . "<span class='tooltip'><span class='top'></span><span class='middle ne11'>Src Asset: <b>" . $asset_src . "</b><br>Dst Asset: <b>" . $asset_dst . "</b><br>Priority: <b>" . $sid_priority . "</b></span><span class='bottom'></span></span></div>";
                $eventTable_body = $eventTable_body . '<tr><td>' . $e_event_id . '</td><td style="text-align:left;padding-left:15px"><a href="' . $event_acid_link . '" >' . $balloon_name . '</a></td><td ' . $rcolor . '>' . $risk . '</td><td>' . '<a href="' . $acid_link_date . '" >' . $date . '</a></td><td>' . '<a href="' . $src_link . '" title="' . $src_ip . '" >' . $src_name . '</a>:' . $src_port . '</td><td>' . '<a href="' . $dst_link . '" title="' . $dst_ip . '" >' . $dst_name . '</a>:' . $dst_port . '</td><td width="7%">' . $alarm->get_rule_level() . '</td></tr>';
            }
        }
        $eventTable_tail = '</table>';
        $eventTable = $eventTable_header . $eventTable_body . $eventTable_tail;
        $objResponse = new xajaxResponse();
        $objResponse->addAssign("eventbox" . $backlog_id . "-" . $event_id, "innerHTML", $eventTable);
        return $objResponse;
    }
    function resize_text($text, $size) {
        if (strlen($text) < $size) {
            return $text;
        } else {
            return substr($text, 0, $size - 3) . "...";
        }
    }
    function write_openandclose($tree_count) {
        print "var expanded=0;";
        print "function opencloseAll(){ \n";
        for ($i = 1; $i <= $tree_count; $i++) {
            print "if (expanded) a" . $i . ".closeAll(); else a" . $i . ".openAll();\n";
        }
        print "if(expanded) { expanded=0; document.getElementById('expandcollapse').src='../pixmaps/plus.png'; } else { expanded=1; document.getElementById('expandcollapse').src='../pixmaps/minus.png'; }\n";
        print "}\n";
        print "function openAll(){ \n";
        for ($i = 1; $i <= $tree_count; $i++) {
            print "a" . $i . ".openAll();\n";
        }
        print "}\n";
        print "function closeAll(){\n";
        for ($i = 1; $i <= $tree_count; $i++) {
            print "a" . $i . ".closeAll();\n";
        }
        print "}\n";
    }
    function build_url($action, $extra) {
        global $date_from, $date_to, $show_options, $src_ip, $dst_ip, $num_alarms_page, $hide_closed, $autorefresh, $refresh_time, $inf, $sup;
        if (empty($action)) {
            $action = "none";
        }
        $options = "";
        if (!empty($date_from)) {
            $options = $options . "&date_from=" . $date_from;
        }
        if (!empty($date_to)) $options = $options . "&date_to=" . $date_to;
        if (!empty($show_options)) $options = $options . "&show_options=" . $show_options;
        if (!empty($autorefresh)) $options = $options . "&autorefresh=on";
        if (!empty($refresh_time)) $options = $options . "&refresh_time=" . $refresh_time;
        if (!empty($src_ip)) $options = $options . "&src_ip=" . $src_ip;
        if (!empty($dst_ip)) $options = $options . "&dsp_ip=" . $dsp_ip;
        if (!empty($num_alarms_page)) $options = $options . "&num_alarms_page=" . $num_alarms_page;
        if (!empty($hide_closed)) $options = $options . "&hide_closed=on";
        if ($action != "change_page") {
            if (!empty($inf)) $options = $options . "&inf=" . $inf;
            if (!empty($sup)) $options = $options . "&sup=" . $sup;
        }
        $url = $_SERVER["PHP_SELF"] . "?action=" . $action . $extra . $options;
        return $url;
    }
    function write_trees($tree_count) {
        for ($i = 1; $i <= $tree_count; $i++) {
            print "document.write(a" . $i . ");\n";
            print "document.write('<br>');\n";
        }
    }
    function create_tree($tree_id, $tree_title) {
        print "\n" . '<!-- Tree n' . $tree_id . ' start... -->' . "\n";
        print '	a' . $tree_id . ' = new dTree("a' . $tree_id . '");
	a' . $tree_id . '.config.useIcons=false;
	a' . $tree_id . '.config.useLines=false;
	a' . $tree_id . '.config.useCookies=true;
	a' . $tree_id . '.config.closeSameLevel=false;
	a' . $tree_id . '.config.useSelection=false;
	a' . $tree_id . '.add(0,-1,"<table class=noborder width=\'100%\'><tr><td class=nobborder style=\'text-align:center;padding:5px;\'><b>' . $tree_title . '</b>&nbsp;|&nbsp;<a href=\'javascript: a' . $tree_id . '.openAll();\'>Expand</a>&nbsp;|&nbsp;<a href=\'javascript: a' . $tree_id . '.closeAll();\'>Collapse</a></td></tr></table>","javascript: void(0);");' . "\n";
    }
    function create_tree_child($father_id, $group_id, $ocurrences, $alarm_name, $owner, $descr, $status, $since, $date, $src_ip, $dst_ip) {
        if ($descr == false) {
            //		$descr = "No hay descripcion";
            $descr = "";
        }
        $sort_descr = resize_text($descr, 20);
        $group_box = "";
        $owner_take = 0;
        $av_description = "";
        $incident_link = "<img border=0 src='../pixmaps/script--pencil-gray.png'/>";
        if ($owner == false) {
            $owner = "<a href='" . build_url("take_alarm", "&group=" . $group_id) . "'>Take</a>";
            $background = '#DFDFDF;';
            //		$av_description = "readonly='true'";
            //		$description = "<input type='text' name='input" . $group_id . "' title='" . $descr . "' " . $av_description . " style='text-decoration: none; border: 0px; background: " . $background . "' size='20' value='" . $descr . "' />";
            $group_box = "<input type='checkbox' id='check_" . $group_id . "' name='group' value='" . $group_id . "' >";
        } else {
            //Si el usuario actual es el propietario de la alarma
            if ($owner == $_SESSION["_user"]) {
                $owner_take = 1;
                $background = '#B5C7DF;';
                if ($status == 'open') {
                    $owner = "<a href='" . build_url("release_alarm", "&group=" . $group_id) . "'>Release</a>";
                }
                $group_box = "<input type='checkbox' id='check_" . $group_id . "' name='group' value='" . $group_id . "' >";
                $incident_link = '<a class=greybox2 title=\'New ticket for Group ID' . $group_id . '\' href=\'../incidents/newincident.php?' . "ref=Alarm&" . "title=" . urlencode($alarm_name) . "&" . "priority=$s_risk&" . "src_ips=$src_ip&" . "event_start=$since&" . "event_end=$date&" . "src_ports=$src_port&" . "dst_ips=$dst_ip&" . "dst_ports=$dst_port" . '\'>' . '<img border=0 src=\'../pixmaps/script--pencil.png\' alt=\'ticket\' border=\'0\'/>' . '</a>';
            } else {
                $background = '#FEE599;';
                $av_description = "readonly='true'";
                $description = "<input type='text' name='input" . $group_id . "' title='" . $descr . "' " . $av_description . " style='text-decoration: none; border: 0px; background: #FEE599' size='20' value='" . $descr . "' />";
                $group_box = "<input type='checkbox' disabled = 'true' name='group' value='" . $group_id . "' >";
            }
        }
        if ($description == "") {
            $description = "<table class='noborder' style='background:$background'><tr><td class='nobborder'><input type='text' name='input" . $group_id . "' title='" . $descr . "' " . $av_description . " style='text-decoration: none; border: 0px; background: #FFFFFF' size='20' value='" . $descr . "' onkeypress='send_descr(this, event);' /></td><td class='nobborder'><a href=javascript:change_descr('input" . $group_id . "') ><img valign='middle' border=0 src='../pixmaps/disk-black.png' /></a></td></tr></table>";
        }
        /* Incident link */
        /*
        $delete_link = "<a title='" . gettext("Delete") . "' href=javascript:confirm_delete_group('".$_SERVER['PHP_SELF']."?delete_group=". $group_id . "');" .
        ">" . "<img border=0 src='../pixmaps/cross-circle-frame.png'/>" . "</a>";
        */
        $delete_link = ($status == "open" && $owner_take) ? "<a title='" . gettext("Close") . "' href='" . build_url("close_group", "&group=" . $group_id) . "'" . ">" . "<img border=0 src='../pixmaps/cross-circle-frame.png'/>" . "</a>" : "<img border=0 src='../pixmaps/cross-circle-frame-gray.png'/>";
        if ($status == 'open') {
            if ($owner_take) $close_link = "<a href='" . build_url("close_group", "&group=" . $group_id) . "'><img src='../pixmaps/lock-unlock.png' alt='Open, click to close group' title='Open, click to close group' border=0></a>";
            //$close_link = "<a title='" . gettext("Close") . "' href='" . build_url("close_group", "&group=" . $group_id ) . "'" . ">" . gettext("Open") . "</a>"; //"<img src='../pixmaps/close.gif'/>" . "</a>";
            else $close_link = "<img src='../pixmaps/lock-unlock.png' alt='Open, take this group then click to close' title='Open, take this group then click to close' border=0>";
        } else {
            if ($owner_take) $close_link = "<a href='" . build_url("open_group", "&group=" . $group_id) . "'><img src='../pixmaps/lock.png' alt='Closed, click to open group' title='Closed, click to open group' border=0></a>";
            else $close_link = "<img src='../pixmaps/lock.png' alt='Closed, take this group then click to open' title='Closed, take this group then click to open' border=0>";
            $group_box = "<input type='checkbox' disabled = 'true' name='group' value='" . $group_id . "' >";
        }
        if ($ocurrences > 1) {
            $ocurrence_text = strtolower(gettext("Alarms"));
        } else {
            $ocurrence_text = strtolower(gettext("Alarm"));
        }
        print '<!-- Child n' . $group_id . ' start... -->' . "\n";
        print '	a' . $father_id . '.add(' . $group_id . ', 0,"' . "<table width='100%' class='noborder'><td style='text-align: center; border-width: 0px' width='3%'>" . $group_box . "</td> <td style='text-align: center; border-width: 0px' width='3%'><a href='javascript: a" . $father_id . ".toggleTo(" . $group_id . ");'><strong><img src='../pixmaps/plus-small.png' border=0></strong></a>" . "</td> <th style='text-align: left; border-width: 0px; background: " . $background . "'><span style='font-size:x-small;'>G" . $group_id . "</span> - " . $alarm_name . "&nbsp;&nbsp;" . "<span style='font-size:xx-small; text-color: #AAAAAA;'>(" . $ocurrences . " " . $ocurrence_text . ")</span>" . "</th><th width='10%' style='text-align: center; border-width: 0px; background: " . $background . "'>" . $owner . "</th><th width='20%' style='text-align: center; border-width: 0px; background: " . $background . "'>" . $description . "</th>" . "<th style='text-align: center; border-width: 0px; background: " . $background . "' width='7%'>" . $close_link . "</th>" . "<td width='7%' style='text-decoration: none;'>" . $delete_link . " " . $incident_link . "</td>" . '</table>","javascript:void(0);");' . "\n";
    }
    function create_alarm_table($conn, $father_id, $group_id, $child_alarms, $owner) {
        $gi = geoip_open("/usr/share/geoip/GeoIP.dat", GEOIP_STANDARD);
        print ' a' . $father_id . '.add(' . $father_id . ', ' . $group_id . ',"' . "  ";
        // Get backlog and event_id for toggle_group
        $group_list = "";
        $default_asset = 2;
        $host_list = Host::get_list($conn);
        foreach($host_list as $host) {
            $assets[$host->get_ip() ] = $host->get_asset();
        }
        foreach($child_alarms as $s_alarm) {
            if (!empty($group_list)) {
                $group_list = $group_list . ",";
            }
            $s_backlog_id = $s_alarm->get_backlog_id();
            $s_event_id = $s_alarm->get_event_id();
            if (empty($s_backlog_id)) {
                $s_backlog_id = "0";
            }
            if (empty($s_event_id)) {
                $s_event_id = "0";
            }
            $group_list = $group_list . $s_backlog_id . "-" . $s_event_id;
        }
        print "<table width='96%' style='margin-left:3.5%'><th></th><th align='right' ><a href=javascript:toggle_alarms('" . $group_list . "') />#</a></th><th align='center' >" . gettext("Alarm") . "</th><th align='center' >" . gettext("Risk") . "</th><th align='center' >" . gettext("Since") . "</th><th align='center' >" . gettext("Last") . "</th><th align='center' >" . gettext("Source") . "</th><th align='center' >" . gettext("Destination") . "</th><th align='center' >" . gettext("Status") . "</th><th align='center' width='7%'>" . gettext("Action") . "</th>";
        $child_number = 0;
        foreach($child_alarms as $s_alarm) {
            $childnumber++;
            $s_id = $s_alarm->get_plugin_id();
            $s_sid = $s_alarm->get_plugin_sid();
            $s_backlog_id = $s_alarm->get_backlog_id();
            $s_event_id = $s_alarm->get_event_id();
            $s_src_ip = $s_alarm->get_src_ip();
            $s_src_port = $s_alarm->get_src_port();
            $s_dst_port = $s_alarm->get_dst_port();
            $s_dst_ip = $s_alarm->get_dst_ip();
            $s_status = $s_alarm->get_status();
            $s_asset_src = array_key_exists($s_src_ip, $assets) ? $assets[$s_src_ip] : $default_asset;
            $s_asset_dst = array_key_exists($s_dst_ip, $assets) ? $assets[$s_dst_ip] : $default_asset;
            /*
            $s_src_port = Port::port2service($conn, $s_src_port);
            $s_dst_port = Port::port2service($conn, $s_dst_port);
            
            */
            $s_src_link = "../report/index.php?host=$s_src_ip&section=events";
            $src_title = "Src Asset: <b>$s_asset_src</b><br>IP: <b>$s_src_ip</b>";
            $s_dst_link = "../report/index.php?host=$s_dst_ip&section=events";
            $dst_title = "Dst Asset: <b>$s_asset_dst</b><br>IP: <b>$s_dst_ip</b>";
            $s_src_name = Host::ip2hostname($conn, $s_src_ip);
            $s_dst_name = Host::ip2hostname($conn, $s_dst_ip);
            // $s_src_name = $s_src_ip;
            // $s_dst_name = $s_dst_ip;
            $s_src_img = str_replace("\"", "'", Host_os::get_os_pixmap($conn, $s_src_ip));
            $s_dst_img = str_replace("\"", "'", Host_os::get_os_pixmap($conn, $s_dst_ip));
            $src_country = strtolower(geoip_country_code_by_addr($gi, $s_src_ip));
            $src_country_img = ($src_country) ? "<img src='/ossim/pixmaps/flags/" . $src_country . ".png'>" : "";
            $dst_country = strtolower(geoip_country_code_by_addr($gi, $s_dst_ip));
            $dst_country_img = ($dst_country) ? "<img src='/ossim/pixmaps/flags/" . $dst_country . ".png'>" : "";
            $source_link = "<a href='" . $s_src_link . "' title='" . $s_src_ip . "' >" . $s_src_name . "</a>:" . $s_src_port . " $s_src_img $src_country_img";
            $source_balloon = "<div class='balloon'>" . $source_link . "<span class='tooltip'><span class='top'></span><span class='middle ne11'>$src_title</span><span class='bottom'></span></span></div>";
            $dest_link = "<a href='" . $s_dst_link . "' title='" . $s_dst_ip . "' >" . $s_dst_name . "</a>:" . $s_dst_port . " $s_dst_img $dst_country_img";
            $dest_balloon = "<div class='balloon'>" . $dest_link . "<span class='tooltip'><span class='top'></span><span class='middle ne11'>$dst_title</span><span class='bottom'></span></span></div>";
            //		    $selection_array[$group_id][$child_number] = $s_backlog_id . "-" . $s_event_id;
            $s_sid_name = "";
            if ($s_plugin_sid_list = Plugin_sid::get_list($conn, "WHERE plugin_id = $s_id AND sid = $s_sid")) {
                $s_sid_name = $s_plugin_sid_list[0]->get_name();
                $s_sid_priority = $s_plugin_sid_list[0]->get_priority();
            } else {
                $s_sid_name = "Unknown (id=$s_id sid=$s_sid)";
                $s_sid_priority = "N/A";
            }
            $s_date = Util::timestamp2date($s_alarm->get_timestamp());
            if ($s_backlog_id != 0) {
                $s_since = Util::timestamp2date($s_alarm->get_since());
            } else {
                $s_since = $s_date;
            }
            $s_risk = $s_alarm->get_risk();
            //$s_alarm_link = Util::get_acid_pair_link($s_date, $s_alarm->get_src_ip(), $s_alarm->get_dst_ip());
            //		    $s_alarm_link = "events.php?backlog_id=$s_backlog_id";
            //$s_alarm_link = "javascript:xajax_getEvents(" . $s_backlog_id . "," . $s_event_id .");";
            $s_alarm_link = "javascript:toggle_event(" . $s_backlog_id . "," . $s_event_id . ");";
            /* Alarm name */
            $s_alarm_name = ereg_replace("directive_event: ", "", $s_sid_name);
            $s_alarm_name = Util::translate_alarm($conn, $s_alarm_name, $s_alarm);
            $balloon_name = "<div class='balloon'>" . $s_alarm_name . "<span class='tooltip'><span class='top'></span><span class='middle ne11'>Src Asset: <b>" . $s_asset_src . "</b><br>Dst Asset: <b>" . $s_asset_dst . "</b><br>Priority: <b>" . $s_sid_priority . "</b></span><span class='bottom'></span></span></div>";
            /* Risk field */
            if ($s_risk > 7) {
                $color = "red; color:white";
            } elseif ($s_risk > 4) {
                $color = "orange; color:black";
            } elseif ($s_risk > 2) {
                $color = "green; color:white";
            }
            if ($color) {
                $risk_field = "<td style='text-align: center; border-width: 1px; background-color: " . $color . ";'>" . $s_risk . "</td>";
            } else {
                $risk_field = "<td style='text-align: center; border-width: 1px;' >" . $s_risk . "</td>";
            }
            /* Delete link */
            /*
            if ($s_backlog_id == 0) {
            $s_delete_link = '<a title=\'' . gettext("Delete") . '\' href=\'javascript:confirm_delete(\"' . $_SERVER["PHP_SELF"] .
            "?delete=$s_event_id" .
            "&sup=" . "$sup" .
            "&inf=" . ($sup-$ROWS) .
            "&hide_closed=$hide_closed" . "\\\");'>" .
            "<img border=0 src='../pixmaps/cross-circle-frame.png' style='visibility: visible;'/>" . "</a>";
            } else {
            $s_delete_link = '<a title=\'' . gettext("Delete") . '\' href=\'javascript:confirm_delete(\"' . $_SERVER["PHP_SELF"] .
            "?delete_backlog=" . "$s_backlog_id-$s_event_id" .
            "&sup=" . "$sup" .
            "&inf=" . ($sup-$ROWS) .
            "&hide_closed=$hide_closed" . "\\\");'>" .
            "<img border=0 src='../pixmaps/cross-circle-frame.png' style='visibility: visible;' />" . "</a>";
            }
            }*/
            $s_delete_link = ($s_status == 'open') ? "<a href='" . build_url("close_alarm", "&alarm=" . $s_backlog_id . "-" . $s_event_id) . "' title='" . gettext("Click here to close alarm") . "'><img border=0 src='../pixmaps/cross-circle-frame.png' style='visibility: visible;'></a>" : "<img border=0 src='../pixmaps/cross-circle-frame-gray.png'>";
            /* Checkbox */
            if ($owner == $_SESSION["_user"] || $owner == "") {
                $checkbox = "<input type='checkbox' name='alarm_checkbox' value='" . $s_backlog_id . "-" . $s_event_id . "'>";
            } else {
                $checkbox = "<input type='checkbox' name='alarm_checkbox' disabled='true' value='" . $s_backlog_id . "-" . $s_event_id . "'>";
            }
            if ($s_status == 'open') {
                $status_link = "<a href='" . build_url("close_alarm", "&alarm=" . $s_backlog_id . "-" . $s_event_id) . "' style='color:" . (($s_status == "open") ? "#923E3A" : "#4C7F41") . "'>" . gettext("Open") . "</a>";
                //$status_link = "<a href='" . build_url("close_alarm", "&alarm=" . $s_backlog_id . "-" . $s_event_id) . "' title='" . gettext("Click here to close alarm") ."'>" . gettext("Open") . "</a>";
                
            } else {
                $status_link = "<a href='" . build_url("open_alarm", "&alarm=" . $s_backlog_id . "-" . $s_event_id) . "' style='color:" . (($s_status == "open") ? "#923E3A" : "#4C7F41") . "'>" . gettext("Closed") . "</a>";
                $checkbox = "<input type='checkbox' name='alarm_checkbox' disabled='true' value='" . $s_backlog_id . "-" . $s_event_id . "'>";
            }
            $summary = Alarm::get_alarm_stats($conn, $s_backlog_id, $s_event_id);
            $event_ocurrences = $summary["total_count"];
            if ($event_ocurrences != 1) {
                $ocurrences_text = strtolower(gettext("Events"));
            } else {
                $ocurrences_text = strtolower(gettext("Event"));
            }
            /* Expand button */
            if ($event_ocurrences > 0) $expand_button = "<a href='" . $s_alarm_link . "' ><strong><img src='../pixmaps/plus-small.png' border=0></strong></a>";
            else $expand_button = "<strong>[-]</strong>";
            print "<tr><td style='text-align: center; border-width: 1px' width='3%'>" . $expand_button . "</td><td style='text-align: center; border-width: 1px;'>" . $checkbox . "</td><td style='text-align: left; padding-left:10px; border-width: 1px' width='30%'><strong>" . $balloon_name . "</strong>" . "&nbsp;&nbsp;<span style='font-size: x-small; text-color: #AAAAAA;'>(" . $event_ocurrences . " " . $ocurrences_text . ")</span>" . "</td>" . $risk_field . "<td style='text-align: center; border-width: 1px' width='12%'>" . $s_since . "</td><td style='text-align: center; border-width: 1px' width='12%'>" . $s_date . "</td><td nowrap style='text-align: center; border-width: 1px; background-color: #eeeeee;'>" . $source_balloon . "</td><td nowrap style='text-align: center; border-width: 1px; background-color: #eeeeee;'>" . $dest_balloon . "</td><td bgcolor='" . (($s_status == "open") ? "#ECE1DC" : "#DEEBDB") . "' style='text-align: center; border-width: 1px;border:1px solid" . (($s_status == "open") ? "#E6D8D2" : "#D6E6D2") . "'><b>" . $status_link . "</b></td><td style='text-align: center; border-width: 1px;'>" . $s_delete_link . "</td>" . "</tr><tr><td></td><td colspan='9'><div name='eventbox" . $s_backlog_id . "-" . $s_event_id . "' id='eventbox" . $s_backlog_id . "-" . $s_event_id . "' style='display: block;'></div></td></tr>";
        }
        print "</table>";
        print "    " . '","javascript:void(0);");' . "\n";
    }
    //$disp=1;
    list($alarm_group, $count) = AlarmGroup::get_list($conn, $src_ip, $dst_ip, $hide_closed, "ORDER BY $order", $inf, $sup, $date_from, $date_to, $disp, $show_options);
    //$count = count($alarm_group);
    $tree_count = 0;
    print '</td></tr>';
    print '<form method="get" enctype="text/plain" name="alarm_form">';
    // Pagination
    print "<tr><td class='nobborder' style='text-align:center'>\n";
    print '<input type="button" value="Show Ungrouped" onclick="document.location.href=\'alarm_console.php\'" class="btn">&nbsp;';
    /* No mola */
    // OPTIMIZADO con SQL_CALC_FOUND_ROWS (5 junio 2009 Granada)
    //$alarm_group = AlarmGroup::get_list($conn, $src_ip, $dst_ip, $hide_closed, "ORDER BY $order", null, null, $date_from, $date_to, $disp, $show_options);
    //$count = count($alarm_group);
    $first_link = build_url("change_page", "&inf=0" . "&sup=" . $ROWS);
    $last_link = build_url("change_page", "&inf=" . ($count - $ROWS) . "&sup=" . $count);
    $inf_link = build_url("change_page", "&inf=" . ($inf - $ROWS) . "&sup=" . ($sup - $ROWS));
    $sup_link = build_url("change_page", "&inf=" . ($inf + $ROWS) . "&sup=" . ($sup + $ROWS));
    if ($inf >= $ROWS) {
        echo "<a href=\"" . $first_link . "\" >&lt;First&nbsp;</a>";
        echo "<a href=\"$inf_link\">&lt;-";
        printf(gettext("Prev %d") , $ROWS);
        echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $inf, $sup, $count);
        echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">";
        printf(gettext("Next %d") , $ROWS);
        echo " -&gt;</a>";
        echo "<a href=\"" . $last_link . "\" >&nbsp;Last&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $inf, $count, $count);
        echo ")&nbsp;&nbsp;";
    }
    print "</td></tr>";
    /* AlarmGroup Header */
    print "</tr><td></td><tr><td>";
    print "<table cellpadding=0 cellspacing=1 width='100%' class='noborder'><td width='3%' class='nobborder' style='text-align:center'><input type='checkbox' name='allcheck' onclick='checkall()'></td><td class='nobborder' style='text-align: center; padding:0px' width='3%'>" . "<a href='javascript: opencloseAll();'><img src='../pixmaps/plus.png' id='expandcollapse' border=0 alt='Expand/Collapse ALL' title='Expand/Collapse ALL'></a>" . "</td><td style='text-align: left;padding-left:10px; background-color:#9DD131;font-weight:bold'>" . gettext("Group") . "</td><td width='10%' style='text-align: center; background-color:#9DD131;font-weight:bold'>" . gettext("Owner") . "</td><td width='20%' style='text-align: center; background-color:#9DD131;font-weight:bold'>" . gettext("Description") . "</td>" . "<td style='text-align: center; background-color:#9DD131;font-weight:bold' width='7%'>" . gettext("Status") . "</td>" . "<td width='7%' style='text-decoration: none; background-color:#9DD131;font-weight:bold'>" . gettext("Action") . "</td>" . '</table>';
    print ' <script type="text/javascript">' . "\n";
    foreach($alarm_group as $alarm) {
        $g_group_id = $alarm->get_group_id();
        $g_ocurrences = $alarm->get_ocurrences();
        $g_id = $alarm->get_plugin_id();
        $g_sid = $alarm->get_plugin_sid();
        $g_backlog_id = $alarm->get_backlog_id();
        $g_src_ip = $alarm->get_src_ip();
        $g_dst_ip = $alarm->get_dst_ip();
        $g_owner = $alarm->get_owner();
        $g_descr = $alarm->get_descr();
        $g_status = $alarm->get_status();
        $g_sid_name = "";
        if ($g_plugin_sid_list = Plugin_sid::get_list($conn, "WHERE plugin_id = $g_id AND sid = $g_sid")) {
            $g_sid_name = $g_plugin_sid_list[0]->get_name();
        } else {
            $g_sid_name = "Unknown (id=$g_id sid=$g_sid)";
        }
        $g_alarm_name = ereg_replace("directive_event: ", "", $g_sid_name);
        $g_alarm_name = Util::translate_alarm($conn, $g_alarm_name, $alarm);
        $g_date = Util::timestamp2date($alarm->get_timestamp());
        if ($g_backlog_id != 0) {
            $g_since = Util::timestamp2date($alarm->get_since());
        } else {
            $g_since = $g_date;
        }
        /* show alarms by days */
        $g_date_slices = split(" ", $g_date);
        list($year, $month, $day) = split("-", $g_date_slices[0]);
        $g_date_formatted = strftime("%A %d-%b-%Y", mktime(0, 0, 0, $month, $day, $year));
        if ($g_datemark != $g_date_slices[0]) {
            $g_link_delete = "
                    <a href=\"" . $_SERVER["SCRIPT_NAME"] . "?delete_day=" . $alarm->get_timestamp() . "&inf=" . ($sup - $ROWS) . "&sup=$sup&hide_closed=$hide_closed\"> " . gettext("Delete") . " </a>
                ";
            $tree_count++;
            create_tree($tree_count, $g_date_formatted);
        }
        $g_datemark = $g_date_slices[0];
        // Add principal node (alarm_group)
        create_tree_child($tree_count, $g_group_id, $g_ocurrences, $g_alarm_name, $g_owner, $g_descr, $g_status, $g_since, $g_date, $g_src_ip, $g_dst_ip);
        // Get list of alarms by group
        $g_child_alarms = AlarmGroup::get_list_of_group($conn, $g_group_id);
        // Create alarm list
        create_alarm_table($conn, $tree_count, $g_group_id, $g_child_alarms, $g_owner);
    }
    write_trees($tree_count);
    write_openandclose($tree_count);
    print "</script>\n";
    print '</form>';
    print "</td></tr>";
    print "<tr><td class='nobborder' style='text-align:center'>\n";
    /* No mola */
    // OPTIMIZADO con SQL_CALC_FOUND_ROWS (5 junio 2009 Granada)
    //$alarm_group = AlarmGroup::get_list($conn, $src_ip, $dst_ip, $hide_closed, "ORDER BY $order", null, null, $date_from, $date_to, $disp, $show_options);
    //$count = count($alarm_group);
    $first_link = build_url("change_page", "&inf=0" . "&sup=" . $ROWS);
    $last_link = build_url("change_page", "&inf=" . ($count - $ROWS) . "&sup=" . $count);
    $inf_link = build_url("change_page", "&inf=" . ($inf - $ROWS) . "&sup=" . ($sup - $ROWS));
    $sup_link = build_url("change_page", "&inf=" . ($inf + $ROWS) . "&sup=" . ($sup + $ROWS));
    if ($inf >= $ROWS) {
        echo "<a href=\"" . $first_link . "\" >&lt;First&nbsp;</a>";
        echo "<a href=\"$inf_link\">&lt;-";
        printf(gettext("Prev %d") , $ROWS);
        echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $inf, $sup, $count);
        echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">";
        printf(gettext("Next %d") , $ROWS);
        echo " -&gt;</a>";
        echo "<a href=\"" . $last_link . "\" >&nbsp;Last&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $inf, $count, $count);
        echo ")&nbsp;&nbsp;";
    }
    print "</td></tr></table>\n";
?>

<?php
    $db->close($conn); ?>
<script>
// DatePicker
$(document).ready(function(){
	GB_TYPE = 'w';
	$("a.greybox2").click(function(){
		var t = this.title || $(this).text() || this.href;
		GB_show(t,this.href,450,'90%');
		return false;
	});
	$("a.greybox").click(function(){
		var t = this.title || $(this).text() || this.href;
		GB_show(t,this.href,150,'40%');
		return false;
	});
	
	$('#date_from').datepicker();
	$('#date_to').datepicker();
});
</script>
</body>
</html>


