<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyNetworks");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete network group"); ?> </h1>

<?php 
    if (!$_GET["name"]) { 
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("WRONG_NET");
    }

$name = validateVar($_GET["name"], OSS_ALPHA . OSS_SCORE . OSS_PUNC);

if (!$_GET["confirm"]) {
?>
    <p> <?php echo gettext("Are you sure"); ?> ?</p>
    <p><a 
      href="<?php echo $_SERVER["PHP_SELF"]."?name=$name&confirm=yes"; ?>">
      <?php echo gettext("Yes"); ?> </a>
      &nbsp;&nbsp;&nbsp;<a href="net.php">
      <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Net_group.inc';
    require_once 'classes/Net_group_scan.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    Net_group::delete($conn, $name);
    Net_group_scan::delete($conn, $name, 3001);
    $db->close($conn);

?>

    <p> <?php echo gettext("Network group deleted"); ?> </p>
    <p><a href="netgroup.php">
    <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

