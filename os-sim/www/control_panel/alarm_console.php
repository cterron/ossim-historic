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
Session::logcheck("MenuControlPanel", "ControlPanelAlarms");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title> Control Panel </title>
  <?php
if (GET('norefresh') == "") { ?><meta http-equiv="refresh" content="150"><?php
} ?>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
	<link rel="stylesheet" href="../style/jquery-ui-1.7.custom.css"/>
	<link rel="stylesheet" type="text/css" href="../style/greybox.css"/>
	
  <script type="text/javascript" src="../js/jquery-1.3.1.js"></script>
  <script type="text/javascript" src="../js/jquery-ui-1.7.custom.min.js"></script>
  <script type="text/javascript" src="../js/greybox.js"></script>
  <script language="javascript">
    function confirm_delete(url) {
        if (confirm('<?php echo _("Are you sure you want to delete this Alarm and all its events?") ?>')) {
            window.location=url;
        }
    }
  
  function show_alarm (id,tr_id) {
	tr = "tr"+tr_id;
	document.getElementById(tr).innerHTML = "<img src='../images/load.gif' alt='Loading'>";
	//alert (id);
	$.ajax({
		type: "GET",
		url: "events_ajax.php?backlog_id="+id,
		data: "",
		success: function(msg){
			//alert (msg);
			document.getElementById(tr).innerHTML = msg;
			plus = "plus"+tr_id;
			document.getElementById(plus).innerHTML = "<a href='' onclick=\"hide_alarm('"+id+"','"+tr_id+"');return false\"><img align='absmiddle' src='../pixmaps/minus-small.png' border='0'></a>"+tr_id;

			// GrayBox
			$(document).ready(function(){
				GB_TYPE = 'w';
				$("a.greybox").click(function(){
					var t = this.title || $(this).text() || this.href;
					GB_show(t,this.href,450,'90%');
					return false;
				});
			});

		}
		});
  }
  function hide_alarm (id,tr_id) {
	tr = "tr"+tr_id;
	document.getElementById(tr).innerHTML = "";
	plus = "plus"+tr_id;
	document.getElementById(plus).innerHTML = "<a href='' onclick=\"show_alarm('"+id+"','"+tr_id+"');return false\"><img align='absmiddle' src='../pixmaps/plus-small.png' border='0'></a>"+tr_id;
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

</head>

<body>

<?php
if (GET('withoutmenu') != "1") include ("../hmenu.php");
require_once ('ossim_db.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');
require_once ('classes/Port.inc');
require_once ('classes/Util.inc');
require_once ('classes/Security.inc');
include ("geoip.inc");
$gi = geoip_open("/usr/share/geoip/GeoIP.dat", GEOIP_STANDARD);
/* default number of events per page */
$ROWS = 50;
/* connect to db */
$db = new ossim_db();
$conn = $db->connect();
$delete = GET('delete');
$close = GET('close');
$open = GET('open');
$delete_day = GET('delete_day');
$order = GET('order');
$src_ip = GET('src_ip');
$dst_ip = GET('dst_ip');
$backup_inf = $inf = GET('inf');
$sup = GET('sup');
$hide_closed = GET('hide_closed');
$norefresh = GET('norefresh');
$params_string = "order=$order&src_ip=$src_ip&dst_ip=$dst_ip&inf=$inf&sup=$sup&hide_closed=$hide_closed";
// By default only show alarms from the past week
/*
// DK 2007/04/02
if (!GET('date_from')) {
list($y, $m, $d) = explode('-', date('Y-m-d'));
$date_from = date('Y-m-d', mktime(0, 0, 0, $m, $d-7, $y));
} else {
*/
$date_from = preg_replace("/(\d\d)\/(\d\d)\/(\d\d\d\d)/", "\\3-\\1-\\2", GET('date_from'));
/*
}
*/
$date_to = preg_replace("/(\d\d)\/(\d\d)\/(\d\d\d\d)/", "\\3-\\1-\\2", GET('date_to'));
$num_alarms_page = GET('num_alarms_page');
ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("order"));
ossim_valid($delete, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("delete"));
ossim_valid($close, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("close"));
ossim_valid($open, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("open"));
ossim_valid($delete_day, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("delete_day"));
if (ossim_error()) {
    die(ossim_error());
}
$ret1 = ossim_valid($src_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:' . _("src_ip"));
$ret2 = ossim_valid($src_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:' . _("src_ip"));
if (!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
$ret1 = ossim_valid($dst_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:' . _("dst_ip"));
$ret2 = ossim_valid($dst_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:' . _("dst_ip"));
if (!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
ossim_valid($inf, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("inf"));
ossim_valid($sup, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("order"));
ossim_valid($hide_closed, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("hide_closed"));
ossim_valid($date_from, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("from date"));
ossim_valid($date_to, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("to date"));
ossim_valid($num_alarms_page, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("field number of alarms per page"));
if (ossim_error()) {
    die(ossim_error());
}
if (!empty($delete)) {
    Alarm::delete($conn, $delete);
}
if (!empty($close)) {
    Alarm::close($conn, $close);
}
if (!empty($open)) {
    Alarm::open($conn, $open);
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
if (!empty($delete_day)) {
    Alarm::delete_day($conn, $delete_day);
}
if (GET('purge')) {
    Alarm::purge($conn);
}
if (empty($order)) $order = " a.timestamp DESC";
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
if ($num_alarms_page) {
    $ROWS = $num_alarms_page;
}
if (empty($inf)) $inf = 0;
if (!$sup) $sup = $ROWS;
$query = ($_GET['query'] != "") ? $_GET['query'] : "";
// Eficiencia mejorada (Granada, junio 2009)
list($alarm_list, $count) = Alarm::get_list3($conn, $src_ip, $dst_ip, $hide_closed, "ORDER BY $order", $inf, $sup, $date_from, $date_to, $query);
if (!isset($_GET["hide_search"])) {
?>

<form method="GET">
<table width="90%" align="center" class="noborder"><tr><td class="nobborder left">
<a href="javascript:;" onclick="tooglebtn()"><img src="../pixmaps/sem/toggle_up.gif" border="0" id="timg" title="Toggle"> <small><font color="black"><?=_("Filters, Actions and Options")?></font></small></a>
</td></tr></table>
<table width="90%" align="center" id="searchtable">
<tr>
	<th><?php echo _("Filter") ?></th>
	<th>Actions</th>
	<th>Options</th>
</tr>
<tr>
	<td class="nobborder" style="text-align:center">
		<table class="noborder">
			<tr>
				<td width="20%" style="text-align: right; border-width: 0px">
			        <b>Alarm name</b>:
			    </td>
			    <td style="text-align: left; border-width: 0px">
			        <input type="text" name="query" value="<?php echo $query ?>">
			    </td>
			</tr>
			<tr>
			    <td width="20%" style="text-align: right; border-width: 0px">
			        <b><?php echo _('Date') ?></b>:
			    </td>
			    <td style="text-align: left; border-width: 0px">
			        <?php echo _('from') ?>: <input type="text" size=10 name="date_from" id="date_from"  value="<?php echo $date_from ?>">&nbsp;
					<script>//$('#date_from').datepicker();</script>
			          <?php
    //Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_from', true))
    
?>
			        <?php echo _('to') ?>: <input type="text" size="10" name="date_to" id="date_to" value="<?php echo $date_to ?>">&nbsp;
					<script>//$('#date_to').datepicker();</script>
			          <?php
    //Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_to', true))
    
?><!-- (<?php echo _('YY-MM-DD') ?>)-->
			    </td>
			</tr>
			<tr>
			    <td width="25%" style="text-align: right; border-width: 0px">
			        <b><?php echo _("IP Address") ?></b>:
			    </td>
			    <td style="text-align: left; border-width: 0px" nowrap>    
			        <?php echo _("source") ?>: <input type="text" size="15" name="src_ip" value="<?php echo $src_ip ?>">&nbsp;&nbsp;
			        <?php echo _("destination") ?>: <input type="text" size="15" name="dst_ip" value="<?php echo $dst_ip ?>">
			    </td>
			</tr>
			<tr>
			    <td width="25%" style="text-align: right; border-width: 0px" nowrap>
			        <b><?php echo _("Num. alarms per page") ?></b>:
			    </td>
			    <td style="text-align: left; border-width: 0px">
			        <input type="text" size=3 name="num_alarms_page" value="<?php echo $ROWS ?>">
			    </td>
			</tr>
		</table>
	</td>
	<td class="nobborder" style="text-align:center">
		<table class="noborder">
			<tr><td class="nobborder">
				<a href="<?php
    echo $_SERVER["PHP_SELF"] ?>?delete_backlog=all"><?php
    echo gettext("Delete ALL alarms"); ?></a> <br><br>
				<a href="<?php
    echo $_SERVER["PHP_SELF"] ?>?purge=1"><?php
    echo gettext("Purge orphaned events"); ?></a><br><br>
				<input type="button" value="Delete selected" onclick="document.fchecks.submit();" class="btn">
			</td></tr>
		</table>
	</td>
	<td class="nobborder" style="text-align:center">
		<table class="noborder">
			<tr>
				<td style="text-align: left; border-width: 0px">
				<?php
    $hide_closed == 1 ? 1 : 0;
    $not_hide_closed = !$hide_closed;
?>
			    <input style="border:none" name="hide_closed" type="checkbox" value="1" 
			        onClick="document.location='<?php
    echo $_SERVER["PHP_SELF"] . "?order=$order&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip" . "&hide_closed=$not_hide_closed&num_alarms_page=$num_alarms_page" ?>'"
			        <?php
    if ($hide_closed) echo " checked " ?> 
			    /> <?php
    echo gettext("Hide closed alarms"); ?>
				</td>
			</tr>
			<tr>
				<td style="text-align: left; border-width: 0px">
				<?php
    if (GET('norefresh') != "") { ?>
				<input style="border:none" name="norefresh" type="checkbox" value="1" checked onclick="document.location.href='alarm_console.php?<?php echo $params_string
?>'"> Do not refresh console
				<?php
    } else { ?>
				<input style="border:none" name="norefresh" type="checkbox" value="1" onclick="document.location.href='alarm_console.php?norefresh=1&<?php echo $params_string
?>'"> Do not refresh console
				<?php
    } ?>
				</td>
			</tr>
		</table>
	</td>
</tr>
<tr><th colspan="3" style="padding:5px"><input type="submit" class="btn" value="<?php echo _("Go") ?>"></th></td>
</table>
</form>
<?php
} ?>
<br>
    <table width="100%" border=0 cellspacing=1 cellpadding=0>
      
	  <tr>
        <td colspan="11" style="padding:5px;border-bottom:0px solid white" nowrap>

		<input type="button" value="Show Grouped" onclick="document.location.href='alarm_group_console.php'" class="btn">&nbsp;

<?php
/*
* prev and next buttons
*/
$inf_link = $_SERVER["PHP_SELF"] . "?order=$order" . "&sup=" . ($sup - $ROWS) . "&inf=" . ($inf - $ROWS) . "&hide_closed=$hide_closed&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh";
$sup_link = $_SERVER["PHP_SELF"] . "?order=$order" . "&sup=" . ($sup + $ROWS) . "&inf=" . ($inf + $ROWS) . "&hide_closed=$hide_closed&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh";
if ($src_ip) {
    $inf_link.= "&src_ip=$src_ip";
    $sup_link.= "&src_ip=$src_ip";
}
if ($dst_ip) {
    $inf_link.= "&dst_ip=$dst_ip";
    $sup_link.= "&dst_ip=$dst_ip";
}
// XXX missing performance improve here
//$tot_alarms = Alarm::get_list($conn, $src_ip, $dst_ip, $hide_closed, "", null, null, $date_from, $date_to);
//$count = count($tot_alarms);
if ($inf >= $ROWS) {
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
} else {
    echo "&nbsp;&nbsp;(";
    printf(gettext("%d-%d of %d") , $inf, $count, $count);
    echo ")&nbsp;&nbsp;";
}
?>
        </td>
      </tr>
    
      <tr>
<!--
        <th><a href="<?php /* echo $_SERVER["PHP_SELF"]?>?order=<?php
echo ossim_db::get_order("plugin_id", $order) .
"&inf=$inf&sup=$sup"
*/ ?>">Plugin id</a></th>
-->
        <td class="nobborder" width="20" align="center"><input type="checkbox" name="allcheck" onclick="checkall()"></td>
		<td style="background-color:#9DD131">#</td>
        <td width="25%" style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("plugin_sid", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh"
?>"> <?php
echo gettext("Alarm"); ?> </a></td>
        <td style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("risk", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh"
?>"> <?php
echo gettext("Risk"); ?> </a></td>
        <td style="background-color:#9DD131"> <?php
echo gettext("Sensor"); ?> </td>
        <td style="background-color:#9DD131"> <?php
echo gettext("Since"); ?> </td>
        <td style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("timestamp", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh" ?>"> 
            <?php
echo gettext("Last"); ?> </a></td>
        <td style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("src_ip", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh"
?>"> <?php
echo gettext("Source"); ?> </a></td>
        <td style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("dst_ip", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh"
?>"> <?php
echo gettext("Destination"); ?> </a></td>
        <td style="background-color:#9DD131"><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("status", $order) . "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to&hide_closed=$hide_closed&norefresh=$norefresh"
?>"> <?php
echo gettext("Status"); ?> </a></td>
        <td style="background-color:#9DD131"> <?php
echo gettext("Action"); ?> </td>
      </tr>
	  <form name="fchecks" action="alarms_check_delete.php" method="post">

<?php
$time_start = time();
if ($count > 0) {
    $datemark = "";
    foreach($alarm_list as $alarm) {
        /* hide closed alarmas */
        if (($alarm->get_status() == "closed") and ($hide_closed == 1)) continue;
        $id = $alarm->get_plugin_id();
        $sid = $alarm->get_plugin_sid();
        $backlog_id = $alarm->get_backlog_id();
        /* get plugin_id and plugin_sid names */
        /*
        * never used ?
        *
        $plugin_id_list = Plugin::get_list($conn, "WHERE id = $id");
        $id_name = $plugin_id_list[0]->get_name();
        */
        /*
        $sid_name = "";
        if ($plugin_sid_list = Plugin_sid::get_list
        ($conn, "WHERE plugin_id = $id AND sid = $sid")) {
        $sid_name = $plugin_sid_list[0]->get_name();
        } else {
        $sid_name = "Unknown (id=$id sid=$sid)";
        }
        */
        $sid_name = $alarm->get_sid_name(); // Plugin_sid table just joined (Granada 27 mayo 2009)
        $date = Util::timestamp2date($alarm->get_timestamp());
        if ($backlog_id && $id==1505) {
            $since = Util::timestamp2date($alarm->get_since());
        } else {
            $since = $date;
        }
        /* show alarms by days */
        $date_slices = split(" ", $date);
        list($year, $month, $day) = split("-", $date_slices[0]);
        $date_formatted = strftime("%A %d-%b-%Y", mktime(0, 0, 0, $month, $day, $year));
        if ($datemark != $date_slices[0]) {
            $link_delete = "
                    <a href=\"" . $_SERVER["PHP_SELF"] . "?delete_day=" . $alarm->get_timestamp() . "&inf=" . ($sup - $ROWS) . "&sup=$sup&hide_closed=$hide_closed\"> " . gettext("Delete") . " </a>
                ";
            echo "
                <tr>
                  
                  <td colspan=\"11\" style='padding:5px;border-bottom:0px solid white;background-color:#B5C7DF'>
                    <!--<hr border=\"0\"/>-->
                    <b>$date_formatted</b> [$link_delete]<br/>
                    <!--<hr border=\"0\"/>-->
                  </td>
                  
                </tr>
                ";
        }
        $datemark = $date_slices[0];
?>
      <tr>
        <td class="nobborder"><input style="border:none" type="checkbox" name="check_<?php echo $backlog_id ?>_<?php echo $alarm->get_event_id() ?>" id="check_<?php echo $backlog_id ?>" value="1"></td>
        <td class="nobborder" nowrap id="plus<?php echo $inf + 1 ?>">
           <? if ($backlog_id && $id==1505) { ?>
           <a href="" onclick="show_alarm('<?php echo $backlog_id ?>','<?php echo $inf + 1 ?>');return false;"><img align='absmiddle' src='../pixmaps/plus-small.png' border='0'></a><?php echo ++$inf ?>
           <? } else { ?>
           <img align='absmiddle' src='../pixmaps/plus-small-gray.png' border='0'><font style="color:gray"><?php echo ++$inf ?></font>
           <? } ?>
        </td>
        <td class="nobborder" style="padding-left:20px"><b>
<?php
        $alarm_name = ereg_replace("directive_event: ", "", $sid_name);
        $alarm_name = Util::translate_alarm($conn, $alarm_name, $alarm);
        $alarm_name_orig = $alarm_name;
        if ($backlog_id && $id==1505) {
            $events_link = "events.php?backlog_id=$backlog_id";
            $alarm_name = "
                <a href=\"\"  onclick=\"show_alarm('" . $backlog_id . "','" . ($inf) . "');return false;\">
                  $alarm_name
                </a>
                ";
        } else {
            $events_link = $_SERVER["PHP_SELF"];
            /*$alarm_link = Util::get_acid_pair_link($date, $alarm->get_src_ip() , $alarm->get_dst_ip());*/
            $alarm_link = Util::get_acid_single_event_link ($alarm->get_snort_sid(), $alarm->get_snort_cid());
            $alarm_name = "<a href=\"" . $alarm_link . "\">$alarm_name</a>";
        }
        echo $alarm_name;

        if ($backlog_id && $id==1505) {
            $aid = $alarm->get_event_id();
            $summary = Alarm::get_alarm_stats($conn, $backlog_id, $aid);
            $event_count_label = $summary["total_count"] . " events";
        } else {
            $event_count_label = 1 . " event";
        }
        echo "<br><font color=\"#AAAAAA\" style=\"font-size: 8px;\">(" . $event_count_label . ")</font>";
?>
        </b></td>
        
        <!-- risk -->
<?php
        $src_ip = $alarm->get_src_ip();
        $dst_ip = $alarm->get_dst_ip();
        $src_port = $alarm->get_src_port();
        $dst_port = $alarm->get_dst_port();
        //$src_port = Port::port2service($conn, $alarm->get_src_port());
        //$dst_port = Port::port2service($conn, $alarm->get_dst_port());
        $sensors = $alarm->get_sensors();
        $risk = $alarm->get_risk();
        if ($risk > 7) {
            echo "
            <td class='nobborder' style='text-align:center;background-color:red'>
              <b>
                <a href=\"$events_link\">
                  <font color=\"white\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } elseif ($risk > 4) {
            echo "
            <td class='nobborder' style='text-align:center;background-color:orange'>
              <b>
                <a href=\"$events_link\">
                  <font color=\"black\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } elseif ($risk > 2) {
            echo "
            <td class='nobborder' style='text-align:center;background-color:green'>
              <b>
                <a href=\"$events_link\">
                  <font color=\"white\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } else {
            echo "
            <td class='nobborder' style='text-align:center'><a href=\"$events_link\">$risk</a></td>
            ";
        }
?>
        <!-- end risk -->


        <!-- sensor -->
        <td class="nobborder" style="text-align:center">
<?php
        foreach($sensors as $sensor) {
?>
          <a href="../sensor/sensor_plugins.php?sensor=<?php
            echo $sensor ?>"
            ><?php
            echo Host::ip2hostname($conn, $sensor) ?></a>  
<?php
        }
        if (!count($sensors)) {
            echo "&nbsp;";
        }
?>
        </td>
        <!-- end sensor -->


        <td nowrap style="padding-left:3px;padding-right:3px" class="nobborder">
        <?php
        $acid_link = Util::get_acid_events_link($since, $date, "time_a");
        echo "
            <a href=\"$acid_link\">
              <font color=\"black\">$since</font>
            </a>
            ";
?>
        </td>
        <td nowrap style="padding-left:3px;padding-right:3px" class="nobborder">
        <?php
        $acid_link = Util::get_acid_events_link($since, $date, "time_d");
        echo "
            <a href=\"$acid_link\">
              <font color=\"black\">$date</font></a>
            ";
?>
        </td>
        
<?php
        $src_link = "../report/index.php?host=$src_ip&section=events";
        $dst_link = "../report/index.php?host=$dst_ip&section=events";
        $src_name = Host::ip2hostname($conn, $src_ip);
        $dst_name = Host::ip2hostname($conn, $dst_ip);
        $src_img = Host_os::get_os_pixmap($conn, $src_ip);
        $dst_img = Host_os::get_os_pixmap($conn, $dst_ip);
        $src_country = strtolower(geoip_country_code_by_addr($gi, $src_ip));
        $src_country_img = "<img src=\"/ossim/pixmaps/flags/" . $src_country . ".png\">";
        $dst_country = strtolower(geoip_country_code_by_addr($gi, $dst_ip));
        $dst_country_img = "<img src=\"/ossim/pixmaps/flags/" . $dst_country . ".png\">";
?>
        <!-- src & dst hosts -->
        <td nowrap style="padding-left:3px;padding-right:3px" class="nobborder">
            <?php
        if ($src_country) {
            echo "<a href=\"$src_link\">$src_name</a>:$src_port $src_img $src_country_img";
        } else {
            echo "<a href=\"$src_link\">$src_name</a>:$src_port $src_img";
        }
?></td>
        <td nowrap style="padding-left:3px;padding-right:3px" class="nobborder">
		<?php
        if ($dst_country) {
            echo "<a href=\"$dst_link\">$dst_name</a>:$dst_port $dst_img $dst_country_img";
        } else {
            echo "<a href=\"$dst_link\">$dst_name</a>:$dst_port $dst_img";
        }
?></td>
        <!-- end src & dst hosts -->
        
        <td nowrap bgcolor="<?php echo ($alarm->get_status() == "open") ? "#ECE1DC" : "#DEEBDB" ?>" style="color:#4C7F41;border:1px solid <?php echo ($alarm->get_status() == "open") ? "#E6D8D2" : "#D6E6D2" ?>">
<?php
        $event_id = $alarm->get_event_id();
        if (($status = $alarm->get_status()) == "open") {
            echo "<a title='" . gettext("Click here to close alarm") . " #$event_id' " . "href=\"" . $_SERVER['PHP_SELF'] . "?close=$event_id" . "&sup=" . "$sup" . "&inf=" . ($sup - $ROWS) . "&hide_closed=$hide_closed\"" . " style='color:#923E3A'><b>" . gettext($status) . "</b></a>";
        } else {
            //echo gettext($status);
            echo "<a title='" . gettext("Click here to open alarm") . " #$event_id' " . "href=\"" . $_SERVER['PHP_SELF'] . "?open=$event_id" . "&sup=" . "$sup" . "&inf=" . ($sup - $ROWS) . "&hide_closed=$hide_closed\"" . " style='color:#4C7F41'><b>" . gettext($status) . "</b></a>";
        }
?>
        </td>

        <td nowrap class="nobborder" style='text-align:center'>
<?php
        if (($status = $alarm->get_status()) == "open") {
            echo "<a title='" . gettext("Click here to close alarm") . " #$event_id' " . "href=\"" . $_SERVER['PHP_SELF'] . "?close=$event_id" . "&sup=" . "$sup" . "&inf=" . ($sup - $ROWS) . "&hide_closed=$hide_closed\"" . " style='color:#923E3A'><img src='../pixmaps/cross-circle-frame.png' border='0' alt='Close alarm' title='Close alarm'></a>";
        } else {
            //echo gettext($status);
            echo "<img src='../pixmaps/cross-circle-frame-gray.png' border='0' alt='Alarm closed' title='Alarm closed'>";
        }
?>
        <a class="greybox" title="New ticket for Alert ID<?php echo $aid
?>" href="<?php
        echo "../incidents/newincident.php?" . "ref=Alarm&" . "title=" . urlencode($alarm_name_orig) . "&" . "priority=$risk&" . "src_ips=$src_ip&" . "event_start=$since&" . "event_end=$date&" . "src_ports=$src_port&" . "dst_ips=$dst_ip&" . "dst_ports=$dst_port" ?>"><img src="../pixmaps/script--pencil.png" alt="ticket" title="ticket" border="0"/></a>
        </td>
      </tr>
	  
	  <tr><td colspan=11 id="tr<?php echo $inf ?>"></td></tr>
<?php
    } /* foreach alarm_list */
?>
</form>
      <tr>
      <td colspan="11" style="padding:10px;border-bottom:1px solid white">
<?php
    if ($backup_inf >= $ROWS) {
        echo "<a href=\"$inf_link\">&lt;-";
        printf(gettext("Prev %d") , $ROWS);
        echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $backup_inf, $sup, $count);
        echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">";
        printf(gettext("Next %d") , $ROWS);
        echo " -&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;(";
        printf(gettext("%d-%d of %d") , $backup_inf, $count, $count);
        echo ")&nbsp;&nbsp;";
    }
?>
      </td></tr>
<?php
} /* if alarm_list */
?>
    </table>


<?php
$time_load = time() - $time_start;
echo "[ " . gettext("Page loaded in") . " $time_load " . gettext("seconds") . " ]";
$db->close($conn);
geoip_close($gi);
?>
<script>
// GreyBox
$(document).ready(function(){
	GB_TYPE = 'w';
	$("a.greybox2").click(function(){
		var t = this.title || $(this).text() || this.href;
		GB_show(t,this.href,450,'90%');
		return false;
	});
	
	$('#date_from').datepicker();
	$('#date_to').datepicker();
});
</script>
</body>
</html>


