<?php

    require_once ("ossim_acl.inc");
    require_once ("ossim_conf.inc");

    $conf = new ossim_conf();
    $phpgacl = $conf->get_conf("phpgacl_path");

    require_once ("$phpgacl/gacl.class.php");


    function check_phpgacl_install()
    {
        require_once ("ossim_db.inc");

        $db = new ossim_db();
        $conn = $db->connect();
        $query = "SELECT * FROM acl";
        if (!$rs = &$conn->Execute($query)) {
            echo "
        <p align=\"center\"><b>You need to configure phpGACL</b><br/>
        Remember to setup the database connection at phpGACL config files!
        <br/>
        Click <a href=\"/phpgacl/setup.php\">here</a> to enter setup
        </p>
            ";
        exit;
            
        }
        $db->close($conn);
    }

    check_phpgacl_install();

    $gacl = new gacl();
    if (! $gacl->acl_check(ACL_DEFAULT_DOMAIN_SECTION,
                           ACL_DEFAULT_DOMAIN_ALL,
                           ACL_DEFAULT_USER_SECTION,
                           ACL_DEFAULT_OSSIM_ADMIN))
    {
        echo "
            <p align=\"center\"><b>You need to setup default acls</b>
            <br/>
            Click <a href=\"../setup/ossim_acl.php\">here</a> to enter setup
            </p>
        ";
        exit;
    }



    require_once ("classes/Session.inc");

    if ($_REQUEST["action"] == "logout") {
        Session::logout();
        header ("Location: ../index.php");
    }

    if ($_REQUEST["user"]) {

        $session = new Session($_REQUEST["user"], $_REQUEST["pass"], "");
        if ($session->login()) {
            if ($_REQUEST["dest"]) {
                header ("Location: " . $_REQUEST["dest"]);
                exit;
            } else {
                header ("Location: ../control_panel/global_score.php");
            }
        } else {
            $bad_pass = True;
        }
    }

?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework Login"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
<script>
if (location.href != top.location.href) top.location.href = location.href;
</script>
</head>
<body>

  <h1> <?php echo gettext("OSSIM Login"); ?> </h1>

<form method="POST" action="<?php $_SERVER["PHP_SELF"] ?>">
<table align="center">
  <input type="hidden" name="dest" value="<?php echo $_GET["dest"] ?>">
  <tr>
    <td> <?php echo gettext("User"); ?> </td>
    <td><input type="text" name="user" /></td>
  </tr>
  <tr>
    <td> <?php echo gettext("Password"); ?> </td>
    <td><input type="password" name="pass" /></td>
  </tr>
  <tr>
    <td colspan="2"><input type="submit" value="Login"></td>
  </tr>
</table>
</form>

<p><i> <?php echo gettext("NOTE: Default user is admin-admin"); ?> .<br/>
<?php echo gettext("For security reasons you should change it at Configuration->Users"); ?> </i></p>

<p>
  <?php

    
  
    if ($bad_pass)
        echo "<p><font color=\"red\">Wrong User & Password</font></p>";
  ?>
</p>

</body>
</html>

