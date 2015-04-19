<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Change password"); ?> </h1>

<?php

    require_once ('ossim_db.inc');
    require_once ('classes/Session.inc');
    require_once ('ossim_acl.inc');

$user  = POST('user');
$pass1 = POST('pass1');
$pass2 = POST('pass2');
$oldpass = POST('oldpass');

ossim_valid($user, OSS_USER, 'illegal:'._("User name"));

if (ossim_error()) {
        die(ossim_error());
}


    $db = new ossim_db();
    $conn = $db->connect();



    /* check params */
    if (!POST("user") || !POST("pass1") || !POST("pass2"))
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("FORM_MISSING_FIELDS");
    }

    if (($_SESSION["_user"] != ACL_DEFAULT_OSSIM_ADMIN) && 
         (($_SESSION["_user"] != $user) && !POST("oldpass"))) 
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("FORM_MISSING_FIELDS");
    }

    /* check for old password if not actual user or admin */
    if ((($_SESSION["_user"] != $user) && 
          $_SESSION["_user"] != ACL_DEFAULT_OSSIM_ADMIN) && 
          !is_array($user_list = Session::get_list($conn,
                        "WHERE login = '" . $user . 
                        "' and pass = '" . md5($oldpass) . "'")))
    {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("BAD_OLD_PASSWORD");
    }

    /* check passwords */
    if (0 != strcmp($pass1, $pass2)) {
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("PASSWORDS_MISMATCH");
    }

    /* check OK, insert into DB */
    if (POST('update')) {

        Session::changepass ($conn, $user, $pass1);

?>
    <p> <?php echo gettext("User succesfully updated"); ?> </p>
    <p><a href="users.php"> <?php echo gettext("Back"); ?> </a></p>
<?php
    }
    $db->close($conn);
?>

</body>
</html>

