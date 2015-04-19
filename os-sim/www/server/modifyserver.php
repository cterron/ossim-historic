<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyServers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Update server"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$name   = POST('name');
$ip     = POST('ip');
$port   = POST('port');
$descr  = POST('descr'); 
$correlate       = POST('correlate');
$cross_correlate = POST('cross_correlate');
$store           = POST('store');
$qualify         = POST('qualify');
$resend_alarms   = POST('resend_alarms');
$resend_events   = POST('resend_events');

ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Server name"));
ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("Ip address"));
ossim_valid($port, OSS_DIGIT, 'illegal:'._("Port number"));
ossim_valid($descr, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Description"));
ossim_valid($correlate, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Correlation"));
ossim_valid($cross_correlate, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Cross Correlation"));
ossim_valid($store, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Store"));
ossim_valid($qualify, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Qualify"));
ossim_valid($resend_alarms, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Resend Alarms"));
ossim_valid($resend_events, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Resend Events"));

if (ossim_error()) {
    die(ossim_error());
}

if (POST('insert')) {
    require_once 'ossim_db.inc';
    require_once 'classes/Server.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    
    Server::update ($conn, $name, $ip, $port, $descr, $correlate, $cross_correlate, $store, $qualify, $resend_alarms, $resend_events );

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Server succesfully updated"); ?> </p>
    <p><a href="server.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>
