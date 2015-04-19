<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete host"); ?> </h1>

<?php
    require_once ('classes/Security.inc');

    $ip = GET('ip');
    $confirm = GET('confirm');    
    
    ossim_valid($ip, OSS_IP_ADDR , 'illegal:'._("ip"));
    ossim_valid($confirm, OSS_ALPHA, OSS_NULLABLE , 'illegal:'._("confirm"));
    
    if (ossim_error()) {
        die(ossim_error());
    }
                            


    
if (!empty($confirm)) {
?>
    <p> <?php echo gettext("Are you sure"); ?> ?</p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"]."?ip=$ip&confirm=yes"; ?>">
      <?php echo gettext("Yes"); ?> </a>
      &nbsp;&nbsp;&nbsp;<a href="host.php">
      <?php echo gettext("No"); ?> </a>
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
    Host_scan::delete($conn, $ip, 2007);
    $db->close($conn);

?>

    <p> <?php echo gettext("Host deleted"); ?> </p>
    <p><a href="host.php">
    <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

