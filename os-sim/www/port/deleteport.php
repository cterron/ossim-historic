<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete port group"); ?> </h1>

<?php 

require_once 'classes/Security.inc';

$port_name = GET('portname');

ossim_valid($port_name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("Port group name"));

if (ossim_error()) {
   die(ossim_error());
}


if (!GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?> ?</p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"]."?portname=$port_name&confirm=yes"; ?>">
    <?php echo gettext("Yes"); ?> </a>&nbsp;&nbsp;&nbsp;<a href="port.php">
    <?php echo gettext("No"); ?> </a>
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

    <p> <?php echo gettext("Port group deleted"); ?> </p>
    <p><a href="port.php">
    <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>
