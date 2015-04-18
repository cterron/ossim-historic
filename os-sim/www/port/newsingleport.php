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

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["port"] || !$_POST["protocol"] || 
        !$_POST["service"] || !$_POST["descr"]))
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $port     = mysql_escape_string($_POST["port"]);
    $protocol = mysql_escape_string($_POST["protocol"]);
    $service  = mysql_escape_string($_POST["service"]);
    $descr    = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Port.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    Port::insert ($conn, $port, $protocol, $service, $descr);

    $db->close($conn);
}
?>
    <p>Port succesfully inserted</p>
    <p><a href="port.php">Back</a></p>

</body>
</html>

