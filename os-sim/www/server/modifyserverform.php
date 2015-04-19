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
Session::logcheck("MenuPolicy", "PolicyServers");
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
if (GET('withoutmenu') != "1") include ("../hmenu.php"); ?>

<?php
require_once 'classes/Server.inc';
require_once 'ossim_db.inc';
require_once 'classes/Security.inc';
$name = GET('name');
ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_SCORE, 'illegal:' . _("Server name"));
if (ossim_error()) {
    die(ossim_error());
}
$db = new ossim_db();
$conn = $db->connect();
if ($server_list = Server::get_list($conn, "WHERE name = '$name'")) {
    $server = $server_list[0];
}
if ($role_list = Role::get_list($conn, $name)) {
    $role = $role_list[0];
}
$db->close($conn);
?>

<form method="post" action="modifyserver.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php
echo gettext("Hostname"); ?> </th>
      <input type="hidden" name="name"
             value="<?php
echo $server->get_name(); ?>">
    <td class="left">
      <b><?php
echo $server->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("IP"); ?> </th>
    <td class="left">
        <input type="text" name="ip" 
               value="<?php
echo $server->get_ip(); ?>"></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Port"); ?> </th>
    <td class="left">
        <input type="text" name="port" 
               value="<?php
echo $server->get_port(); ?>"></td>
  </tr>
<?php
?>
  <tr>
    <th> <?php
echo gettext("Correlate events"); ?> </th>
    <td class="left">
    <input type="radio" name="correlate" value="1" <?php
if ($role->get_correlate() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="correlate" value="0" <?php
if ($role->get_correlate() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Cross Correlate events"); ?> </th>
    <td class="left">
    <input type="radio" name="cross_correlate" value="1" <?php
if ($role->get_cross_correlate() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="cross_correlate" value="0" <?php
if ($role->get_cross_correlate() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Store events"); ?> </th>
    <td class="left">
    <input type="radio" name="store" value="1" <?php
if ($role->get_store() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="store" value="0" <?php
if ($role->get_store() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Qualify events"); ?> </th>
    <td class="left">
    <input type="radio" name="qualify" value="1" <?php
if ($role->get_qualify() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="qualify" value="0" <?php
if ($role->get_qualify() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Resend alarms"); ?> </th>
    <td class="left">
    <input type="radio" name="resend_alarms" value="1" <?php
if ($role->get_resend_alarm() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="resend_alarms" value="0" <?php
if ($role->get_resend_alarm() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Resend events"); ?> </th>
    <td class="left">
    <input type="radio" name="resend_events" value="1" <?php
if ($role->get_resend_event() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="resend_events" value="0" <?php
if ($role->get_resend_event() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Sign"); ?> </th>
    <td class="left">
    <input type="radio" name="sign" value="1" <?php
if ($role->get_sign() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="sign" value="0" <?php
if ($role->get_sign() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Sem"); ?> </th>
    <td class="left">
    <input type="radio" name="sem" value="1" <?php
if ($role->get_sem() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="sem" value="0" <?php
if ($role->get_sem() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Sim"); ?> </th>
    <td class="left">
    <input type="radio" name="sim" value="1" <?php
if ($role->get_sim() == 1) echo " checked "; ?>> <?php echo _("Yes"); ?>
    <input type="radio" name="sim" value="0" <?php
if ($role->get_sim() == 0) echo " checked "; ?>> <?php echo _("No"); ?>
    </td>
  </tr>

  <tr>
    <th> <?php
echo gettext("Description"); ?> </th>
    <td class="left">
        <textarea name="descr" rows="2"
            cols="20"><?php
echo $server->get_descr(); ?></textarea>
    </td>
  </tr>

  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK" class="btn" style="font-size:12px">
      <input type="reset" value="reset" class="btn" style="font-size:12px">
    </td>
  </tr>
</table>
</form>

</body>
</html>

