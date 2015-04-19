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
                                                                                
  <h1> <?php echo gettext("Modify Port group"); ?> </h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["name"] || !$_POST["nports"] || !$_POST["descr"]))
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("FORM_MISSING_FIELDS");
/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $name  = validateVar($_POST["name"], OSS_ALPHA . OSS_SCORE . OSS_DOT);
    $nports = validateVar($_POST["nports"]);
    $descr = validateVar($_POST["descr"], OSS_ALPHA . OSS_SCORE . OSS_AT . OSS_PUNC);

    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    for ($i = 1; $i <= $nports; $i++) {
        $mboxname = "mbox" . $i;
        if ($_POST[$mboxname]) {
            $port_list[] = validateVar($_POST[$mboxname]);
        }
    }
   
    Port_group::update ($conn, $name, $port_list, $descr);

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Port succesfully updated"); ?> </p>
    <p><a href="port.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

