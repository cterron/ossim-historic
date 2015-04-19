<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPolicy");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete policy"); ?> </h1>

<?php 
    require_once 'classes/Security.inc';
   
    $id = GET('id');
    ossim_valid($id, OSS_ALPHA, 'illegal:'._("Policy id"));

    if (ossim_error()) {
            die(ossim_error());
    }

if (!GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?> ?</p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"]."?id=$id&confirm=yes"; ?>">
    <?php echo gettext("Yes"); ?> </a>&nbsp;&nbsp;&nbsp;<a href="policy.php">
    <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Policy.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Policy::delete($conn, $id);
    $db->close($conn);

?>

    <p> <?php echo gettext("Policy deleted"); ?> </p>
    <p><a href="policy.php">
    <?php echo gettext("Back"); ?> </a></p>
<?php
// update indicators on top frame
$OssimWebIndicator->update_display();
?>

</body>
</html>

