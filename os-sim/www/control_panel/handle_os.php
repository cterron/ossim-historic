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
    $ip = mysql_escape_string($ip);
    $os = mysql_escape_string($os);
    $os = ereg_replace("ack","",$os);
    if(ereg(" or ", $os)){
    $os = ereg_replace(" or ","|",$os);
    }
    $ip = ereg_replace ("_",".",$ip);
    Host_os::ack($conn,$ip,$os);
}

    $db->close($conn);
?>
    <p>Successfully Acked/Deleted</p>
    <p><a href="index.php">Back</a></p>

</body>
</html>

