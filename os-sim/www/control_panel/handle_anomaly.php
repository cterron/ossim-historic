<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/RRD_anomaly.inc';
require_once 'classes/RRD_anomaly_global.inc';
require_once 'classes/RRD_data.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($key,$val) = each($_GET)) {
list($action, $ip, $what) = split (",", $key, 3);
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
    <p>Successfully Acked/Deleted</p>
    <p><a href="index.php">Back</a></p>

</body>
</html>

