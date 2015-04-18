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
         !$_POST["threshold_c"] || !$_POST["threshold_a"] || 
         // !$_POST["persistence"] || 
         !$_POST["nsens"] || !$_POST["descr"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $hostname    = mysql_escape_string($_POST["hostname"]);
    $ip          = mysql_escape_string($_POST["ip"]);
    $asset       = mysql_escape_string($_POST["asset"]);
    $threshold_c = mysql_escape_string($_POST["threshold_c"]);
    $threshold_a = mysql_escape_string($_POST["threshold_a"]);
    $alert       = mysql_escape_string($_POST["alert"]);
    $persistence = mysql_escape_string($_POST["persistence"]);
    $nat         = mysql_escape_string($_POST["nat"]);
    $descr       = mysql_escape_string($_POST["descr"]);

    for ($i = 1; $i <= mysql_escape_string($_POST["nsens"]); $i++) {
        $name = "mboxs" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $sensors[] = mysql_escape_string($_POST[$name]);
        }
    }

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    Host::insert ($conn, $ip, $hostname, $asset, $threshold_c, 
                  $threshold_a, $alert, $persistence, $nat, $sensors, $descr);

    $db->close($conn);
}
?>
    <p>Host succesfully inserted</p>
    <p><a href="host.php">Back</a></p>

</body>
</html>

