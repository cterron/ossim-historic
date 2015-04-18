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

  <h1>Delete port group</h1>

<?php 
    if (!$port_name = mysql_escape_string($_GET["portname"])) { 
?>
    <p>Wrong port name</p>
<?php 
        exit;
    }

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"]."?portname=$port_name&confirm=yes"; ?>">Yes</a>&nbsp;&nbsp;&nbsp;<a href="port.php">No</a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Port_group::delete($conn, $port_name);
    $db->close($conn);

?>

    <p>Port group deleted</p>
    <p><a href="port.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

