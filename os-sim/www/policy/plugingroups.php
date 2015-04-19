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
require_once 'classes/Plugingroup.inc';
require_once 'ossim_db.inc';
Session::logcheck("MenuPolicy", "PolicyPluginGroups");
$db = new ossim_db();
$conn = $db->connect();
$plgid = intval(GET('id'));
$groups = Plugingroup::get_list($conn);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <script src="../js/prototype.js" type="text/javascript"></script>
</head>
<body>

<?php
if (GET('withoutmenu') != "1") include ("../hmenu.php"); ?>

<script>
function toggle_info(id)
{
    Element.toggle('plugins'+id);
    var img = 'img'+id;
    if ($(img).src.match(/minus/)) {
        $(img).src = '../pixmaps/plus-small.png';
    } else {
        $(img).src = '../pixmaps/minus-small.png';
    }
}
    
</script>

<p style="text-align: right">
	<span style="background:#E8E8E8;border:1px solid #D7D7D7;padding:2px">&nbsp;<a href="modifyplugingroupsform.php?action=new"><img src="../pixmaps/tables/table_row_insert.png" border=0 align="absmiddle"> <?php echo _("Insert new group") ?></a>&nbsp;</span> 
</p>

<table width="95%" align="center">
    <tr>
        <th><?php echo _("ID") ?></th>
        <th><?php echo _("Name") ?></th>
        <th><?php echo _("Description") ?></th>
        <th><?php echo _("Actions") ?></th>
    </tr>
    <?php
foreach($groups as $group) {
    $id = $group->get_id();
?>
        <tr>
            <td NOWRAP>
                <img id="img<?php echo $id
?>" src="../pixmaps/plus-small.png" align="absmiddle" border="none" style="cursor:pointer" onClick="javascript: toggle_info('<?php echo $id
?>');"> <b><?php echo $id
?></b>
            </td>
            <td NOWRAP><b><?php echo htm($group->get_name()) ?></b></td>
            <td width="70%" style="text-align: left"><?php echo htm($group->get_description()) ?></td>
            <td width="1%" NOWRAP class="nobborder">
				<span style="background:#E8E8E8;border:1px solid #D7D7D7;padding:2px">&nbsp;<a href="modifyplugingroupsform.php?action=edit&id=<?php echo $id
?>"><img src="../pixmaps/tables/table_edit.png" border=0 align="absmiddle"> <?php echo _("Edit") ?></a>&nbsp;</span> 
				<span style="background:#E8E8E8;border:1px solid #D7D7D7;padding:2px">&nbsp;<a href="modifyplugingroups.php?action=delete&id=<?php echo $id
?>"><img src="../pixmaps/tables/table_row_delete.png" border=0 align="absmiddle"> <?php echo _("Delete") ?></a>&nbsp;</span> 
            </td>
        </tr>
        <tr id="plugins<?php echo $id ?>" style="display:none;background:#DEEBDB">
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td width="100%" style="text-align: left;" NOWRAP>
                <table width="100%" align="left" style="border-width:0px;background:#D3E4CF">
                <?php
    foreach($group->get_plugins() as $p) { ?>
                    <tr>
                        <td><?php echo $p['id'] ?></td>
                        <td><?php echo $p['name'] ?></td>
                        <td NOWRAP><?php echo $p['descr'] ?></td>
                        <td style="text-align:left"><?php echo ($p['sids'] == "0") ? "ANY" : $p['sids'] ?></td>
                    </tr>
                <?php
    } ?>
                </table>
            </td>
            <td>&nbsp;</td>
        </tr>

    <?php
} ?>
</table>
<?php
if ($plgid != "") { ?>
<script>toggle_info('<?php echo $plgid
?>');</script>
<?php
} ?>