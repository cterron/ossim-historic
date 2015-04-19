<?php

    require_once "ossim_acl.inc";
    require_once "ossim_conf.inc";
    require_once "ossim_db.inc";
    
    $conf = $GLOBALS["CONF"];
    $gacl = $GLOBALS['ACL'];
    
    function check_phpgacl_install()
    {
        global $gacl; 
        $db_table_prefix = $gacl->_db_table_prefix;
    
        require_once "ossim_db.inc";

        $db = new ossim_db();
        if (!$conn = $db->phpgacl_connect()) {
            echo "<p align=\"center\">
                <b>Can't connect to OSSIM acl database (phpgacl)</b><br/>
                Check for phpgacl values at framework configuration
                </p>";
            exit;
        }
        
        $query1 = OssimQuery("SELECT * FROM acl");
        $query2 = OssimQuery("SELECT * FROM ".$db_table_prefix."_acl");

        if ( (! $conn->Execute($query1)) and 
             (! $conn->Execute($query2)) )
        {
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



    require_once 'classes/Session.inc';
    require_once 'classes/Security.inc';

    $action = REQUEST('action');

    if ($action == "logout") {
        require_once 'classes/Log_action.inc';
        $infolog = array(Session::get_session_user());
        Log_action::log(2, $infolog);
        Session::logout();
        header ("Location: ../index.php");
    }

    if (REQUEST('user')) {

        
        $user = REQUEST('user');
        $pass = REQUEST('pass');
        $dest = REQUEST('dest');

        ossim_valid($user, OSS_USER, OSS_NULLABLE , 'illegal:'._("User name"));
        ossim_valid($dest, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Destination"));

        if (ossim_error()) {
            die(ossim_error());
        }
       
        $session = new Session($user, $pass, "");
        if ($session->login()) {
            require_once 'classes/Log_action.inc';
            $infolog = array(REQUEST('user'));
            Log_action::log(1, $infolog);
 
        if (REQUEST('dest'))
            {
                if (preg_match("/top\.php$/", $dest)) {
                    header ("Location: ../index.php");
                    exit;
                } else {
                    header ("Location: " . $dest);
                    exit;
                }

            } else {
                header ("Location: ../control_panel/global_score.php");
            }
        } else {
            $bad_pass = true;
        }
    }

?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework Login"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
<script>
if (location.href != top.location.href) top.location.href = location.href;
</script>
</head>
<body onLoad="javascript: document.f.user.focus();">

  <h1> <?php echo gettext("OSSIM Login"); ?> </h1>

<?php
    require_once 'classes/About.inc';
    $about = new About();
?>

<form name="f" method="POST" action="<?php $_SERVER["PHP_SELF"] ?>">
<table align="center">
  <tr>
    <td>
      <table align="center">
        <tr>
          <td>
            <?php $logo_src = $about->get_logo(); ?>
            <img src="<?php echo $logo_src ?>" width="280" alt="OSSIM logo" />
          </td>
        </tr>
      </table>
    </td>
    <td>
<table align="center" class="noborder">
  <input type="hidden" name="dest" value="<?php echo REQUEST('dest') ?>">
  <tr>
    <td colspan="2">
      <b>OSSIM (Open Source Security Information Management)</b><br/>
<?php 
        echo gettext("Version") . ": " . $about->get_version();
        echo " (" . $about->get_date() . ")";
?>
      <br/><br/><br/>
    </td>
  </tr>
  <tr>
    <td> <?php echo gettext("User"); ?> </td>
    <td><input type="text" name="user" /></td>
  </tr>
  <tr>
    <td> <?php echo gettext("Password"); ?> </td>
    <td><input type="password" name="pass" /></td>
  </tr>
  <tr>
    <td colspan="2"><input type="submit" value="<?php echo gettext("Login"); ?>"></td>
  </tr>
  <tr><td colspan="2"></td></tr>
  <tr>
    <td colspan="2">
    <br/><br/>
    <i><?php echo gettext("NOTE: Default user is admin-admin"); ?> .<br/>
    <?php echo gettext("For security reasons you should change it at Configuration->Users"); ?></i>
    </td>
  </tr>
</table>
    </td>
  </tr>
</table>
</form>

<p>
  <?php

    if (isset($bad_pass))
        echo "<p><font color=\"red\">".gettext("Wrong User & Password")."</font></p>";
  ?>
</p>

</body>
</html>

