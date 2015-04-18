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
        (!$_POST["name"] || !$_POST["ips"] || !$_POST["priority"] ||
         !$_POST["threshold_c"] || !$_POST["threshold_a"] || 
         !$_POST["persistence"] || !$_POST["descr"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $name        = mysql_escape_string($_POST["name"]);
    $ips          = mysql_escape_string($_POST["ips"]);
    $priority    = mysql_escape_string($_POST["priority"]);
    $threshold_c = mysql_escape_string($_POST["threshold_c"]);
    $threshold_a = mysql_escape_string($_POST["threshold_a"]);
    $alert       = mysql_escape_string($_POST["alert"]);
    $persistence = mysql_escape_string($_POST["persistence"]);
    $descr       = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    $db = new ossim_db();
    $conn = $db->connect();
   
    Net::insert ($conn, $name, $ips, $priority, $threshold_c, 
                 $threshold_a, $alert, $persistence, $descr);

    $db->close($conn);
}
?>
    <p>Network succesfully inserted</p>
    <p><a href="net.php">Back</a></p>

</body>
</html>

