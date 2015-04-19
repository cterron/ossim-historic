<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySignatures");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete signature group"); ?> </h1>

<?php 
require_once ("classes/Security.inc");

$sig_name = GET('signame');

ossim_valid($sig_name, OSS_PUNC, OSS_ALPHA, 'illegal:'._("Signature name"));

if (ossim_error()) {
    die(ossim_error());
}

if (GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?>? </p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"]."?signame=$sig_name&confirm=yes"; ?>"> 
    <?php echo gettext("Yes"); ?> </a>&nbsp;&nbsp;&nbsp;<a href="signature.php"> 
    <?php echo gettext("No"); ?></a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Signature_group::delete($conn, $sig_name);
    $db->close($conn);

?>

    <p> <?php echo gettext("Signature group deleted"); ?> </p>
    <p><a href="signature.php"> <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

