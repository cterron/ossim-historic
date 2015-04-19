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

    $db = new ossim_db();
    $conn = $db->connect();


    /* check params */
    if (!$_POST["user"] || !$_POST["oldpass"] ||
        !$_POST["pass1"] || !$_POST["pass2"])
    {
        echo "<p align=\"center\">Please, complete all the fields</p>";
        exit();
    }

    /* check for old password */
    if (!$user_list = Session::get_list($conn,
        "WHERE login = '" . $_POST["user"] . "' and pass = '" . md5($_POST["oldpass"]) . "'"))
    {
        echo "<p align=\"center\">Authentication failure</p>";
        exit();
    }

    /* check passwords */
    if (0 != strcmp($_POST["pass1"], $_POST["pass2"])) {
        echo "<p align=\"center\">Password mismatch</p>";
        exit();
    }

    /* check OK, insert into DB */
    if ($_POST["update"]) {

        Session::changepass ($conn, $_POST["user"], $_POST["pass1"]);

?>
    <p> <?php echo gettext("User succesfully updated"); ?> </p>
    <p><a href="users.php"> <?php echo gettext("Back"); ?> </a></p>
<?php
    }
    $db->close($conn);
?>

</body>
</html>

