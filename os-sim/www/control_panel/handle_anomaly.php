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
require_once 'classes/RRD_anomaly.inc';
require_once 'classes/RRD_anomaly_global.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();
while (list($key,$val) = each($_GET)) {
list($action, $ip, $what) = split (",", $key, 3);
$what = ereg_replace("_"," ",$what);
$what = ereg_replace("rrd anomaly","rrd_anomaly",$what);
if($ip == "Global"){
    switch($action){
    case 'ack':
        RRD_anomaly_global::ack($conn,$what);
        break;
        case 'del':
        RRD_anomaly_global::delete($conn,$what);
        break;
        }
    } else {
    $ip = ereg_replace ("_",".",$ip);
    switch($action){
        case 'ack':
        RRD_anomaly::ack($conn,$ip,$what);
        break;
        case 'del':
        RRD_anomaly::delete($conn,$ip,$what);
        break;
        }
    }
}

    $db->close($conn);
?>
    <p> <?php echo gettext("Successfully Acked/Deleted"); ?> </p>
    <p><a href="anomalies.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

