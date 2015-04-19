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

  <h1> <?php echo gettext("Delete host group"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$name = GET('name');

ossim_valid($name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("name"));

if (ossim_error()) {
       die(ossim_error());
}

if (!GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?> ?</p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"]."?name=$name&confirm=yes"; ?>">
      <?php echo gettext("Yes"); ?> </a>
      &nbsp;&nbsp;&nbsp;<a href="hostgroup.php">
      <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Host_group.inc';
    require_once 'classes/Host_group_scan.inc'; 

    $db = new ossim_db();
    $conn = $db->connect();
    Host_group::delete($conn, $name);
    Host_group_scan::delete($conn, $name, 3001);
    $db->close($conn);

?>

    <p> <?php echo gettext("Host group deleted"); ?> </p>
    <p><a href="hostgroup.php">
    <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

