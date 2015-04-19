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

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["port"] || !$_POST["protocol"] || 
        !$_POST["service"] || !$_POST["descr"]))
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("FORM_MISSING_FIELDS");
} elseif($_POST["insert"]) {

    $port     = validateVar($_POST["port"], OSS_DIGIT);
    $protocol = validateVar($_POST["protocol"]);
    $service  = validateVar($_POST["service"]);
    $descr    = validateVar($_POST["descr"], OSS_ALPHA . OSS_PUNC . OSS_SCORE);

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

