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
        (!$_POST["hostname"] || !$_POST["ip"] || 
         !$_POST["asset"] || !$_POST["threshold_c"] || 
         !$_POST["threshold_a"] || !$_POST["descr"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $id          = mysql_escape_string($_POST["id"]);
    $hostname    = mysql_escape_string($_POST["hostname"]);
    $ip          = mysql_escape_string($_POST["ip"]);
    $asset    = mysql_escape_string($_POST["asset"]);
    $threshold_c = mysql_escape_string($_POST["threshold_c"]);
    $threshold_a = mysql_escape_string($_POST["threshold_a"]);
    $descr       = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    
    Host::update ($conn, $ip, $hostname, $asset, $threshold_c, 
                  $threshold_a, $descr);

    $db->close($conn);
}
?>
    <p>Host succesfully updated</p>
    <p><a href="host.php">Back</a></p>

</body>
</html>

