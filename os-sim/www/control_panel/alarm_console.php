<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAlarms");
?>

<html>
<head>
  <title> Control Panel </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

<?php
require_once ('ossim_db.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');
require_once ('classes/Port.inc');
require_once ('classes/Util.inc');
require_once ('classes/Security.inc');

/* default number of events per page */
$ROWS = 50;

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

$delete = GET('delete');
$close  = GET('close');
$delete_day = GET('delete_day');
$order = GET('order');
$src_ip = GET('src_ip');
$dst_ip = GET('dst_ip');
$backup_inf = $inf = GET('inf');
$sup = GET('sup');
$hide_closed = GET('hide_closed');
// By default only show alarms from the past week
/*
// DK 2007/04/02
if (!GET('date_from')) {
    list($y, $m, $d) = explode('-', date('Y-m-d'));
    $date_from = date('Y-m-d', mktime(0, 0, 0, $m, $d-7, $y));
} else {
*/
    $date_from = GET('date_from');
/*
}
*/
$date_to = GET('date_to');
$num_alarms_page = GET('num_alarms_page');

ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
ossim_valid($delete, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("delete"));
ossim_valid($close, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("close"));
ossim_valid($delete_day, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("delete_day"));
$ret1 = ossim_valid($src_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("src_ip"));
$ret2 = ossim_valid($src_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:'._("src_ip"));
if(!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
$ret3 = ossim_valid($dst_ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("dst_ip"));
$ret4 = ossim_valid($dst_ip, OSS_IP_CIDR, OSS_NULLABLE, 'illegal:'._("dst_ip"));
if(!$ret1 && !$ret2) die(ossim_error());
// Cleanup errors
ossim_set_error(false);
ossim_valid($inf, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("inf"));
ossim_valid($sup, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("order"));
ossim_valid($hide_closed, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("hide_closed"));
ossim_valid($date_from, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("from date"));
ossim_valid($date_to, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("to date"));
ossim_valid($num_alarms_page, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("field number of alarms per page"));

if (ossim_error()) {
    die(ossim_error());
}
                    
if (!empty($delete)) {
    Alarm::delete($conn, $delete);
}

if (!empty($close)) {
    Alarm::close($conn, $close);
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

if ($num_alarms_page) {
    $ROWS = $num_alarms_page;
}

if (empty($inf))
    $inf = 0;
if (!$sup)
    $sup = $ROWS;

$alarm_list = Alarm::get_list($conn, $src_ip, $dst_ip, $hide_closed,
                              "ORDER BY $order", $inf, $sup, $date_from, $date_to);

?>
<form method="GET">
<table width="60%" align="center">
<tr><th colspan="2">
<?=_("Filter")?>
<?php
        $hide_closed  == 1 ? 1 : 0;
        $not_hide_closed = !$hide_closed;
    ?>
    (<input type="checkbox" 
        onClick="document.location='<?php echo 
            $_SERVER["PHP_SELF"] .
            "?order=$order&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip" .
            "&hide_closed=$not_hide_closed&num_alarms_page=$num_alarms_page" ?>'"
        <?php if ($hide_closed) echo " checked " ?> 
    /><?php echo gettext("Hide closed alarms"); ?>)
</th></tr>
<tr>
    <td width="20%" style="text-align: right; border-width: 0px">
        <b><?=_('Date')?></b>:
    </td>
    <td style="text-align: left; border-width: 0px">
        <?=_('from')?> <input type="text" size=10 name="date_from" value="<?=$date_from?>">&nbsp;
          <? Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_from', true))?>
        <?=_('to')?> <input type="text" size="10" name="date_to" value="<?=$date_to?>">&nbsp;
          <? Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_to', true))?> (<?=_('YY-MM-DD')?>)
    </td>
</tr>
<tr>
    <td width="25%" style="text-align: right; border-width: 0px">
        <b><?=_("IP Address")?></b>:
    </td>
    <td style="text-align: left; border-width: 0px" nowrap>    
        <?=_("source")?>: <input type="text" size="15" name="src_ip" value="<?=$src_ip?>">&nbsp;-&nbsp;
        <?=_("destination")?>: <input type="text" size="15" name="dst_ip" value="<?=$dst_ip?>">
    </td>
</tr>
<tr>
    <td width="25%" style="text-align: right; border-width: 0px" nowrap>
        <b><?=_("Num. alarms per page")?></b>:
    </td>
    <td style="text-align: left; border-width: 0px">
        <input type="text" size=3 name="num_alarms_page" value="<?=$ROWS?>">
    </td>
</tr>
<tr><th colspan="2"><input type="submit" value="<?=_("Go")?>"></th></td>
</table>
</form>
<br>
    <table width="100%">
      <tr>
        <td colspan="10">
<?php

    /* 
     * prev and next buttons 
     */
    $inf_link = $_SERVER["PHP_SELF"] . 
            "?order=$order" . 
            "&sup=" . ($sup - $ROWS) .
            "&inf=" . ($inf - $ROWS) .
            "&hide_closed=$hide_closed&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to";
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + $ROWS) .
        "&inf=" . ($inf + $ROWS) .
        "&hide_closed=$hide_closed&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to";
    if ($src_ip) {
        $inf_link .= "&src_ip=$src_ip";
        $sup_link .= "&src_ip=$src_ip";
    }
    if ($dst_ip) {
        $inf_link .= "&dst_ip=$dst_ip";
        $sup_link .= "&dst_ip=$dst_ip";
    }
    // XXX missing performance improve here
    $tot_alarms = Alarm::get_list($conn, $src_ip, $dst_ip, $hide_closed, "", null, null, $date_from, $date_to);
    $count = count($tot_alarms);
    
    if ($inf >= $ROWS) {
        echo "<a href=\"$inf_link\">&lt;-"; printf(gettext("Prev %d"),$ROWS); echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $sup, $count); echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">"; printf(gettext("Next %d"), $ROWS); echo " -&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $count, $count); echo ")&nbsp;&nbsp;";
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
        <th>#</th>
        <th width="25%"><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("plugin_sid", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to"
            ?>"> <?php echo gettext("Alarm"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("risk desc", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to"
            ?>"> <?php echo gettext("Risk"); ?> </a></th>
        <th> <?php echo gettext("Sensor"); ?> </th>
        <th> <?php echo gettext("Since"); ?> </th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("timestamp", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to" ?>"> 
            <?php echo gettext("Last"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("src_ip", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to"
            ?>"> <?php echo gettext("Source"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("dst_ip", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to"
            ?>"> <?php echo gettext("Destination"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("status", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip&num_alarms_page=$num_alarms_page&date_from=$date_from&date_to=$date_to"
            ?>"> <?php echo gettext("Status"); ?> </a></th>
        <th> <?php echo gettext("Action"); ?> </th>
      </tr>

<?php
    $time_start = time();
    if (count($alarm_list))
    {
        $datemark = "";

        foreach ($alarm_list as $alarm) {

            /* hide closed alarmas */
            if (($alarm->get_status() == "closed") and
                ($hide_closed == 1))
                continue;

            $id  = $alarm->get_plugin_id();
            $sid = $alarm->get_plugin_sid();
            $backlog_id = $alarm->get_backlog_id();

            /* get plugin_id and plugin_sid names */
            
            /*
             * never used ?
             *
            $plugin_id_list = Plugin::get_list($conn, "WHERE id = $id");
            $id_name = $plugin_id_list[0]->get_name();
             */

            $sid_name = "";
            if ($plugin_sid_list = Plugin_sid::get_list
                ($conn, "WHERE plugin_id = $id AND sid = $sid")) {
                $sid_name = $plugin_sid_list[0]->get_name();
            } else {
                $sid_name = "Unknown (id=$id sid=$sid)";
            }

            $date = Util::timestamp2date($alarm->get_timestamp());
            if ($backlog_id != 0) {
                $since = Util::timestamp2date($alarm->get_since());
            } else {
                $since = $date;
            }
 
            /* show alarms by days */
            $date_slices = split(" ", $date);
            list ($year, $month, $day) = split("-", $date_slices[0]);
            $date_formatted = strftime("%A %d-%b-%Y", 
                                       mktime(0, 0, 0, $month, $day, $year));
            if ($datemark != $date_slices[0]) 
            {
                $link_delete = "
                    <a href=\"" . 
                        $_SERVER["PHP_SELF"] . "?delete_day=" . 
                        $alarm->get_timestamp() . "&inf=".($sup-$ROWS)."&sup=$sup&hide_closed=$hide_closed\"> ". gettext("Delete")." </a>
                ";
                echo "
                <tr>
                  <td>&nbsp;</td>
                  <td colspan=\"8\">
                    <!--<hr border=\"0\"/>-->
                    <b>$date_formatted</b> [$link_delete]<br/>
                    <!--<hr border=\"0\"/>-->
                  </td>
                  <td>&nbsp;</td>
                </tr>
                ";
            }
            $datemark = $date_slices[0];
?>
      <tr>
        <td><?php echo ++$inf ?></td>
        <td><b>
<?php
            $alarm_name = ereg_replace("directive_event: ", "", $sid_name);
            $alarm_name = Util::translate_alarm($conn, $alarm_name, $alarm);
            $alarm_name_orig = $alarm_name;
            if ($backlog_id != 0) 
            {
                $events_link = "events.php?backlog_id=$backlog_id";
                $alarm_name = "
                <a href=\"$events_link\">
                  $alarm_name
                </a>
                ";
            } else {
                $events_link = $_SERVER["PHP_SELF"];
                $alarm_link = Util::get_acid_pair_link($date, 
                    $alarm->get_src_ip(), $alarm->get_dst_ip());
                $alarm_name = "<a href=\"" . $alarm_link .  "\">$alarm_name</a>";
            }
            echo $alarm_name;
if($backlog_id){
$aid = $alarm->get_event_id();
$summary = Alarm::get_alarm_stats($conn, $backlog_id, $aid);
$event_count_label = $summary["total_count"] . " events";
} else {
$event_count_label = 1 . " event";
}
echo " <font color=\"#AAAAAA\" style=\"font-size: 8px;\">(" . $event_count_label . ")</font>";
?>
        </b></td>
        
        <!-- risk -->
<?php 
        $src_ip   = $alarm->get_src_ip();
        $dst_ip   = $alarm->get_dst_ip();
        $src_port = Port::port2service($conn, $alarm->get_src_port());
        $dst_port = Port::port2service($conn, $alarm->get_dst_port());
        $sensors  = $alarm->get_sensors();

        $risk = $alarm->get_risk();
        if ($risk  > 7) {
            echo "
            <td bgcolor=\"red\">
              <b>
                <a href=\"$events_link\">
                  <font color=\"white\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } elseif ($risk > 4) {
            echo "
            <td bgcolor=\"orange\">
              <b>
                <a href=\"$events_link\">
                  <font color=\"black\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } elseif ($risk > 2) {
            echo "
            <td bgcolor=\"green\">
              <b>
                <a href=\"$events_link\">
                  <font color=\"white\">$risk</font>
                </a>
              </b>
            </td>
            ";
        } else {
            echo "
            <td><a href=\"$events_link\">$risk</a></td>
            ";
        }
?>
        <!-- end risk -->


        <!-- sensor -->
        <td>
<?php
    foreach ($sensors as $sensor) { 
?>
          <a href="../sensor/sensor_plugins.php?sensor=<?php echo $sensor ?>"
            ><?php echo Host::ip2hostname($conn, $sensor) ?></a>  
<?php 
    }
    if (!count($sensors)) {
        echo "&nbsp;";
    }
?>
        </td>
        <!-- end sensor -->


        <td nowrap>
        <?php
            $acid_link = Util::get_acid_events_link($since, $date, "time_a");
            echo "
            <a href=\"$acid_link\">
              <font color=\"black\">$since</font>
            </a>
            ";
        ?>
        </td>
        <td nowrap>
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
    $src_img  = Host_os::get_os_pixmap($conn, $src_ip);
    $dst_img  = Host_os::get_os_pixmap($conn, $dst_ip);

?>
        <!-- src & dst hosts -->
        <td bgcolor="#eeeeee">
            <?php echo "<a href=\"$src_link\">$src_name</a>:$src_port $src_img"; ?></td>
        <td bgcolor="#eeeeee">
            <?php echo "<a href=\"$dst_link\">$dst_name</a>:$dst_port $dst_img"; ?></td>
        <!-- end src & dst hosts -->
        
        <td nowrap>
<?php

    $event_id = $alarm->get_event_id();

    if ( ($status = $alarm->get_status()) == "open") {
        echo "<a title='" . gettext("Click here to close alarm") . " #$event_id' " .
             "href=\"" . $_SERVER['PHP_SELF'] . "?close=$event_id" . 
             "&sup=" . "$sup" .
             "&inf=" . ($sup-$ROWS) .
             "&hide_closed=$hide_closed\"" .
             ">" . gettext($status) . "</a>";
    } else {
        echo gettext($status);
    }
?>
        </td>

        <td nowrap>
<?php
        if ($backlog_id == 0) {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?delete=$event_id" .
            "&sup=" . "$sup" .
            "&inf=" . ($sup-$ROWS) .
            "&hide_closed=$hide_closed"; ?>">
            <?php echo gettext("Delete"); ?> </a>]
<?php
        } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?delete_backlog=" . "$backlog_id-$event_id" . 
            "&sup=" . "$sup" .
            "&inf=" . ($sup-$ROWS) .
            "&hide_closed=$hide_closed"; ?>">
            <?php echo gettext("Delete"); ?> </a>]
<?php
        }
?>
        <a href="<?php echo "../incidents/newincident.php?" .
            "ref=Alarm&"  .
            "title=".urlencode($alarm_name_orig)."&" .
            "priority=$risk&" .
            "src_ips=$src_ip&" .
            "src_ports=$src_port&" .
            "dst_ips=$dst_ip&" .
            "dst_ports=$dst_port"  ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
        </td>
      </tr>
<?php
        } /* foreach alarm_list */
?>
      <tr>
      <td colspan="10">
<?php
    if ($backup_inf >= $ROWS) {
        echo "<a href=\"$inf_link\">&lt;-"; printf(gettext("Prev %d"),$ROWS); echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$backup_inf, $sup, $count); echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">"; printf(gettext("Next %d"), $ROWS); echo " -&gt;</a>";
    } else { 
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$backup_inf, $count, $count); echo ")&nbsp;&nbsp;";
    }
?>
      </td></tr>
      <tr>
        <td></td>
        <td colspan="10">
        <a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete_backlog=all"><?php
            echo gettext("Delete ALL alarms"); ?></a> &nbsp;|&nbsp;
        <a href="<?php echo $_SERVER["PHP_SELF"] ?>?purge=1"><?php
            echo gettext("Purge orphaned events"); ?></a>
        </td>
      </tr>
<?php
    } /* if alarm_list */
?>
    </table>


<?php
$time_load = time() - $time_start;
echo  "[ ".gettext("Page loaded in")." $time_load ".gettext("seconds")." ]";
$db->close($conn);
?>

</body>
</html>


