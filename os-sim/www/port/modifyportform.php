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
Session::logcheck("MenuPolicy", "PolicyPorts");
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
require_once 'classes/Port_group.inc';
require_once 'classes/Port.inc';
require_once 'classes/Port_group_reference.inc';
require_once 'ossim_db.inc';
require_once 'classes/Security.inc';
$port_name = GET('portname');
ossim_valid($port_name, OSS_ALPHA, OSS_SPACE, 'illegal:' . _("Port group name"));
if (ossim_error()) {
    die(ossim_error());
}
$db = new ossim_db();
$conn = $db->connect();
if ($port_group_list = Port_group::get_list($conn, "WHERE name = '$port_name'")) {
    $port_group = $port_group_list[0];
}
?>
<form method="post" action="modifyport.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php
echo gettext("Name"); ?> </th>
      <input type="hidden" name="name"
             value="<?php
echo $port_group->get_name(); ?>">
    <td class="left">
      <b><?php
echo $port_group->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Ports"); ?> </th>
<?php
$ports = array();
$actives = array();
if ($port_list = Port::get_list($conn)) {
    foreach($port_list as $port) {
        $ports[$port->get_protocol_name() ][] = $port->get_port_number();
        $actives[$port->get_protocol_name() ][$port->get_port_number() ] = Port_group_reference::in_port_group_reference($conn, $port_group->get_name() , $port->get_port_number() , $port->get_protocol_name());
    }
}
$db->close($conn);
?>
    <td class="left">
		<?php
foreach($ports as $protocol => $list) { ?>
		<select name="proto_<?php echo $protocol ?>[]" size="20" multiple="multiple" style="width:120px">
		<?php
    foreach($list as $port) {
        $sel = ($actives[$protocol][$port]) ? "selected" : "";
        echo "<option value='$port-$protocol' $sel>$port-$protocol";
    }
?>
		</select>
		<?php
} ?>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Description"); ?> :&nbsp;</th>
    <td class="left">
      <textarea name="descr" rows="2" 
        cols="30"><?php
echo $port_group->get_descr(); ?></textarea>
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

