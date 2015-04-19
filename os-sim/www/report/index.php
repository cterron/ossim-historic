<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
<title> OSSIM </title>
</head>

<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php?host=<?php echo validateVar($_GET["host"], OSS_IP) ?>">

<?php 

    /* inventory */
    if (!strcmp($_GET["section"], 'inventory')) {
        echo "<frame src=\"inventory.php?host=" . validateVar($_GET["host"], OSS_IP) . "\" name=\"report\">";
    }
    
    /* metrics */
    elseif (!strcmp($_GET["section"], 'metrics')) {
        echo "<frame src=\"metrics.php?host=" . validateVar($_GET["host"], OSS_IP) . "\" name=\"report\">";
    }

    /* events */
    elseif (!strcmp($_GET["section"], 'events')) {
        require_once ('ossim_conf.inc');

        $conf = $GLOBALS["CONF"];
        $ip = validateVar($_GET["host"], OSS_IP);

        $acid_link = $conf->get_conf("acid_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $acid_main_link = $conf->get_conf("acid_link") . $acid_prefix . "_stat_ipaddr.php?ip=$ip&netmask=32";

        echo "<frame src=\"". $acid_main_link . "\" name=\"report\">";
    }

    /* ntop */
    elseif (!strcmp($_GET["section"], 'usage')) {
    
        require_once ('ossim_db.inc');
        require_once ('classes/Sensor.inc');
        $db = new ossim_db();
        $conn = $db->connect();
        $ntop_link = Sensor::get_sensor_link($conn, validateVar($_GET["host"]));
        $db->close($conn);
        
        echo "<frame src=\"$ntop_link/" . validateVar($_GET["host"]) . 
            ".html\" name=\"report\">";
    }
    

    /* default */
    else {
        echo "<frame src=\"inventory.php?host=" . validateVar($_GET["host"]) . "\" name=\"report\">";
    }
?>

<frame src="inventory.php?host=<?php echo validateVar($_GET["host"]) ?>" name="report">
<body>
</body>
</html>

