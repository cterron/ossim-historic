<html>
<head>
  <title>OSSIM</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<?php

    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();

    if (!$sensor = $_GET["sensor"]) 
    {
        require_once ('ossim_db.inc');
        require_once ('classes/Sensor.inc');

        $db = new ossim_db();
        $conn = $db->connect();

        echo "<p align=\"center\">Please select a sensor from the following list:</p>
              <p align=\"center\">";

        if ($sensor_list = Sensor::get_list($conn)) {
            foreach ($sensor_list as $sensor) {
                $name = $sensor->get_name();
                $ip   = $sensor->get_ip();
                echo "<a href=\"" . $_SERVER["PHP_SELF"] . 
                    "?sensor=$ip\">" . $name . " (". $ip .
                    ")</a><br/>";
            }
        }
        echo "</p>";

        $db->close($conn);
       
    } else {

        # get ntop port from default ntop entry at
        # /etc/ossim/framework/ossim.conf
        # a better solution ??
        list($protocol, $ip, $port) = 
            split(':', $conf->get_conf("ntop_link"));
?>
<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php?sensor=<?php echo $sensor ?>&port=<?php echo $port ?>&proto=<?php echo $protocol ?>">
<frame src="<?php echo "http://$sensor:$port" ?>/trafficStats.html" name="ntop">

<?php
    }
?>

<body>
</body>
</html>

