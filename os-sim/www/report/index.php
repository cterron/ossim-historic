<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
<title> OSSIM </title>
</head>

<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php?host=<?php echo $_GET["host"] ?>">

<?php 

    /* inventory */
    if (!strcmp($_GET["section"], 'inventory')) {
        echo "<frame src=\"inventory.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }
    
    /* metrics */
    elseif (!strcmp($_GET["section"], 'metrics')) {
        echo "<frame src=\"metrics.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }

    /* alerts */
    elseif (!strcmp($_GET["section"], 'alerts')) {
        require_once ('ossim_conf.inc');

        $conf = new ossim_conf();
        $ip = $_GET["host"];

        $acid_link = $conf->get_conf("acid_link");
        $acid_prefix = $conf->get_conf("alert_viewer");
        $acid_main_link = $conf->get_conf("acid_link") . $acid_prefix . "_stat_ipaddr.php?ip=$ip&netmask=32";

        echo "<frame src=\"". $acid_main_link . "\" name=\"report\">";
    }

    /* ntop */
    elseif (!strcmp($_GET["section"], 'usage')) {
    
        require_once ('ossim_db.inc');
        require_once ('classes/Sensor.inc');
        $db = new ossim_db();
        $conn = $db->connect();
        $ntop_link = Sensor::get_sensor_link($conn, $_GET["host"]);
        $db->close($conn);
        
        echo "<frame src=\"$ntop_link/" . $_GET["host"] . 
            ".html\" name=\"report\">";
    }
    

    /* default */
    else {
        echo "<frame src=\"inventory.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }
?>

<frame src="inventory.php?host=<?php echo $_GET["host"] ?>" name="report">
<body>
</body>
</html>

