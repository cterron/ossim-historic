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
* Classes list:
*/
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

	<?php
include ("../hmenu.php"); ?>

<?php
require_once ('ossim_db.inc');
require_once ('classes/Session.inc');
require_once ('ossim_acl.inc');
require_once ('classes/Security.inc');
$order = GET('order');
ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("order"));
if (ossim_error()) {
    die(ossim_error());
}
if (empty($order)) $order = "login";
?>

  <table align="center">
    <tr>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("login", $order);
?>">
	  <?php
echo gettext("Login"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("name", $order);
?>"> 
	  <?php
echo gettext("Name"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("email", $order);
?>">
	  <?php
echo gettext("Email"); ?> </a></th>
      <th> <?php
echo gettext("Password"); ?> </th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("company", $order);
?>">
      <?php
echo gettext("Company"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("department", $order);
?>">
	  <?php
echo gettext("Department"); ?> </a></th>
      <th> <?php
echo gettext("Actions"); ?> </th>
	 <th> <?php
echo gettext("Language"); ?> </th>
    </tr>

<?php
$db = new ossim_db();
$conn = $db->connect();
if (isset($_POST['user_id'])) {
    $_SESSION['_user_language'] = $_POST['language'];
    Session::changelang($conn, $_POST['user_id'], $_POST['language']);
}
if ($session_list = Session::get_list($conn, "ORDER BY $order")) {
    foreach($session_list as $session) {
        $login = $session->get_login();
        $name = $session->get_name();
        $email = $session->get_email();
        $pass = "...";
        $company = $session->get_company();
        $department = $session->get_department();
        $language = $session->get_language();
?>
    <tr>
      <td><?php
        echo $login; ?></td>
      <td><?php
        echo $name; ?>&nbsp;</td>
      <td><?php
        echo $email;
        if ($email) { ?>
            <a href="mailto:<?php echo $email
?>">
                <img border="0" src="../pixmaps/email_icon.gif"></a>
      <?php
        } ?>
      &nbsp;
      </td>
      <td><?php
        echo $pass; ?></td>
      <td><?php
        echo $company; ?>&nbsp;</td>
      <td><?php
        echo $department; ?>&nbsp;</td>
       <td>
      [<a href="changepassform.php?user=<?php
        echo $login ?>">
      <?php
        echo gettext("Change Password"); ?> </a>]
<?php
        if (Session::am_i_admin()) {
            if ($login != ACL_DEFAULT_OSSIM_ADMIN) {
?>
      [<a href="modifyuserform.php?user=<?php
                echo $login ?>"> 
      <?php
                echo gettext("Update"); ?> </a>]
      [<a href="deleteuser.php?user=<?php
                echo $login ?>"> 
      <?php
                echo gettext("Delete"); ?> </a>]
<?php
            } elseif ($login == ACL_DEFAULT_OSSIM_ADMIN) {
?>
      [<a href="modifyuserform.php?user=<?php
                echo $login ?>"> 
      <?php
                echo gettext("Update"); ?> </a>]
<?php
            }
        }
        if ($login == $_SESSION['_user'] || $_SESSION['_user'] == "admin") {
            echo "</td>
     <form name=\"langform_" . $login . "\" action=\"users.php\" method=\"post\">
	<td>";
            $languages = array(
                "type" => array(
                    "de_DE" => gettext("German") ,
                    "en_GB" => gettext("English") ,
                    "es_ES" => gettext("Spanish") ,
                    "fr_FR" => gettext("French") ,
                    "ja_JP" => gettext("Japanese") ,
                    "pt_BR" => gettext("Brazilian Portuguese") ,
                    "zh_CN" => gettext("Simplified Chinese") ,
                    "zh_TW" => gettext("Traditional Chinese") ,
                    "ru_RU.UTF-8" => gettext("Russian")
                ) ,
                "help" => gettext("")
            );
            $lform = "<select name=\"language\" onChange='document.langform_" . $login . ".submit()'>";
            foreach($languages['type'] as $option_value => $option_text) {
                $lform.= "<option ";
                if ($language == $option_value) $lform.= " SELECTED ";
                $lform.= "value=\"$option_value\">$option_text</option>";
            }
            $lform.= "</select>";
            $lform.= "<input type='hidden' name='user_id' value='" . $login . "'>";
            echo $lform . "</td></form>";
        } else {
            echo "</td><td>&nbsp; </td>";
        }
?>
    </tr>

<?php
    }
}
if (Session::am_i_admin()) {
?>
    <tr>
      <td colspan="8"><a href="newuserform.php"> <?php
    echo gettext("Insert new user"); ?> </a></td>
    </tr>
    <tr>
      <td colspan="8"><a href="../setup/ossim_acl.php"> <?php
    echo gettext("Reload ACLS"); ?> </a></td>
    </tr>
<?php
}
?>
  </table>

<?php
$db->close($conn);
?>

</body>
</html>

