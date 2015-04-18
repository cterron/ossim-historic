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
        (!$_POST["hostname"] || !$_POST["ip"] || !$_POST["asset"] ||
         !$_POST["threshold_c"] || !$_POST["threshold_a"] || 
         !$_POST["descr"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $hostname    = $_POST["hostname"];
    $ip          = $_POST["ip"];
    $asset    = $_POST["asset"];
    $threshold_c = $_POST["threshold_c"];
    $threshold_a = $_POST["threshold_a"];
    $descr       = $_POST["descr"];

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    $db = new ossim_db();
    $conn = $db->connect();
   
    Host::insert ($conn, $ip, $hostname, $asset, $threshold_c, 
                  $threshold_a, $descr);

    $db->close($conn);
}
?>
    <p>Host succesfully inserted</p>
    <p><a href="host.php">Back</a></p>

</body>
</html>

