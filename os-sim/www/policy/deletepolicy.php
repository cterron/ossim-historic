<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPolicy");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Delete policy</h1>

<?php 
    if (!$id = mysql_escape_string($_GET["id"])) { 
?>
    <p>Wrong policy id</p>
<?php 
        exit;
    }

if (!$_GET["confirm"]) {
?>
    <p>Are you sure?</p>
    <p><a href="<?php echo $_SERVER["PHP_SELF"]."?id=$id&confirm=yes"; ?>">Yes</a>&nbsp;&nbsp;&nbsp;<a href="policy.php">No</a>
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

    <p>Policy deleted</p>
    <p><a href="policy.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>

