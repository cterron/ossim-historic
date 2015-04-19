<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
$loguser = Session::get_session_user();
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
    require_once ("classes/Security.inc");

    $user = GET('user');
    
    ossim_valid($user, OSS_USER, 'illegal:'._("User name"));
    
    if (ossim_error()) {
            die(ossim_error());
    }


    if ( !Session::am_i_admin() )
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("ONLY_ADMIN");
    }


if (!GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?> </p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"].
        "?user=$user&confirm=yes"; ?>"> 
	<?php echo gettext("Yes"); ?> </a>
        &nbsp;&nbsp;&nbsp;<a href="users.php"> <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}


    if ($loguser == $user) {
         require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("USER_CANT_REMOVE");
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

