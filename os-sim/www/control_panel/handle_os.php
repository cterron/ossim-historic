<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsAnomalies");
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
require_once 'classes/Host_os.inc';
require_once 'classes/Security.inc';
?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($key,$val) = each($_GET)) {
    list($place_holder, $ip, $sensor, $date) = split (",", $key, 4);
    $ip = $val;
    if(preg_match("/ack/i", $ip)){
        $ip = ereg_replace("ack","",$ip);
        $ip = ereg_replace ("_",".",$ip);
        $sensor = ereg_replace ("_",".",$sensor);
        $date = ereg_replace ("_"," ",$date);

        ossim_valid($ip, OSS_IP_ADDR , 'illegal:'._("ip"));
        ossim_valid($sensor, OSS_IP_ADDR, 'illegal:'._("Sensor"));
        ossim_valid($date, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Date"));
        
        if (ossim_error()) {
            die(ossim_error());
        }
        
        print "Ack: $ip $date $sensor<br>";
        Host_os::ack_ign($conn, $ip, $date, $sensor);
    } elseif(preg_match("/ignore/i", $ip)){
        $ip = ereg_replace("ignore","",$ip);
        $ip = ereg_replace ("_",".",$ip);
        $sensor = ereg_replace ("_",".",$sensor);
        $date = ereg_replace ("_"," ",$date);
        
        ossim_valid($ip, OSS_IP_ADDR , 'illegal:'._("ip"));
        ossim_valid($sensor, OSS_IP_ADDR, 'illegal:'._("Sensor"));
        ossim_valid($date, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Date"));
        
        if (ossim_error()) {
            die(ossim_error());
        }
        
        print "Ignore: $ip $date $sensor<br>";
        Host_os::ack_ign($conn, $ip, $date, $sensor);
    }
}

    $db->close($conn);
?>
    <p> <?php echo gettext("Successfully Acked/Deleted"); ?> </p>
    <p><a href="anomalies.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

