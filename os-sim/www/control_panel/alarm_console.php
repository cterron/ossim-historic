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
require_once ('classes/Alert.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');

function timestamp2date($timestamp){ 
    return(substr($timestamp, 0, 4) . '-' . substr($timestamp, 4, 2) . '-' . substr($timestamp, 6, 2)
    . ' ' . substr($timestamp, 8, 2) . ':' . substr($timestamp, 10, 2) . ':' . substr($timestamp, 12, 2));
} 

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

if ($id = $_GET["delete"]) {
    Alert::delete($conn, $id);
}


if (!$order = $_GET["order"]) $order = "timestamp DESC";

if (($src_ip = $_GET["src_ip"]) && ($dst_ip = $_GET["dst_ip"])) {
    $where = "WHERE (alarm = 1) AND (inet_ntoa(src_ip) = '$src_ip' 
                                        OR inet_ntoa(dst_ip) = '$dst_ip')";
} elseif ($src_ip = $_GET["src_ip"]) {
    $where = "WHERE alarm = 1 AND inet_ntoa(src_ip) = '$src_ip'";
} elseif ($dst_ip = $_GET["dst_ip"]) {
    $where = "WHERE alarm = 1 AND inet_ntoa(dst_ip) = '$dst_ip'";
} else {
    $where = 'WHERE alarm = 1';
}

if (!$inf = $_GET["inf"])
    $inf = 0;
if (!$sup = $_GET["sup"])
    $sup = 25;

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
            "&sup=" . ($sup - 25) .
            "&inf=" . ($inf - 25);
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + 25) .
        "&inf=" . ($inf + 25);
    if ($src_ip) {
        $inf_link .= "&src_ip=$src_ip";
        $sup_link .= "&src_ip=$src_ip";
    }
    if ($dst_ip) {
        $inf_link .= "&dst_ip=$dst_ip";
        $sup_link .= "&dst_ip=$dst_ip";
    }
    $count = Alert::get_count($conn, $where);
    
    if ($inf >= 25) {
        echo "<a href=\"$inf_link\">&lt;- Prev 25</a>";
    }
    echo "&nbsp;&nbsp;($inf-$sup of $count)&nbsp;&nbsp;";
    if ($sup < $count) {
        echo "<a href=\"$sup_link\">Next 25 -&gt;</a>";
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
                "&inf=$inf&sup=$sup"
            ?>">Alarm</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("risk_a", $order) .
                "&inf=$inf&sup=$sup"
            ?>">Risk</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("timestamp", $order) .
                "&inf=$inf&sup=$sup"
            ?>">Date</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("src_ip", $order) .
                "&inf=$inf&sup=$sup"
            ?>">Source</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("dst_ip", $order) .
                "&inf=$inf&sup=$sup"
            ?>">Destination</a></th>
        <th>Action</th>
      </tr>

<?php
    if ($alert_list = Alert::get_list($conn, 
                                      "$where ORDER BY $order", 
                                      $inf, $sup)) 
    {
        foreach ($alert_list as $alert) {

            $id  = $alert->get_plugin_id();
            $sid = $alert->get_plugin_sid();

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
<!--    <td><?php // echo "$id_name ($id)"; ?></td>    -->
        <td><b><?php 
                echo ereg_replace("directive_alert: ", "", $sid_name);
        ?></b></td>
        
        <!-- risk A -->
<?php 
        $date = timestamp2date($alert->get_timestamp());
        $src_ip = $alert->get_src_ip();
        $dest_ip = $alert->get_dst_ip();

        $risk_a = $alert->get_risk_a();
        if ($risk_a  > 7) {
            echo "<td bgcolor=\"red\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"white\">$risk_a</font></a></b></td>";
        } elseif ($risk_a > 4) {
            echo "<td bgcolor=\"orange\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"black\">$risk_a</font></b></td>";
        } elseif ($risk_a > 2) {
            echo "<td bgcolor=\"green\"><b><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\"><font color=\"white\">$risk_a</font></b></td>";
        } else {
            echo "<td><a href=\"".
                get_acid_date_link($date, $src_ip, "ip_src")
                ."\">$risk_a</a></td>";
        }
?>
        <!-- end risk A -->

        <td nowrap><?php echo $date ?></td>
        <td bgcolor="#eeeeee">
        <a href="../report/index.php?host=<?php echo $alert->get_src_ip() 
        ?>&section=alerts"><?php echo $alert->get_src_ip(); ?></a> <?php echo ":" . 
        $alert->get_src_port() ?>
        </td>
        <td bgcolor="#eeeeee">
        <a href="../report/index.php?host=<?php echo $alert->get_dst_ip()
        ?>&section=alerts"><?php echo $alert->get_dst_ip(); ?></a> <?php echo ":" .
        $alert->get_dst_port() ?>
        </td>
        <td><a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete=<?php 
            echo $alert->get_id() ?>">Ack</a></td>
      </tr>
<?php
        } /* foreach alert_list */
?>
      <tr>
        <td colspan="8"><a href="<?php 
            echo $_SERVER["PHP_SELF"] ?>?delete=all">Delete ALL</a>
        </td>
      </tr>
<?php
    } /* if alert_list */
?>
    </table>


<?php
$db->close($conn);
?>

</body>
</html>


