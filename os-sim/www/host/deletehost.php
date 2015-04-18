<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Delete host</h1>

<?php
    if (!$_GET["ip"]) {
?>
    <p>Wrong ip</p>
<?php
        exit;
    }


$ip = $_GET["ip"];

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"]."?ip=$ip&confirm=yes"; ?>">Yes</a>
      &nbsp;&nbsp;&nbsp;<a href="host.php">No</a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Host::delete($conn, $ip);
    Host_scan::delete($conn, $ip, 3001);
    $db->close($conn);

?>

    <p>Host deleted</p>
    <p><a href="host.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

