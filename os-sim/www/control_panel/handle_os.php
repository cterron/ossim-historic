<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAnomalies");
?>

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
require_once 'classes/Host_os.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($key,$val) = each($_GET)) {
    list($place_holder, $ip) = split (",", $key, 2);
    $os = base64_decode($val);
    if(preg_match("/ack/i", $os)){
        $ip = mysql_escape_string($ip);
        $os = mysql_escape_string($os);
        $os = ereg_replace("ack","",$os);
        if(ereg(" or ", $os)){
            $os = ereg_replace(" or ","|",$os);
        }
        $ip = ereg_replace ("_",".",$ip);
        Host_os::ack($conn,$ip,$os);
    } elseif (preg_match("/ignore/i", $os)){
        $ip = mysql_escape_string($ip);
        $os = mysql_escape_string($os);
        $os = ereg_replace("ignore","",$os);
        if(ereg(" or ", $os)){
            $os = ereg_replace(" or ","|",$os);
        }
        $ip = ereg_replace ("_",".",$ip);
        Host_os::ignore($conn,$ip,$os);
    }
}

    $db->close($conn);
?>
    <p>Successfully Acked/Deleted/Ignored</p>
    <p><a href="anomalies.php">Back</a></p>

</body>
</html>

