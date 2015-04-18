<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAnomalies");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework - Mac list</h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Host_mac.inc';
require_once 'classes/Host.inc';

if (!$order = $_GET["order"]) $order = "ip";
if (!$offset = intval($_GET["offset"])){ $offset = 0;}
if (!$count = intval($_GET["count"])){ $count = 50;}

$args = "ORDER BY $order LIMIT $offset,$count ";

?>
<ul>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=10&order=<?php echo $order ?>"> Show 10 </a>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=50&order=<?php echo $order ?>"> Show 50 </a>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=100&order=<?php echo $order ?>"> Show 100 </a>
</ul>
<?php

$db = new ossim_db();
$conn = $db->connect();
?>
<table width="100%">
<tr>
<th><a href="<?php echo $_SERVER["PHP_SELF"]?>?offset=<?php echo
intval($offset); ?>&count=<?php echo $count ?>&order=<?php
            echo ossim_db::get_order("ip", $order);
          ?>">Host</a></th>
<th><a href="<?php echo $_SERVER["PHP_SELF"]?>?offset=<?php echo
intval($offset); ?>&count=<?php echo $count ?>&order=<?php
            echo ossim_db::get_order("mac", $order);
          ?>">Mac</a></th>
<th>Vendor </th>
<th><a href="<?php echo $_SERVER["PHP_SELF"]?>?offset=<?php echo
intval($offset); ?>&count=<?php echo $count ?>&order=<?php
            echo ossim_db::get_order("previous", $order);
          ?>">Previous Mac</a></th>
<th>Previous Vendor</th>
<th><a href="<?php echo $_SERVER["PHP_SELF"]?>?offset=<?php echo
intval($offset); ?>&count=<?php echo $count ?>&order=<?php
            echo ossim_db::get_order("date", $order);
          ?>">When</a></th></tr>


<?php
if ($host_mac_list = Host_mac::get_list($conn, $args)) {
    foreach($host_mac_list as $host_mac) {
?>
<tr>
<?php
        $ip = $host_mac->get_ip();
        $date = $host_mac->get_date();
        $mac = $host_mac->get_mac();
        $mac_vendor = $host_mac->get_vendor();
        $anom = $host_mac->get_anom();
        $previous = $host_mac->get_previous();
        $previous_vendor = "Unknown";
if($anom){
        ?>
<th><font color="red"><?php echo Host::ip2hostname($conn, $ip);?></font></th>
<?php
} else {
?>
<th><?php echo Host::ip2hostname($conn, $ip);?></th>
<?php
}
?>
<td><?php echo $mac;?></td>
<td><?php echo $mac_vendor;?></td><td><?php echo $previous?></td>
<td><?php echo $previous_vendor;?></td><td><?php echo $date;?></tr>
<?php
    }
}
    $db->close($conn);
?>

</tr>
<tr>
<?php
if($offset == 0){
?>
<td colspan=6><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo
intval($offset+$count); ?>&count=<?php echo $count;?>&order=<?php echo $order
?>"> Next <?php echo $count ?> </a></td>
<?php
} else {
?>
<td colspan=3><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo
intval($offset-$count); ?>&count=<?php echo $count;?>&order=<?php echo $order
?>"> Previous <?php echo $count ?></a></td>
<td colspan=3><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo
intval($offset+$count); ?>&count=<?php echo $count;?>&order=<?php echo $order
?>"> Next <?php echo $count ?> </a></td>
<?php
}
?>
</tr>
</table>
</body>
</html>

