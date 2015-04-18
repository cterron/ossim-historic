<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Framework</h1>
  <h2>Delete sensor</h2>

<?php 
    if (!$_GET["name"]) { 
?>
    <p>Wrong sensor</p>
<?php 
        exit;
    }


$name = $_GET["name"];

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a 
      href="<?php echo $_SERVER["PHP_SELF"]."?name=$name&confirm=yes"; ?>">Yes</a>
      &nbsp;&nbsp;&nbsp;<a href="sensor.php">No</a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Sensor::delete($conn, $name);
    $db->close($conn);

?>

    <p>Sensor deleted</p>
    <p><a href="sensor.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

