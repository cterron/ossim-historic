<?php

    require_once ("ossim_acl.inc");
    require_once ("ossim_conf.inc");

    $conf = new ossim_conf();
    $phpgacl = $conf->get_conf("phpgacl_path");

    require_once ("$phpgacl/gacl.class.php");
    
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
  <title>OSSIM Framework Login</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Login</h1>

<form method="POST" action="<?php $_SERVER["PHP_SELF"] ?>">
<table align="center">
  <input type="hidden" name="dest" value="<?php echo $_GET["dest"] ?>">
  <tr>
    <th>User</th>
    <td><input type="text" name="user" /></td>
  </tr>
  <tr>
    <th>Password</th>
    <td><input type="password" name="pass" /></td>
  </tr>
  <tr>
    <td colspan="2"><input type="submit" value="Login"></td>
  </tr>
</table>
</form>

<p><i>NOTE: Default user is admin-admin.<br/>
For security reasons you should change it at Configuration->Users</i></p>

<p>
  <?php

    
  
    if ($bad_pass)
        echo "<p><font color=\"red\">Wrong User & Password</font></p>";
  ?>
</p>

</body>
</html>

