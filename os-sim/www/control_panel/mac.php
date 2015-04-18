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

if (!$offset = intval($_GET["offset"])){ $offset = 0;}
if (!$count = intval($_GET["count"])){ $count = 50;}

$where_clause = " limit $count offset $offset ";

?>
<ul>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=10"> Show 10 </a>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=50"> Show 50 </a>
<li> <a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset); ?>&count=100"> Show 100 </a>
</ul>
<?php

$db = new ossim_db();
$conn = $db->connect();
?>
<table width="100%">
<tr><th> Host </th><th> Mac </th><th> Vendor </th><th> Previous Mac</th><th>
Previous Vendor</th><th> When </th></tr>


<?php
if ($host_mac_list = Host_mac::get_list($conn, $where_clause, "")) {
    foreach($host_mac_list as $host_mac) {
?>
<tr>
<?php
        $ip = $host_mac->get_ip();
        $mac_time = $host_mac->get_mac_time();
        $mac = $host_mac->get_mac();
        $anom = $host_mac->get_anom();
        if(ereg("\|",$mac)){
            list($mac, $mac_vendor) = split ("\|", $mac, 2);
        } else {
        $mac_vendor = "Unknown";
        }
        $previous = $host_mac->get_previous();
        if(ereg("\|",$previous)){
            list($previous, $previous_vendor) = split ("\|", $previous, 2);
        } else {
        $previous_vendor = "Unknown";
        }
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
<td><?php echo $previous_vendor;?></td><td><?php echo $mac_time;?></tr>
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
<td colspan=6><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset+$count); ?>&count=<?php echo $count;?>"> Next <?php echo $count ?> </a></td>
<?php
} else {
?>
<td colspan=3><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset-$count); ?>&count=<?php echo $count;?>"> Previous <?php echo $count ?></a></td>
<td colspan=3><a href="<?php echo $_SERVER["PHP_SELF"] ?>?offset=<?php echo intval($offset+$count); ?>&count=<?php echo $count;?>"> Next <?php echo $count ?> </a></td>
<?php
}
?>
</tr>
</table>
</body>
</html>

