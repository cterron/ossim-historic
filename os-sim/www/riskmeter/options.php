<html>
<head>
  <link rel="stylesheet" href="../style/style.css"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
</head>

<body>

<?php

    if (!$_GET["ip"]) {
        echo "No Ip to show! Wrong params.\n";
        exit();
    }
    $ip = $_GET["ip"];

    require_once "ossim_conf.inc";
    $conf = new ossim_conf();
    $acid_link = $conf->get_conf("acid_link");
    $ntop_link = $conf->get_conf("ntop_link");
    $mrtg_link = $conf->get_conf("mrtg_link");

    require_once "ossim_db.inc";
    $db = new ossim_db();
    $conn = $db->connect();

?>

<p align="center">
  <b><?php echo $ip ?></b><br/>

[ <a href="<?php echo "$acid_link/acid_stat_ipaddr.php?ip=$ip&netmask=32"?>"
     target="main">Alerts</a> ] 
[ <a href="<?php 
//        echo "$mrtg_link/host_qualification/$ip.html" 
        echo "../control_panel/show_image.php?range=day&ip=$ip&what=compromise&start=N-1D&type=host&zoom=1"
?>"
     target="main">History</a> ] 
[ <a href="<?php echo ossim_db::get_sensor_link($conn, $ip) . "/$ip" ?>.html" 
     target="main">Monitor</a> ]
<!--
[ <a href="<?php echo "$ntop_link/$ip" ?>.html" 
     target="main">Monitor</a> ]
-->
[ <a href="resetip.php?ip=<?php echo $ip ?>"
     target="main">Reset</a> ]
</p>

<?php
    $db->close($conn);
?>

</body>
</html>
