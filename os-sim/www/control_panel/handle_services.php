<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAnomalies");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("OSSIM Framework"); ?> </h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Host_services.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($key,$val) = each($_GET)) {
    list($place_holder, $ip, $sensor, $date, $port) = split (",", $key, 5);
    $ip = $val;
    if(preg_match("/ack/i", $ip)){
        $sensor = validateVar($sensor);
        $ip = ereg_replace("ack","",$ip);
        $ip = ereg_replace ("_",".",$ip);
        $sensor = ereg_replace ("_",".",$sensor);
        $date = ereg_replace ("_"," ",$date);
        $ip = validateVar($ip, OSS_IP);
        $date = validateVar($date);
        $port = validateVar($port, OSS_DIGIT);
        print "Ack: $ip $date $sensor<br>";
        Host_services::ack_ign($conn, $ip, $port, $date, $sensor);
    } elseif(preg_match("/ignore/i", $ip)){
        $sensor = validateVar($sensor);
        $ip = ereg_replace("ignore","",$ip);
        $ip = ereg_replace ("_",".",$ip);
        $sensor = ereg_replace ("_",".",$sensor);
        $ip = validateVar($ip, OSS_IP);
        $date = ereg_replace ("_"," ",$date);
        $date = validateVar($date);
        $port = validateVar($port);
        print "Ignore: $ip $date $sensor<br>";
        Host_services::ack_ign($conn, $ip, $port, $date, $sensor);
    }
}

    $db->close($conn);
?>
    <p> <?php echo gettext("Successfully Acked/Deleted"); ?> </p>
    <p><a href="anomalies.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

