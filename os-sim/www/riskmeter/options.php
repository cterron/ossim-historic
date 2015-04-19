<html>
<head>
  <link rel="stylesheet" href="../style/style.css"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
</head>

<body>

<?php

    require_once 'classes/Security.inc';

    $ip = GET('ip');
    
    ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("IP address"));

    if (ossim_error()) {
        die(ossim_error());
    }

    require_once "ossim_conf.inc";
    $conf = $GLOBALS["CONF"];
    $acid_link = $conf->get_conf("acid_link");
    $acid_prefix = $conf->get_conf("event_viewer");
    $ntop_link = $conf->get_conf("ntop_link");
    $mrtg_link = $conf->get_conf("mrtg_link");

    require_once "ossim_db.inc";
    $db = new ossim_db();
    $conn = $db->connect();

    require_once "classes/Sensor.inc";
?>

<p align="center">
  <b><?php echo $ip ?></b><br/>

[ <a href="<?php echo "$acid_link/".$acid_prefix."_stat_ipaddr.php?ip=$ip&netmask=32"?>"
     target="main"> <?php echo gettext("Events"); ?> </a> ] 
[ <a href="<?php 
//        echo "$mrtg_link/host_qualification/$ip.html" 
        echo "../control_panel/show_image.php?range=day&ip=$ip&what=compromise&start=N-1D&type=host&zoom=1"
?>"
     target="main"> <?php echo gettext("History"); ?> </a> ] 
[ <a href="<?php echo Sensor::get_sensor_link($conn, $ip) . "/$ip" ?>.html" 
     target="main"> <?php echo gettext("Monitor"); ?> </a> ]
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
