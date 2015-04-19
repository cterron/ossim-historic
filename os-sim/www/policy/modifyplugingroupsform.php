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
require_once 'ossim_db.inc';
require_once 'classes/Plugin.inc';
require_once 'classes/Plugingroup.inc';
session_start();
Session::logcheck("MenuPolicy", "PolicyPluginGroups");
$db = new ossim_db();
$conn = $db->connect();
$plugin_list = Plugin::get_list($conn, "ORDER BY name");
$nump = intval(GET('nump'));
if ($nump == 0) $nump = 50;
if (GET('action') == 'edit') {
    $group_id = GET('id');
    ossim_valid($group_id, OSS_DIGIT, 'illegal:ID');
    if (ossim_error()) {
        die(ossim_error());
    }
    $where = "plugin_group_descr.group_id=$group_id";
    $list = Plugingroup::get_list($conn, $where);
    if (count($list) != 1) {
        die("Invalid ID");
    }
    $plug_ed = $list[0];
    $name = $plug_ed->get_name();
    $descr = $plug_ed->get_description();
    $plugs = $plug_ed->get_plugins();
    foreach($plugs as $k => $v) $_SESSION["pid" . $k] = $v['sids'];
} else {
    $group_id = $name = $descr = null;
    ossim_valid(GET('pname') , OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:Name');
    ossim_valid(GET('pdesc') , OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:Name');
    if (ossim_error()) {
        die(ossim_error());
    }
    $name = GET('pname');
    $descr = GET('pdesc');
    $plugs = array();
    if (GET('action') == 'new' && GET('pag') == "") {
        foreach($_SESSION as $k => $v) if (preg_match("/pid(\d+)/", $k, $found)) {
            unset($_SESSION["pid" . $found[1]]);
        }
    }
}
// maintain checks throught pages
foreach($_SESSION as $k => $v) if (preg_match("/pid(\d+)/", $k, $found)) {
    $plugs[$found[1]]['sids'] = $v;
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <link rel="stylesheet" type="text/css" href="../style/greybox.css"/>
  <script type="text/javascript" src="../js/jquery-1.3.2.min.js"></script>
  <script type="text/javascript" src="../js/urlencode.js"></script>
  <script type="text/javascript" src="../js/greybox.js"></script>
</head>
<body>

<?php
if (GET('withoutmenu') != "1") include ("../hmenu.php"); ?>
	
<script>

var field_id = null;
function toggle_plugin(id,des)
{
    var check = $("input[name='check"+id+"']:checked").val();
    if (check==1) {
	   $("#editsid"+id).show();
	   $("#plugin"+id).css({background: '#CFCFCF'});
    } else {
		$("#sid"+id).val('');
		$("#editsid"+id).hide();
		$("#errorsid"+id).hide();
		$("#plugin"+id).css({background: 'white'});
		if (des) {
			$.ajax({
				type: "GET",
				url: "modifyplugingroups.php?interface=ajax&method=deactivate&pid="+id,
				success: function(msg) {}
			});
		}
    }
}

function changefield(txt) {  if (field_id!=null) $("#"+field_id).val(txt); }

function validate_sids_str(id)
{
	var sids_str = $("#sid"+id).val();
	$.ajax({
		type: "GET",
		url: "modifyplugingroups.php?interface=ajax&method=validate_sids_str&sids_str="+sids_str+"&pid="+id,
		data: "",
		success: function(msg) {
			if (msg) {
				$("#errorsid"+id).show();
				$("#errorsid"+id).html(msg+'<br/>');
			} else {
				$("#errorsid"+id).hide();
			}
		}
	});
    return false;
}

function change_page (page,nump) {
	url = "modifyplugingroupsform.php?action=<?php echo $_GET["action"] ?>&id=<?php echo $_GET["id"] ?>&withoutmenu=<?php echo GET('withoutmenu') ?>&pag="+page+"&nump="+nump
	url = url+"&pname="+urlencode($("#pname").val())+"&pdesc="+urlencode($("#pdesc").val());
	document.location.href = url
}

function GB_onclose() {
}

</script>
<table width="100%" style="margin-bottom:10px;background:transparent;border:none"><tr>
<td width="25%" class="nobborder left" nowrap>
	<form action="modifyplugingroupsform.php" method="GET" style="margin:0px">
	<b>Num. plugins per page</b>: <input type="text" size=3 name="nump" value="<?php echo $nump ?>">
	<input type="hidden" name="action" value="<?php echo GET('action') ?>">
	<input type="hidden" name="id" value="<?php echo $group_id ?>">
	<input type="hidden" name="withoutmenu=" value="<?php echo GET('withoutmenu') ?>">
	<input type="submit" value="<?php echo _("View") ?>" class="btn" style="font-size:12px">
	</form>
</td>
<td class="nobborder center">
	<form id="myform" name="myform" action="modifyplugingroups.php?action=<?php echo GET('action') ?>&id=<?php echo $group_id ?>&withoutmenu=<?php echo GET('withoutmenu') ?>" method="POST" style="margin:0px">
	<input type="submit" value="<?php echo _("Accept") ?>" class="btn" style="font-size:12px">
</td>
</tr></table>
<table align="center" width="100%">
    <tr>
        <th width="10%"><?php echo _("Group ID") ?></th>
        <th width="25%"><?php echo _("Name") . required() ?></th>
        <th width="70%"><?php echo _("Description") . required() ?></th>
    </tr>
    <tr>
        <td class="noborder"><b><?php echo $group_id ?></b>&nbsp;</td>
        <td class="noborder">
            <input type="text" name="name" id="pname" value="<?php echo $name ?>" size="30">
        </td>
        <td class="noborder">
          <textarea name="descr" rows="2" id="pdesc" cols="50" wrap="on"><?php echo $descr ?></textarea>
        </td>
    </tr>
    <tr>
        <td class="noborder" colspan="3">
        
        <table width="100%">
        <?php
$paginas = ceil(count($plugin_list) / $nump);
if ($_GET["pag"] == "") {
    $p = 1;
} else {
    $p = $_GET["pag"];
}
$inicio = ($p - 1) * $nump;
if ($p < $paginas) $final = $p * $nump;
else $final = count($plugin_list);
for ($i = $inicio; $i < $final; $i++) {
    $plugin = $plugin_list[$i];
    $id = $plugin->get_id();
    if (array_key_exists($id, $plugs)) {
        $checked = 'checked';
        $sids = $plugs[$id]['sids'];
    } else {
        $checked = '';
        $sids = '';
    }
    if ($sids == "0") $sids = "ANY";
?>
            <tr id="plugin<?php echo $id
?>">
                <td width="24" style="text-align: center;">
                    <input id="check<?php echo $id
?>" name="check<?php echo $id
?>" type="checkbox" value="1" name=""
                           onClick="javascript:toggle_plugin('<?php echo $id
?>',true);"
                           <?php echo $checked
?>>
                </td>
                <td width="50"><?php echo $id
?></td>
                <td style="text-align: left;"><b><?php echo $plugin->get_name() ?></b></td>
                <td style="text-align: left; padding:0px 10px 0px 10px"><?php echo $plugin->get_description() ?></td>
                <td style="text-align: left;" nowrap>
                    <span id="errorsid<?php echo $id
?>" style="background: red; display: none"></span>
                    <span id="editsid<?php echo $id
?>" style="display: none;" NOWRAP>
                        <b>SIDs</b>:&nbsp;
                        <input id="sid<?php echo $id
?>" onBlur="javascript:return validate_sids_str('<?php echo $id
?>')"
                                 type="text" 
                                 name="sids[<?php echo $id
?>]"
                                 value="<?php echo $sids
?>"
                                 size="35"> 
                        <!-- <a href="#" onClick="javascript:return validate_sids_str('<?php echo $id
?>')"><img src="../pixmaps/theme/arrow-180-medium.png" border=0></a> -->
                        <a href="get_sids.php?id=<?php echo $id
?>" name="sid<?php echo $id
?>" class="greybox"><img src="../pixmaps/theme/magnifier-medium-left.png" align="top" border=0></a>
                    </span>&nbsp;
                    <script>toggle_plugin('<?php echo $id
?>',false)</script>
                </td>
            </tr>
        <?php
} ?>
        </table>
        </td>
    </tr>
	<tr><td colspan="3" class="noborder" style="text-align: center;">
		<?php
if ($p > 1) echo "<a href='#' onclick=\"change_page('" . ($p - 1) . "','" . $nump . "')\"><<</a>";
else echo "<span style=\" color:#A1A1A1\"><<</span>";
echo "&nbsp;" . $p . " de " . $paginas . "&nbsp;";
if ($p < $paginas) echo "<a href='#' onclick=\"change_page('" . ($p + 1) . "','" . $nump . "')\">>></a>";
else echo "<span style=\" color:#A1A1A1\">>></span>"; ?>
		 </td>
	</tr>
</table>
<br>
<center><input type="submit" value="<?php echo _("Accept") ?>" class="btn" style="font-size:12px"></center>
</form>
<script>
$(document).ready(function(){
	GB_TYPE = 'w';
	$("a.greybox").click(function(){
		//var t = this.title || $(this).text() || this.href;
		field_id = $(this).attr('name');
		sids = $('#'+field_id).val();
		GB_show("Plugin SIDs",this.href+"&field="+urlencode(sids),400,"70%");
		return false;
	});
});
</script>
