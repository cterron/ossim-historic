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
        (!$_POST["name"] || !$_POST["ip"] || 
         !$_POST["descr"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $name        = mysql_escape_string($_POST["name"]);
    $ip          = mysql_escape_string($_POST["ip"]);
    $descr       = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    
    Sensor::update ($conn, $name, $ip, $descr);

    $db->close($conn);
}
?>
    <p>Sensor succesfully updated</p>
    <p><a href="sensor.php">Back</a></p>

</body>
</html>

