<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<?php
    session_start();
    $loguser = $_SESSION["_user"];
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete user"); ?> </h1>

<?php
    if (!$_GET["user"]) {
        echo "<p align=\"center\">Wrong user</p>";
        exit;
    }

    $user = $_GET["user"];

if (!$_GET["confirm"]) {
?>
    <p> <?php echo gettext("Are you sure"); ?> </p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"].
        "?user=$user&confirm=yes"; ?>"> 
	<?php echo gettext("Yes"); ?> </a>
        &nbsp;&nbsp;&nbsp;<a href="host.php"> <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}


    if ($loguser == $user) {
        echo "<p align=\"center\">You can't remove yourself!</p>";
        echo "<p><a href=\"users.php\">Back</a></p>";
        exit;
    }

    require_once 'ossim_db.inc';
    require_once 'classes/Session.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Session::delete($conn, $user);
    $db->close($conn);

?>

    <p> <?php echo gettext("User deleted"); ?> </p>
    <p><a href="users.php"> <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

