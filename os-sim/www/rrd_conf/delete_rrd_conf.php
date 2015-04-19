<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationRRDConfig");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete RRD Conf"); ?> </h1>

<?php 

if (!$_GET["profile"]) { 
    echo "<p align=\"center\">Wrong profile</p>";
    exit;
}

$profile = mysql_escape_string($_GET["profile"]);

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"].
        "?profile=$profile&confirm=yes"; ?>">
	<?php echo gettext("Yes"); ?> </a>
      &nbsp;&nbsp;&nbsp;<a href="rrd_conf.php">
      <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/RRD_config.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    RRD_config::delete($conn, $profile);
    $db->close($conn);

?>

    <p> <?php echo gettext("RRD profile deleted"); ?> </p>
    <p><a href="rrd_conf.php"> <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

