<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("New port"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$port = POST('port');
$protocol = POST('protocol');
$service = POST('service');
$descr = POST('descr');

ossim_valid($port, OSS_DIGIT, 'illegal:'._("Action id"));
ossim_valid($protocol, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Protocol"));
ossim_valid($service, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("Service"));
ossim_valid($descr, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_AT, 'illegal:'._("Description"));

if (ossim_error()) {
    die(ossim_error());
}

if(POST('insert')) {
    require_once 'ossim_db.inc';
    require_once 'classes/Port.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    Port::insert ($conn, $port, $protocol, $service, $descr);

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Port succesfully inserted"); ?> </p>
    <p><a href="port.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>
