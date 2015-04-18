<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify Port group</h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["name"] || !$_POST["nports"] || !$_POST["descr"]))
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $name  = mysql_escape_string($_POST["name"]);
    $nports = mysql_escape_string($_POST["nports"]);
    $descr = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    for ($i = 1; $i <= $_POST["nports"]; $i++) {
        $mboxname = "mbox" . $i;
        if ($_POST[$mboxname]) {
            $port_list[] = mysql_escape_string($_POST[$mboxname]);
        }
    }
   
    Port_group::update ($conn, $name, $port_list, $descr);

    $db->close($conn);
}
?>
    <p>Port succesfully updated</p>
    <p><a href="port.php">Back</a></p>

</body>
</html>

