<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
<title> OSSIM </title>
</head>

<?php
require_once 'classes/Security.inc';

$host = GET('host');

ossim_valid($host, OSS_IP_ADDR, 'illegal:'._("Host"));

if (ossim_error()) {
    die(ossim_error());
}

?>

<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php?host=<?php echo $host; ?>">

<?php 

    /* inventory */
    if (!strcmp(GET('section'), 'inventory')) {
        echo "<frame src=\"inventory.php?host=" . $host . "&origin=passive\" name=\"report\">";
    }
    
    /* metrics */
    elseif (!strcmp(GET('section'), 'metrics')) {
        echo "<frame src=\"metrics.php?host=" . $host . "\" name=\"report\">";
    }

    /* events */
    elseif (!strcmp(GET('section'), 'events')) {
        require_once ('ossim_conf.inc');

        $conf = $GLOBALS["CONF"];

        $acid_link = $conf->get_conf("acid_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $acid_main_link = $conf->get_conf("acid_link") . $acid_prefix . "_stat_ipaddr.php?ip=$host&netmask=32";

        echo "<frame src=\"". $acid_main_link . "\" name=\"report\">";
    }

    /* ntop */
    elseif (!strcmp(GET('section'), 'usage')) {
    
        require_once ('ossim_db.inc');
        require_once ('classes/Sensor.inc');
        $db = new ossim_db();
        $conn = $db->connect();
        $ntop_link = Sensor::get_sensor_link($conn, $host);
        $db->close($conn);
        
        echo "<frame src=\"$ntop_link/" . $host . ".html\" name=\"report\">";
    }
    

    /* default */
    else {
        echo "<frame src=\"inventory.php?host=" . $host . "&origin=passive\" name=\"report\">";
    }
?>

<frame src="inventory.php?host=<?php echo $host; ?>" name="report">
<body>
</body>
</html>

