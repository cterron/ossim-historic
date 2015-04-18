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
require_once ('classes/Host.inc');
require_once ('classes/Backlog.inc');

if (!$order = $_GET["order"]) $order = "utime";

if (($src_ip = $_GET["src_ip"]) && ($dst_ip = $_GET["dst_ip"])) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip' OR inet_ntoa(dst_ip) = '$dst_ip'";
} elseif ($src_ip = $_GET["src_ip"]) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip'";
} elseif ($dst_ip = $_GET["dst_ip"]) {
    $where = "WHERE inet_ntoa(dst_ip) = '$dst_ip'";
} else {
    $where = '';
}

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();


?>
    <table width="100%">
    <form action="handle_alarm.php" method="GET">
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
            ?>">Alarm</a></th>
        <th>Risk</th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("utime", $order);
            ?>">Date</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("src_ip", $order);
            ?>">Source</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("dst_ip", $order);
            ?>">Destination</a></th>
        <th>Description</th>
        <th>ACK</th>
      </tr>
<?php
    if ($backlog_list = Backlog::get_list($conn, "$where ORDER BY $order")) {
        foreach($backlog_list as $backlog) {
        
            $ip = $backlog->get_dst_ip();
    
            /* 
             * calculate alarm risk
             * risk = host asset * alarm realiability * alarm priority 
             */
            if ($host_list = Host::get_list($conn, "where ip = '$ip'", "")) {
                $host = $host_list[0];
                $asset = $host->get_asset();
            } else {
                $asset = 5;
            }

            $risk = intval($asset * 
                           $backlog->get_reliability() * 
                           $backlog->get_priority());
            $risk = round($risk / 3);  // Esto está mal. DK

?>
      <tr>
        <td bgcolor="#eeeeee"><?php echo $backlog->get_name(); ?></td>

        <!-- risk level -->
<?php 
            if($risk > 7) { 
?>
        <td bgcolor="red"><font color="white"><b><?php echo $risk ?></b></font></td>
<?php 
            } elseif ($risk > 4) { 
?>
        <td bgcolor="orange"><font color="black"><b><?php echo $risk ?></b></font></td>
<?php
            } elseif ($risk > 2) {
?>
        <td bgcolor="green"><font color="white"><b><?php echo $risk ?></b></font></td>
<?php
            } else {
?>
        <td><?php echo $risk ?></td>
<?php
            }
?>
        <!-- end risk level -->
        
        
        <td nowrap><?php echo date ("Y-m-d H:i:s" , 
                             $backlog->get_utime()/1000000) ?> 
        </td>
        <td bgcolor="#eeeeee">
<?php 
            echo $backlog->get_src_ip() . ":" . 
                 $backlog->get_src_port()
?>
        </td>
        <td bgcolor="#eeeeee">
<?php
            echo $backlog->get_dst_ip() . ":" .
                 $backlog->get_dst_port();
?>
        </td>
        <td><font color="blue"><?php echo $backlog->get_rule_name() ?></font></td>
        <td><input type="checkbox"
                   name="<?php echo $backlog->get_utime() ?>"/>
        </td>
      </tr>
<?php
        } /* foreach backlog_list */
?>
      <tr>
        <td colspan="13">
          <input type="submit" value="OK"/>
          <input type="reset" />
        </td>
      </tr>
<?php
    } /* if backlog_list */
?>
    </form>
    </table>


<?php
$db->close($conn);
?>

</body>
</html>


