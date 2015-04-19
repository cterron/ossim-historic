<div style="position:absolute;left:0px;top:0px;width:100%;background:#8E8E8E">
	<table width="100%" border=0 cellpadding=0 cellspacing=0 style="background:transparent"><tr>
	<td style="width:15px;vertical-align:bottom">&nbsp;</td>
	<td style="padding-top:7px">
		<table border=0 cellpadding=0 cellspacing=0 style="background:transparent"><tr>
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
if ($tabs) {
    $ctabs = count($tabs) - 1;
    $j = 0;
    foreach($tabs as $tab_id => $tab_name) {
        if (strlen($tabs[$tab_id]["tab_icon_url"]) > 0) {
            $image_string = '<img border="0" src="' . $tabs[$tab_id]["tab_icon_url"] . '">';
        } else {
            $image_string = "";
        }
        $txt = $tabs[$tab_id]["tab_name"] . $image_string;
        $url = "?panel_id=$tab_id";
        if ($panel_id == $tab_id) {
?>
				<td style="vertical-align:bottom">
					<table border=0 cellpadding=0 cellspacing=0 height="26"><tr>
					<td width="16"><img src="../pixmaps/menu/tsl<?php echo ($j > 0) ? "2" : "" ?>.gif" border=0></td>
					<td style="background:url(../pixmaps/menu/bgts.gif) repeat-x bottom left;padding:0px 15px 0px 15px" nowrap><a href="<?php echo $url ?>" class="gristabon"><?php echo $txt ?></a></td>
					<td width="16"><img src="../pixmaps/menu/tsr<?php echo ($j == $ctabs) ? "2" : "" ?>.gif" border=0></td>
					<tr></table>
				</td>
		<?php
            $selected = true;
        } else {
?>
				<td style="vertical-align:bottom">
					<table border=0 cellpadding=0 cellspacing=0 height="26"><tr>
					<?php
            if (!$selected) { ?><td width="16"><img src="../pixmaps/menu/tul<?php echo ($j == 0) ? "2" : "" ?>.gif" border=0></td><?php
            } ?>
					<td height="26" style="background:url(../pixmaps/menu/bgtu.gif) repeat-x bottom left;padding:0px 10px 0px 10px" nowrap><a href="<?php echo $url
?>" class="gristab"><?php echo $txt
?></a></td>
					<?php
            if ($j == $ctabs) { ?><td width="16"><img src="../pixmaps/menu/tur.gif" border=0></td><?php
            } ?>
					<tr></table>
				</td>
		<?php
            $selected = false;
        }
        $j++;
    }
}
?>
		<td style="width:100%;vertical-align:bottom">&nbsp;</td>
		</tr></table>
	</td>
	<td style="vertical-align:bottom;text-align:right" nowrap>


		<table border=0 style="display: <?php
$can_edit || $show_edit ? 'inline' : 'none' ?>; margin: 0px; padding: 0px;">
		<tr><td align="left" nowrap>
		<?php
if ($can_edit) { ?>
		<small>
		    <?php echo _("Panel config") ?>:
		    <?php echo _("Geom") ?>: <input id="rows" type="text" size="2" value="<?php echo $rows ?>">x
		    <input id="cols" type="text" size="2" value="<?php echo $cols ?>">
		    <a href="#" onClick="javascript:
		        panel_save($('rows').value, $('cols').value);
		        panel_load($('rows').value, $('cols').value);
		        "><?php echo _("Apply") ?></a>
		</small>
		<?php
}
?>
		</td><td align="right" nowrap><small class="white">

		<?php
if ($show_edit && !$can_edit) { ?>
		<a href="<?php echo $_SERVER['SCRIPT_NAME'] ?>?edit=1&panel_id=<?php echo $panel_id ?>"><?php
    echo gettext("Edit"); ?></a> | 
		<a href="<?php echo $_SERVER['SCRIPT_NAME'] ?>?edit_tabs=1&panel_id=<?php echo $panel_id ?>"><?php
    echo gettext("Edit Tabs"); ?></a>
		<?php
} elseif ($show_edit && $can_edit) { ?>
		<a href="<?php echo $_SERVER['SCRIPT_NAME'] ?>?edit=0&panel_id=<?php echo $panel_id ?>"><?php
    echo gettext("No Edit"); ?></a> |
		<a href="<?php echo $_SERVER['SCRIPT_NAME'] ?>?edit_tabs=1&panel_id=<?php echo $panel_id ?>"><?php
    echo gettext("Edit Tabs"); ?></a>
		<?php
} ?>
		| <a href="http://www.ossim.net/dokuwiki/doku.php?id=usingwww:executive_panel" target="popup">Help</a> |
		[<a href="<?php echo $_SERVER['SCRIPT_NAME'] ?>?fullscreen=1&panel_id=<?php echo $panel_id ?>" target="popup" onClick="wopen('<?php echo $_SERVER['SCRIPT_NAME'] ?>?fullscreen=1&panel_id=<?php echo $panel_id ?>', 'popup', 800, 600); return false;"><?php echo _("Fullscreen") ?></a>]

		</small>
		</td></tr>
		</table>


	</td>
	<td style="width:15px;vertical-align:bottom">&nbsp;</td>
	</tr></table>
</div>
<table width="100%" class="noborder" border=0 cellpadding=0 cellspacing=0><tr><td height="40">&nbsp;</td></tr></table>

