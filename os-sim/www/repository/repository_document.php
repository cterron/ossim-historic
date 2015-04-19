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
$id_document = (GET('id_document') != "") ? GET('id_document') : ((POST('id_document') != "") ? POST('id_document') : "");
if ($id_document == "") exit;
$maximized = (GET('maximized') != "") ? 1 : 0;
// DB Connection
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
$document = Repository::get_document($conn, $id_document);
$atch_list = Repository::get_attachments($conn, $id_document);
$rel_list = Repository::get_relationships($conn, $id_document);
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
<?php
include ("../hmenu.php"); ?>
<table cellpadding=0 cellspacing=2 border=0 width="100%">
<th colspan=2 style="font-size:12px;padding:2px 0px 2px 0px">
    &nbsp;<?php echo $document['title'] ?>
</th>
	<tr>
		<?php
if (!$maximized) { ?>
		<td style="padding:3px"><a href="repository_document.php?id_document=<?php echo $id_document
?>&maximized=1" target="_parent"><img src="images/max.gif" align="absmiddle" border=0> Maximize</a></td>
		<?php
} else { ?>
		<td style="padding:3px"><a href="index.php?pag=<?php echo GET('pag') ?>&search_bylink=<?php echo GET('search_bylink') ?>"><?php echo _("Back to main") ?></a></td>
		<?php
} ?>
	</tr>
	<tr>
		<td>
			<table cellpadding=0 cellspacing=0 border=0 class="noborder">
				<tr>
					<td valign="top" width="250" style="padding-right:20px">
						<table cellpadding=0 cellspacing=2 border=0 width="100%">
							<tr><th>Date</th></tr>
							<tr>
								<td class="center" style="padding-left:5px"><?php echo $document['date'] ?></td>
							</tr>
							<tr><th>User</th></tr>
							<tr>
								<td class="center" style="padding-left:5px"><?php echo $document['user'] ?></td>
							</tr>
							<tr><th>Keywords</th></tr>
							<tr>
								<td class="center" style="padding-left:5px"><?php echo $document['keywords'] ?></td>
							</tr>
							
							<tr><th>Attachments</th></tr>
							<!-- Attachments -->
							<tr>
								<td>
									<table class="noborder" align="center">
										<?php
foreach($atch_list as $f) {
    $type = ($f['type'] != "") ? $f['type'] : "unkformat";
    $img = (file_exists("images/$type.gif")) ? "images/$type.gif" : "images/unkformat.gif";
    $filepath = "../uploads/$id_document/" . $f['id_document'] . "_" . $f['id'] . "." . $f['type'];
?>
										<tr>
											<td align=center class="nobborder"><img src="<?php echo $img
?>"></td>
											<td class="nobborder"><a href="<?php echo $filepath
?>" target="_blank"><?php echo $f['name'] ?></a></td>
											<td class="nobborder"><a href="<?php echo $filepath ?>">[View]</a></td>
											<td class="nobborder"><a href="download.php?file=<?php echo urlencode($filepath) ?>&name=<?php echo urlencode($f['name']) ?>"><img src="images/download.gif" border="0"></a></td>
										</tr>
										<?php
} ?>
									</table>
								</td>
							</tr>
							
							<tr><th>Links</th></tr>
							<!-- Relationships -->
							<tr>
								<td>
									<table class="noborder" align="center">
										<?php
foreach($rel_list as $rel) {
    if ($rel['type'] == "host") $page = "../report/index.php?host=" . $rel['key'];
    if ($rel['type'] == "net") $page = "../net/net.php";
    if ($rel['type'] == "host_group") $page = "../host/hostgroup.php";
    if ($rel['type'] == "net_group") $page = "../net/netgroup.php";
    if ($rel['type'] == "incident") $page = "../net/incidents/incident.php?id=" . $rel['key'];
?>
										<tr>
											<td class="nobborder"><a href="<?php echo $page
?>"><?php echo $rel['name'] ?></a></td>
											<td class="nobborder"><?php echo ($rel['type'] == "incident") ? "ticket" : $rel['type'] ?></td>
										</tr>
										<?php
} ?>
									</table>
								</td>
							</tr>
						</table>
					</td>
					<td valign="top">
						<table cellpadding=0 cellspacing=2 border=0 class="noborder">
							<tr>
								<td class="nobborder" style="padding-left:5px">
									<?php echo $document['text'] ?>
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<?php
$db->close($conn); ?>
</body>
</html>
