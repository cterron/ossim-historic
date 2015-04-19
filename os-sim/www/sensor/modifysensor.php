<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Update sensor"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$name   = POST('name');
$ip     = POST('ip');
$port   = POST('port');
$descr  = POST('descr'); 
$priority = POST('priority');

ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Sensor name"));
ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("Ip address"));
ossim_valid($port, OSS_DIGIT, 'illegal:'._("Port number"));
ossim_valid($descr, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Description"));
ossim_valid($priority, OSS_DIGIT, OSS_DOT, 'illegal:'._("Priority"));

if (ossim_error()) {
    die(ossim_error());
}

if (POST('insert')) {
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    
    Sensor::update ($conn, $name, $ip, $priority, $port, $descr);

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Sensor succesfully updated"); ?> </p>
    <p><a href="sensor.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>
