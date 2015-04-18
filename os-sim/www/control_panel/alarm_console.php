<html>
<head>
  <title> Control Panel </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center">Alarms</h1>

<?php
require_once ('ossim_db.inc');
require_once ('common.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');

/* number of alerts by page */
$ROWS = 50;

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

if ($id = $_GET["delete"]) {
    Alarm::delete($conn, $id);
}

if ($id = $_GET["delete_backlog"]) {
    Alarm::delete_from_backlog($conn, $id);
}


if (!$order = $_GET["order"]) $order = "timestamp DESC";

if (($src_ip = $_GET["src_ip"]) && ($dst_ip = $_GET["dst_ip"])) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip' 
                     OR inet_ntoa(dst_ip) = '$dst_ip'";
} elseif ($src_ip = $_GET["src_ip"]) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip'";
} elseif ($dst_ip = $_GET["dst_ip"]) {
    $where = "WHERE inet_ntoa(dst_ip) = '$dst_ip'";
} else {
    $where = '';
}

if (!$inf = $_GET["inf"])
    $inf = 0;
if (!$sup = $_GET["sup"])
    $sup = $ROWS;

?>
    <table width="100%">
      <tr>
        <td colspan="8">
<?php

    /* 
     * prev and next buttons 
     */
    $inf_link = $_SERVER["PHP_SELF"] . 
            "?order=$order" . 
            "&sup=" . ($sup - $ROWS) .
            "&inf=" . ($inf - $ROWS);
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + $ROWS) .
        "&inf=" . ($inf + $ROWS);
    if ($src_ip) {
        $inf_link .= "&src_ip=$src_ip";
        $sup_link .= "&src_ip=$src_ip";
    }
    if ($dst_ip) {
        $inf_link .= "&dst_ip=$dst_ip";
        $sup_link .= "&dst_ip=$dst_ip";
    }
    $count = Alarm::get_count($conn, $src_ip, $dst_ip);
    
    if ($inf >= $ROWS) {
        echo "<a href=\"$inf_link\">&lt;- Prev $ROWS</a>";
    }
    echo "&nbsp;&nbsp;($inf-$sup of $count)&nbsp;&nbsp;";
    if ($sup < $count) {
        echo "<a href=\"$sup_link\">Next $ROWS -&gt;</a>";
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
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("plugin_sid", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip"
            ?>">Alarm</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("risk", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip"
            ?>">Risk</a></th>
        <th>Since</th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("timestamp", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip"
            ?>">Last</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("src_ip", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip"
            ?>">Source</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("dst_ip", $order) .
                "&inf=$inf&sup=$sup&src_ip=$src_ip&dst_ip=$dst_ip"
            ?>">Destination</a></th>
        <th>Action</th>
      </tr>

<?php
    if ($alarm_list = Alarm::get_list($conn, $src_ip, $dst_ip,
                                      "ORDER by $order", 
                                      $inf, $sup))
    {
        foreach ($alarm_list as $alarm) {

            $id  = $alarm->get_plugin_id();
            $sid = $alarm->get_plugin_sid();
            $backlog_id = $alarm->get_backlog_id();

            /* get plugin_id and plugin_sid names */
            $plugin_id_list = Plugin::get_list($conn, "WHERE id = $id");
            $id_name = $plugin_id_list[0]->get_name();

            $sid_name = "";
            if ($plugin_sid_list = Plugin_sid::get_list
                ($conn, "WHERE plugin_id = $id AND sid = $sid")) {
                $sid_name = $plugin_sid_list[0]->get_name();
            } else {
                $sid_name = "Unknown (id=$id sid=$sid)";
            }
        
?>
      <tr>
        <td><b>
<?php
            $alarm_name = ereg_replace("directive_alert: ", "", $sid_name);
            if ($backlog_id != 0) {
                $alarm_name = "<a href=\"alerts.php?backlog_id=$backlog_id \">$alarm_name</a>";
            }
    echo $alarm_name;
?>
        </b></td>
        
        <!-- risk -->
<?php 
        $date = timestamp2date($alarm->get_timestamp());
        if ($backlog_id != 0) {
            $since = timestamp2date($alarm->get_since());
        } else {
            $since = $date;
        }
        
        $src_ip   = $alarm->get_src_ip();
        $dst_ip   = $alarm->get_dst_ip();
        $src_port = $alarm->get_src_port();
        $dst_port = $alarm->get_dst_port();

        $risk = $alarm->get_risk();
        if ($risk  > 7) {
            echo "<td bgcolor=\"red\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"white\">$risk</font></a></b></td>";
        } elseif ($risk > 4) {
            echo "<td bgcolor=\"orange\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"black\">$risk</font></b></td>";
        } elseif ($risk > 2) {
            echo "<td bgcolor=\"green\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"white\">$risk</font></b></td>";
        } else {
            echo "<td><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\">$risk</a></td>";
        }
?>
        <!-- end risk -->
        
        <td nowrap>
        <?php
            $acid_link = get_acid_alerts_link($since, $date, "time_a");
            echo "
            <a href=\"$acid_link\">
              <font color=\"black\">$since</font>
            </a>
            ";
        ?>
        </td>
        <td nowrap>
        <?php
            $acid_link = get_acid_alerts_link($since, $date, "time_d");
            echo "
            <a href=\"$acid_link\">
              <font color=\"black\">$date</font></a>
            ";
        ?>
        </td>
        
<?php
    $src_link = "../report/index.php?host=$src_ip&section=alerts";
    $dst_link = "../report/index.php?host=$dst_ip&section=alerts";
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
        
        <td>
<?php
        if ($backlog_id == 0) {
?>
        <a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete=<?php 
            echo $alarm->get_alert_id() ?>">Ack</a>
<?php
        } else {
?>
        <a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete_backlog=<?php 
            echo $backlog_id ?>">Ack</a>
<?php
        }
?>
        </td>
      </tr>
<?php
        } /* foreach alarm_list */
?>
      <tr>
        <td colspan="8"><a href="<?php 
            echo $_SERVER["PHP_SELF"] ?>?delete_backlog=all">Delete ALL</a>
        </td>
      </tr>
<?php
    } /* if alarm_list */
?>
    </table>


<?php
$db->close($conn);
?>

</body>
</html>


