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
require_once ("classes/Repository.inc");
// menu authentication
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsScan");
$user = $_SESSION["_user"];
// get upload dir from ossim config file
require_once 'ossim_conf.inc';
$conf = $GLOBALS["CONF"];
$link_type = (GET('linktype') != "") ? GET('linktype') : "host";
$id_document = (GET('id_document') != "") ? GET('id_document') : ((POST('id_document') != "") ? POST('id_document') : "");
if ($id_document == "") exit;
// DB connect
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
// New link on relationships
if (GET('newlinkname') != "" && GET('insert') == "1") {
    $aux = explode("####", GET('newlinkname'));
    Repository::insert_relationships($conn, $id_document, $aux[0], $link_type, $aux[1]);
}
// Delete link on relationships
if (GET('key_delete') != "") {
    Repository::delete_relationships($conn, $id_document, GET('key_delete'));
}
$document = Repository::get_document($conn, $id_document);
$rel_list = Repository::get_relationships($conn, $id_document);
list($hostnet_list, $num_rows) = Repository::get_hostnet($conn, $link_type);
?>
<html>
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<body style="margin:0">
<table width="100%">
	<tr><th>RELATIONSHIPS for document: <?php echo $document['title'] ?></th></tr>
	<?php
if (count($rel_list) > 0) { ?>
	<tr>
		<td>
			<table class="noborder" align="center">
				<tr>
					<th>Name</th>
					<th>Type</th>
					<th>Action</th>
				</tr>
				<?php
    foreach($rel_list as $rel) {
        if ($rel['type'] == "host") $page = "/ossim/report/index.php?host=" . $rel['key'];
        if ($rel['type'] == "net") $page = "/ossim/net/net.php";
        if ($rel['type'] == "host_group") $page = "/ossim/host/hostgroup.php";
        if ($rel['type'] == "net_group") $page = "/ossim/net/netgroup.php";
        if ($rel['type'] == "incident") $page = "/ossim/incidents/incident.php?id=" . $rel['key'];
?>
				<tr>
					<td class="nobborder"><a href="<?php echo $page
?>" target="_parent"><?php echo $rel['name'] ?></a></td>
					<td class="nobborder"><?php echo ($rel['type'] == "incident") ? "ticket" : $rel['type'] ?></td>
					<td class="nobborder"><a href="<?php echo $_SERVER['PHP_SELF'] ?>?id_document=<?php echo $id_document ?>&key_delete=<?php echo $rel['key'] ?>"><img src="images/del.gif" border="0"></a></td>
				</tr>
				<?php
    } ?>
			</table>
		</td>
	</tr>
	<?php
} ?>
<form name="flinks" method="GET">
<input type="hidden" name="id_document" value="<?php echo $id_document
?>">
<input type="hidden" name="insert" value="0">
	<tr>
		<td>
			<table class="noborder" align="center">
				<tr>
					<th>Link Type</th>
					<th>Name</th>
					<td></td>
				</tr>
				<tr>
					<td>
						<select name="linktype" onchange="document.flinks.submit();">
						<option value="host"<?php
if ($link_type == "host") echo " selected" ?>>Host
						<option value="host_group"<?php
if ($link_type == "host_group") echo " selected" ?>>Host Group
						<option value="net"<?php
if ($link_type == "net") echo " selected" ?>>Net
						<option value="net_group"<?php
if ($link_type == "net_group") echo " selected" ?>>Net Group
						<option value="incident"<?php
if ($link_type == "incident") echo " selected" ?>>Ticket
						</select>
					</td>
					<td>
						<select name="newlinkname">
						<?php
foreach($hostnet_list as $hostnet) { ?>
						<option value="<?php echo $hostnet['name'] ?>####<?php echo $hostnet['key'] ?>"><?php echo $hostnet['name'] ?>
						<?php
} ?>
						</select>
					</td>
					<td><input class="btn" type="button" value="Link" onclick="document.flinks.insert.value='1';document.flinks.submit();"></td>
				</tr>
			</table>
		</td>
	</tr>
</form>
	<tr><td align="center"><input class="btn" type="button" onclick="parent.document.location.href='index.php'" value="Finish"></td></tr>
</table>
</body>
</html>
<?php
$db->close($conn);
?>
