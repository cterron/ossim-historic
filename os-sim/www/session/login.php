<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* - check_phpgacl_install()
* Classes list:
*/
require_once "ossim_acl.inc";
require_once "ossim_conf.inc";
require_once "ossim_db.inc";
$conf = $GLOBALS["CONF"];
$gacl = $GLOBALS['ACL'];
function check_phpgacl_install() {
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
    $query2 = OssimQuery("SELECT * FROM " . $db_table_prefix . "_acl");
    if ((!$conn->Execute($query1)) and (!$conn->Execute($query2))) {
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
if (!$gacl->acl_check(ACL_DEFAULT_DOMAIN_SECTION, ACL_DEFAULT_DOMAIN_ALL, ACL_DEFAULT_USER_SECTION, ACL_DEFAULT_OSSIM_ADMIN)) {
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
    $infolog = array(
        Session::get_session_user()
    );
    if (trim($infolog[0]) != "") Log_action::log(2, $infolog);
    Session::logout();
    header("Location: ../index.php");
}
$user = REQUEST('user');
$pass = REQUEST('pass');
$accepted = REQUEST('first_login');
ossim_valid($user, OSS_USER, OSS_NULLABLE, 'illegal:' . _("User name"));
ossim_valid($accepted, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("First login"));
if (ossim_error()) {
    die(ossim_error());
}
$failed = true;
$first_login = 0;
if (REQUEST('user')) {
    require_once ("classes/Config.inc");
    $session = new Session($user, $pass, "");
    $conf = new Config();
    if ($accepted == "1") {
        $conf->update("first_login", "0");
    }
    if ($session->login()) {
        $first_login = 1;
        $first_login = $conf->get_conf("first_login", FALSE);
        if ($first_login == "") {
            $first_login = 1;
        }
        if ($first_login == "0") {
            $accepted = 1;
        }
        $failed = false;
        if ($accepted) {
            $first_login = 0;
            $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB); //get vector size on ECB mode
            $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND); //Creating the vector
            $_SESSION["mdspw"] = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $conf->get_conf("md5_salt", FALSE) , $pass, MCRYPT_MODE_ECB, $iv);
            require_once 'classes/Log_action.inc';
            $infolog = array(
                REQUEST('user')
            );
            Log_action::log(1, $infolog);
            if (POST('maximized') == "1") {
?>
				<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
				<html xmlns="http://www.w3.org/1999/xhtml">
				<body><script>window.open("../index.php","full_main_window","fullscreen,scrollbars")</script></body>
				</html>
				<?php
            } else header("Location: ../index.php");
            exit;
        }
    } else {
        $failed = true;
        $bad_pass = true;
    }
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title> <?php
echo gettext("AlienVault - The Open Source SIM"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <link rel="Shortcut Icon" type="image/x-icon" href="../favicon.ico">
<script>
if (location.href != top.location.href) top.location.href = location.href;
</script>
</head>
<?php
if ($failed) { ?>
<body onLoad="javascript:document.f.user.focus();" bgcolor=#aaaaaa>

<?php
    require_once 'classes/About.inc';
    $about = new About();
?>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<br/>
<form name="f" method="POST" action="login.php" style="margin:1px">

<table align="center" style="padding:1px;background-color:#f2f2f2;border-color:#aaaaaa" class=nobborder><tr><td class="nobborder">
<table align="center" class="noborder" style="background-color:white">

  <tr> <td class="nobborder" style="text-align:center;padding:30px 20px 0px 20px">
       <? $version = $conf->get_conf("ossim_server_version", FALSE); ?>
       <img src="../pixmaps/ossim<?= (preg_match("/.*pro.*/i",$version)) ? "_siem" : ((preg_match("/.*demo.*/i",$version)) ? "_siemdemo" : "") ?>.png" alt="open source SIM logo" />
  </td> </tr>
 
  <tr>
    <td align="center" class="nobborder" style="text-align:center">
      <br/><br/><br/>
    </td>
  </tr>
   <tr>
    <td class="nobborder center">
	  <table align="center" cellspacing=4 cellpadding=2 style="background-color:#eeeeee;border-color:#dedede">
	  <tr>
	    <td style="text-align:right" class="nobborder"> <?php
    echo gettext("User"); ?> </td>
	    <td style="text-align:left" class="nobborder"><input type="text" name="user" /></td>
	  </tr>
	  <tr>
	    <td style="text-align:right" class="nobborder"> <?php
    echo gettext("Password"); ?> </td>
	    <td style="text-align:left" class="nobborder"><input type="password" name="pass" /></td>
	  </tr>
	  </table>
    </td>
  </tr>
  <tr>
    <td class="nobborder" style="text-align:center;height:30px;font-size:12px">

    <input type="checkbox" value="1" name="maximized" style="font-size:7px"> Maximized

    </td>
  </tr>
  <tr>
    <td class="nobborder" style="text-align:center;padding-top:20px">

    <input type="submit" value="<?php
    echo gettext("Login"); ?>" class="btn" style="font-size:12px">

    </td>
  </tr>
  <tr>
    <td class="nobborder" style="text-align:center">
    <br/>
    </td>
  </tr>
</table>

    </td>
  </tr>
</table>

</form>

  <?php
    if (isset($bad_pass)) echo "<p><font color=\"red\">" . gettext("Wrong User & Password") . "</font></p>";
?>

</body>

<?php
}
if ($first_login) { // first login
     ?>

<body bgcolor=#aaaaaa>

<form name="f" method="POST" action="login.php" style="margin:1px">
<input type="hidden" name="user" value="<?php echo $user
?>"/>
<input type="hidden" name="pass" value="<?php echo $pass
?>"/>
<input type="hidden" name="first_login" value="1"/>

<table align="center" style="padding:1px;background-color:#f2f2f2;border-color:#aaaaaa" class=nobborder><tr><td class="nobborder">
<table align="center" class="noborder" style="background-color:white">

  <tr> <td class="nobborder" style="text-align:center;padding:10px 20px 0px 20px">
       <img src="../pixmaps/ossim.png" alt="open source SIM logo" />
  </td> </tr>
 
  <tr>
    <td align="center" class="nobborder" style="padding-top:10px">
		<table height="400" width="740"><tr><td class="nobborder">
			<div style="text-align:left;padding:5px;height:400px;overflow-y:scroll">
			<?php
    if (file_exists("../../include/First_login.txt")) {
        require_once ("../../include/First_login.txt");
    }
?>
			</div>
		</td></tr>
		</table>
    </td>
  </tr>
  <tr>
    <td class="nobborder" style="text-align:center;padding-top:20px">
	
	<input type="submit" value="<?php
    echo gettext("Accept"); ?>" class="btn" style="font-size:12px"> &nbsp;&nbsp;&nbsp;
	<input type="button" onclick="document.location.href='login.php'" value="<?php
    echo gettext("Logout"); ?>" class="btn" style="font-size:12px">
	
    </td>
  </tr>
  <tr>
    <td class="nobborder" style="text-align:center">
    <br/>
    </td>
  </tr>
</table>

    </td>
  </tr>
</table>

</form>
</body>

<?php
} ?>
</html>

