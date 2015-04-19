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
* - check_perms()
* Classes list:
*/
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
require_once ('ossim_acl.inc');
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
require_once ("classes/Security.inc");
$user = GET('user');
$networks = GET('networks');
$sensors = GET('sensors');
$perms = GET('perms');
$copy_panels = GET('copy_panels');
ossim_valid($user, OSS_USER, OSS_NULLABLE, 'illegal:' . _("User name"));
ossim_valid($user, OSS_USER, 'illegal:' . _("User name"));
ossim_valid($networks, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("networks"));
ossim_valid($sensors, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("sensors"));
ossim_valid($perms, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("perms"));
if (ossim_error()) {
    die(ossim_error());
}
function check_perms($user, $mainmenu, $submenu) {
    $gacl = $GLOBALS['ACL'];
    return $gacl->acl_check($mainmenu, $submenu, ACL_DEFAULT_USER_SECTION, $user);
}
require_once ('classes/Session.inc');
require_once ('classes/Net.inc');
require_once ('classes/Sensor.inc');
require_once ('ossim_db.inc');
$db = new ossim_db();
$conn = $db->connect();
if ($user_list = Session::get_list($conn, "WHERE login = '$user'")) {
    $user = $user_list[0];
}
$net_list = Net::get_all($conn);
$sensor_list = Sensor::get_all($conn, "ORDER BY name ASC");
?>

<form method="post" action="modifyuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <input type="hidden" name="user" value="<?php
echo $user->get_login() ?>" />
  <tr>
    <th> <?php
echo gettext("User login"); ?> </th>
    <td class="left"><b><?php
echo $user->get_login(); ?></b></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("User name"); ?> </th>
    <td class="left"><input type="text" name="name"
        value="<?php
echo $user->get_name(); ?>" /></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("User email"); ?> <img src="../pixmaps/email_icon.gif"></th>
    <td class="left"><input type="text" name="email"
        value="<?php
echo $user->get_email(); ?>" /></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("User language"); ?></th>
    <td class="left">
<?php
$language = array(
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
$lform = "<select name=\"language\">";
foreach($language['type'] as $option_value => $option_text) {
    $lform.= "<option ";
    if ($user->get_language() == $option_value) $lform.= " SELECTED ";
    $lform.= "value=\"$option_value\">$option_text</option>";
}
$lform.= "</select>";
echo $lform;
?>
</td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Company"); ?> </th>
    <td class="left"><input type="text" name="company"
        value="<?php
echo $user->get_company(); ?>" /></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Department"); ?> </th>
    <td class="left"><input type="text" name="department"
        value="<?php
echo $user->get_department(); ?>" /></td>
  </tr>
<?php
if ($user->get_login() != 'admin') { ?>
<tr>
<th><?php echo _("Pre-set executive panels to admin panels") ?></th>
    <td align="center">
   <input type="radio" name="copy_panels" value="1" checked> <?php echo _("Yes"); ?>
   <input type="radio" name="copy_panels" value="0" > <?php echo _("No"); ?>
    </td>
</tr>
<?php
} else { ?>
   <input type="hidden"  name="copy_panels" value="1" checked>
<?php
} ?>
<tr>
    <td>&nbsp;</td>
    <td align="center">
      <input type="submit" value="OK">
      <input type="reset" value="<?php
echo gettext("reset"); ?>">
    </td>
</tr>
</table>
  <br/>
  <table align="center">
  <tr>
    <th> <?php
echo gettext("Allowed nets"); ?> </th>
    <th> <?php
echo gettext("Allowed sensors"); ?> </th>
    <th colspan="2"> <?php
echo gettext("Permissions"); ?> </th>
  </tr><tr>
    <td class="left" valign="top">
<?php
if ($networks) {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&networks=0" . "&sensors=" . $sensors . "&perms=" . $perms; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&networks=1" . "&sensors=" . $sensors . "&perms=" . $perms; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
}
?>
<?php
$i = 0;
foreach($net_list as $net) {
    $net_name = $net->get_name();
    $input = "<input type=\"checkbox\" name=\"net$i\" value=\"" . $net_name . "\"";
    if (false !== strpos(Session::allowedNets($user->get_login()) , $net->get_ips())) {
        $input.= " checked ";
    }
    if ($networks || ($user->get_login() == 'admin')) {
        $input.= " checked ";
    }
    if ($user->get_login() == 'admin') {
        $input.= "disabled";
    }
    $input.= "/>$net_name<br/>";
    echo $input;
    $i++;
}
?>
      <input type="hidden" name="nnets" value="<?php
echo $i ?>" />
      <i><?php
echo gettext("NOTE: No selection allows ALL") . " " . gettext("nets"); ?></i>
    </td>
    <td class="left" valign="top">
<?php
if ($sensors) {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&sensors=0" . "&networks=" . $networks . "&perms=" . $perms; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&sensors=1" . "&networks=" . $networks . "&perms=" . $perms; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
}
?>

<?php
$i = 0;
foreach($sensor_list as $sensor) {
    $sensor_name = $sensor->get_name();
    $sensor_ip = $sensor->get_ip();
    $input = "<input type=\"checkbox\" name=\"sensor$i\" value=\"" . $sensor_ip . "\"";
    if (false !== strpos(Session::allowedSensors($user->get_login()) , $sensor_ip)) {
        $input.= " checked ";
    }
    if ($sensors || ($user->get_login() == 'admin')) {
        $input.= " checked ";
    }
    if ($user->get_login() == 'admin') {
        $input.= "disabled";
    }
    $input.= "/>$sensor_name<br/>";
    echo $input;
    $i++;
}
?>
      <input type="hidden" name="nsensors" value="<?php
echo $i ?>" />
      <i><?php
echo gettext("NOTE: No selection allows ALL") . " " . gettext("sensors"); ?></i>
    </td>
    <td colspan="2" class="left" valign="top">
<?php
if ($perms) {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&perms=0" . "&networks=" . $networks . "&sensors=" . $sensors; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php
    echo $_SERVER["SCRIPT_NAME"] . "?user=" . $user->get_login() . "&perms=1" . "&networks=" . $networks . "&sensors=" . $sensors; ?>"><?php
    echo gettext("Select / Unselect all"); ?></a>
<hr noshade>
<?php
}
?>

<?php
foreach($ACL_MAIN_MENU as $mainmenu => $menus) {
    foreach($menus as $key => $menu) {
?>
            <input type="checkbox" name="<?php
        echo $key ?>"
<?php
        if ($user->get_login() == 'admin') echo " disabled";
        if (check_perms($user->get_login() , $mainmenu, $key)) echo " checked ";
        if ($perms || ($user->get_login() == 'admin')) echo " checked ";
?>>
<?php
        echo $menu["name"] . "<br/>\n";
    }
    echo "<hr noshade>";
}
?>
    </td>
  </tr>
</table>

<br/>
<table align="center">
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="<?php
echo gettext("reset"); ?>">
    </td>
  </tr>
</table>
</form>

</body>
</html>

