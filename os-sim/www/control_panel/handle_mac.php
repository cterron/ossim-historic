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
require_once 'classes/Host_mac.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($key,$val) = each($_GET)) {
    list($place_holder, $ip) = split (",", $key, 2);
    $mac = base64_decode($val);
    if(preg_match("/ack/i", $mac)){
        $ip = mysql_escape_string($ip);
        $mac = mysql_escape_string($mac);
        $mac = ereg_replace("ack","",$mac);
        if(ereg(" or ", $mac)){
            $mac = ereg_replace(" or ","|",$mac);
        }
        $ip = ereg_replace ("_",".",$ip);
        Host_mac::ack($conn,$ip,$mac);
    } elseif(preg_match("/ignore/i", $mac)){
        $ip = mysql_escape_string($ip);
        $mac = mysql_escape_string($mac);
        $mac = ereg_replace("ignore","",$mac);
        if(ereg(" or ", $mac)){
            $mac = ereg_replace(" or ","|",$mac);
        }
        $ip = ereg_replace ("_",".",$ip);
        Host_mac::ignore($conn,$ip,$mac);
    }
}

    $db->close($conn);
?>
    <p>Successfully Acked/Deleted</p>
    <p><a href="index.php">Back</a></p>

</body>
</html>

