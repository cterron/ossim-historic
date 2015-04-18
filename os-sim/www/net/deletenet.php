<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyNetworks");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Delete net</h1>

<?php 
    if (!$_GET["name"]) { 
?>
    <p>Wrong name</p>
<?php 
        exit;
    }


$name = mysql_escape_string($_GET["name"]);

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a 
      href="<?php echo $_SERVER["PHP_SELF"]."?name=$name&confirm=yes"; ?>">Yes</a>
      &nbsp;&nbsp;&nbsp;<a href="net.php">No</a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    require_once 'classes/Net_scan.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    Net::delete($conn, $name);
    Net_scan::delete($conn, $name, 3001);
    $db->close($conn);

?>

    <p>Net deleted</p>
    <p><a href="net.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

