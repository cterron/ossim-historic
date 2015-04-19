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
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<?php
require_once ('ossim_acl.inc');
require_once ('ossim_db.inc');
require_once ('classes/Net.inc');
require_once ('classes/Sensor.inc');
$db = new ossim_db();
$conn = $db->connect();
$net_list = Net::get_all($conn);
$sensor_list = Sensor::get_all($conn, "ORDER BY name ASC");
?>

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
$user = GET('user');
$pass1 = GET('pass1');
$pass2 = GET('pass2');
$name = GET('name');
$email = GET('email');
$company = GET('company');
$department = GET('department');
$networks = GET('networks');
$sensors = GET('sensors');
$perms = GET('perms');
$copy_panels = GET('copy_panels');
ossim_valid($user, OSS_USER, OSS_NULLABLE, 'illegal:' . _("User name"));
ossim_valid($copy_panels, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("Copy panels"));
ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_SPACE, OSS_NULLABLE, 'illegal:' . _("Name"));
ossim_valid($email, OSS_MAIL_ADDR, OSS_NULLABLE, 'illegal:' . _("e-mail"));
ossim_valid($company, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE, 'illegal:' . _("Company"));
ossim_valid($department, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE, 'illegal:' . _("Department"));
if (ossim_error()) {
    die(ossim_error());
}
$all = $defaults = array();
?>

<form method="post" action="newuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <tr>
    <th> <?php echo _("User login") . required() ?></th>
    <td class="left">
        <input type="text" id="1" name="user" value="<?php echo $user ?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?php echo _("User full name") . required() ?> </th>
    <td class="left">
        <input type="text" id="2" name="name" value="<?php echo $name ?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?php echo _("User Email") . required() ?> <img src="../pixmaps/email_icon.gif"></th>
    <td class="left">
        <input type="text" id="3" name="email" value="<?php echo $email ?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?php echo _("Enter password") . required() ?> </th>
    <td class="left">
        <input type="password" id="4" name="pass1" value="<?php echo $pass1 ?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?php echo _("Re-enter password") . required() ?> </th>
    <td class="left">
        <input type="password" id="5" name="pass2" value="<?php echo $pass2 ?>" size="30" />
    </td>
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
    $lform.= "value=\"$option_value\">$option_text</option>";
}
$lform.= "</select>";
echo $lform;
?>
</td>
  </tr>
  <tr>
    <th> <?php echo _("Company") ?> </th>
    <td class="left">
        <input type="text" id="6" name="company" value="<?php echo $company ?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?php echo _("Department") ?> </th>
    <td class="left">
        <input type="text" id="7" name="department" value="<?php echo $department ?>" size="30" />
    </td>
  </tr>
<tr>
<th><?php echo _("Pre-set executive panels to admin panels") ?></th>
    <td align="center">
   <input type="radio" name="copy_panels" value="1" checked> <?php echo _("Yes"); ?>
   <input type="radio" name="copy_panels" value="0" > <?php echo _("No"); ?> 
    </td>
</tr>
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
    <th><?php echo _("Allowed nets") ?></th>
    <th><?php echo _("Allowed sensors") ?></th>
    <th colspan="2"> <?php echo _("Permissions") ?> </th>
</tr><tr>
    <td class="left" valign="top">


<a href="#" onClick="return selectAll('nets');"><?php echo _("Select / Unselect all") ?></a>
<hr noshade>

<?php
$i = 0;
foreach($net_list as $net) {
    $all['nets'][] = "net" . $i;
?>
        <input type="checkbox" id="<?php echo "net" . $i ?>" name="<?php echo "net" . $i ?>"
               value="<?php echo $net->get_name(); ?>" /><?php echo $net->get_name() ?><br/>
<?php
    $i++;
}
?>
        <input type="hidden" name="nnets" value="<?php
echo $i ?>" />
        <i><?php
echo gettext("NOTE: No selection allows ALL") . " " . gettext("nets"); ?></i>
    </td>
    <td class="left" valign="top">

<a href="#" onClick="return selectAll('sensors');"><?php echo _("Select / Unselect all"); ?></a>
<hr noshade>

<?php
$i = 0;
foreach($sensor_list as $sensor) {
    $all['sensors'][] = "sensor" . $i;
?>
        <input type="checkbox" id="<?php echo "sensor" . $i ?>" name="<?php echo "sensor" . $i ?>"
               value="<?php echo $sensor->get_ip() ?>" /><?php echo $sensor->get_name() ?><br/> 
<?php
    $i++;
}
?>
        <input type="hidden" name="nsensors" value="<?php
echo $i ?>" />
        <i><?php
echo gettext("NOTE: No selection allows ALL") . " " . gettext("sensors"); ?></i>
    </td>
    <td colspan="2" class="left" valign="top">

<a href="#" onClick="return selectAll('perms');"><?php echo _("Select / Unselect all"); ?></a>
&nbsp;-&nbsp;<a href="#" onClick="return selectAll('perms', true);"><?php echo _("Back to Defaults"); ?></a>
<hr noshade>

<?php
foreach($ACL_MAIN_MENU as $menus) {
    foreach($menus as $key => $menu) {
        $all['perms'][] = $key;
?>
            <input type="checkbox" id="<?php echo $key
?>" name="<?php echo $key
?>"
            <?php
        if ($menu["default_perm"]) {
            echo " checked ";
            $defaults['perms'][$key] = true;
        } else {
            $defaults['perms'][$key] = false;
        }
?>
            ><?php echo $menu["name"] ?><br/>
<?php
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
    <td colspan="2" align="center" valign="top">
      <input type="submit" value="OK">
      <input type="reset" value="<?php
echo gettext("reset"); ?>">
    </td>
  </tr>
</table>
</form>
<script>

var check_nets    = true; // if true next click on "Select/Unselect" puts all to checked
var check_sensors = true;
var check_perms   = true;

function selectAll(category, defaults)
{
    if (category == 'perms' && !defaults) {
    <?php
foreach($all['perms'] as $id) { ?>
        document.getElementById('<?php echo $id
?>').checked = check_perms;
    <?php
} ?>
        check_perms = check_perms == false ? true : false;
    }
    if (category == 'perms' && defaults) {
    <?php
foreach($defaults['perms'] as $id => $check) { ?>
        document.getElementById('<?php echo $id
?>').checked = <?php echo $check ? 'true' : 'false' ?>;
    <?php
} ?>
    }
    if (category == 'sensors') {
    <?php
foreach($all['sensors'] as $id) { ?>
        document.getElementById('<?php echo $id
?>').checked = check_sensors;
    <?php
} ?>
        check_sensors = check_sensors == false ? true : false;
    }
    if (category == 'nets') {
    <?php
foreach($all['nets'] as $id) { ?>
        document.getElementById('<?php echo $id
?>').checked = check_nets;
    <?php
} ?>
        check_nets = check_nets == false ? true : false;
    }    
    return false;
}
            
</script>  
</body>
</html>
