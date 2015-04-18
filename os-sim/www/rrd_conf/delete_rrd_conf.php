<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Framework</h1>
  <h2>Delete RRD Conf</h2>

<?php 
    if (!$_GET["ip"]) { 
?>
    <p>Wrong ip</p>
<?php 
        exit;
    }


$ip = mysql_escape_string($_GET["ip"]);

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a 
      href="<?php echo $_SERVER["PHP_SELF"]."?ip=$ip&confirm=yes"; ?>">Yes</a>
      &nbsp;&nbsp;&nbsp;<a href="rrd_conf.php">No</a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/RRD_conf.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    RRD_conf::delete($conn, $ip);
    $db->close($conn);

?>

    <p>RRD_conf deleted</p>
    <p><a href="rrd_conf.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

