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
// menu authentication
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsScan");
$user = $_SESSION["_user"];
// Get a list of nets from db
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
require_once ("classes/Repository.inc");
require_once 'ossim_conf.inc';
$conf = $GLOBALS["CONF"];
$nmap_path = $conf->get_conf("nmap_path");
if (file_exists($nmap_path)) {
    $nmap_exists = 1;
} else {
    $nmap_exists = 0;
}
$search_str = (GET('searchstr') != "") ? GET('searchstr') : "";
$id_document = (GET('id_document') != "") ? GET('id_document') : "";
$search_bylink = (GET('search_bylink') != "") ? GET('search_bylink') : "";
// Pagination variables
$maxrows = 10;
$pag = (GET('pag') != "") ? GET('pag') : 1;
$from = ($pag - 1) * $maxrows;
if ($search_bylink != "") list($repository_list, $total) = Repository::get_list_bylink($conn, $from, $maxrows, $search_bylink);
else list($repository_list, $total) = Repository::get_list($conn, $from, $maxrows, $search_str);
$total_pages = floor(($total - 1) / $maxrows) + 1;
$db->close($conn);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <script type="text/javascript">
  function deletesubmit(txt,id) {
	if (confirm(txt+"\nAre you sure?")) {
		document.getElementById('repository_frame').src="repository_delete.php?id_document="+id;
	}	
  }
  </script>
</head>

<body>
<?php
include ("../hmenu.php"); ?>
<table cellpadding=0 cellspacing=2 border=0 width="100%" class="noborder">
	<tr>
		<td valign="top" class="nobborder">
			<table cellpadding=0 cellspacing=2 border=0 width="100%" class="noborder">
				<tr>
					<td align=center class="nobborder" style="padding-bottom:10px">
					  <table align="center" width="100%">
					    <!-- repository search form -->
						<form name="repository_search_form" method="GET" action="<?php echo $_SERVER['PHP_SELF'] ?>">
						<tr><th colspan=2>Knowledge DB DOCUMENT SEARCH</th></tr>
						<tr>
					      <td>
					        <?php
echo gettext("Please, type a search term (you can use AND, OR clauses):") ?><input type="text" value="<?php echo $search_str ?>" size="35" name="searchstr" enabled />
					      </td>
					    </tr>
					    <tr>
					      <td><input type="submit" class="btn" value="<?php
echo gettext("Search") ?>" <?php echo (!$nmap_exists) ? "disabled" : "" ?> />&nbsp;<input type="button" class="btn" value="New Document" onclick="document.location.href='index.php'"></td>
					    </tr>
						</form>
						<!-- end of repository search form -->
					  </table>
					 </td>
				</tr>
	
				<tr>
					<td class="nobborder">
						<table cellpadding=0 cellspacing=1 border=0 width="100%">
							<tr>
								<th></th>
								<th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("date", $order); ?>">
						        <?php
echo gettext("Date"); ?></a>
								</th>
						        <th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("login", $order); ?>">
						        <?php
echo gettext("User"); ?></a>
								</th>
								<th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("title", $order); ?>">
						        <?php
echo gettext("Title"); ?></a>
								</th>
								<th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("atch", $order); ?>">
						        <?php
echo gettext("Attach"); ?></a>
								</th>
								<th>
						        <?php
echo gettext("Links"); ?>
								</th>
								<?php
if ($search_str != "") { ?>
								<th>
						        <?php
    echo gettext("Relevance"); ?>
								</th>
								<?php
} ?>
							</tr>
							<?php
$i = 0;
foreach($repository_list as $repository_object) {
    if (!$i && $id_document == "" && $search_bylink != "") $color = "#D7DEE4";
    elseif ($id_document == $repository_object->id_document) $color = "#D7DEE4";
    else $color = "#FFFFFF";
?>
							<tr bgcolor="<?php echo $color
?>">
								<td nowrap>
									<a href="repository_delete.php?id_document=<?php echo $repository_object->id_document
?>" onclick="deletesubmit('Document with attachments will be deleted.',<?php echo $repository_object->id_document
?>);return false;"><img src="images/del.gif" border=0 alt="Delete Document" title="Delete Document"></a>
									<a href="repository_editdocument.php?id_document=<?php echo $repository_object->id_document
?>" target="repository_frame"><img src="images/editdocu.gif" border=0 alt="Edit Document" title="Edit Document"></a>
								</td>
								<td><?php echo $repository_object->date
?></td>
								<td><?php echo $repository_object->user
?></td>
								<td class="left">
									<a href="repository_document.php?id_document=<?php echo $repository_object->id_document
?>&maximized=1&search_bylink=<?php echo $search_bylink
?>&pag=<?php echo $pag
?>"><?php echo $repository_object->title
?></a>
								</td>
								<td><table align="center"><tr><?php
    if (count($repository_object->atch) > 0) echo "<td class='nobborder'>(" . count($repository_object->atch) . ")</td>"; ?><td class="nobborder"><a href='repository_attachment.php?id_document=<?php echo $repository_object->id_document ?>' target='repository_frame'><img src='images/attach.gif' border=0 alt="Attached Files" title="Attached Files"></a></td></tr></table></td>
								<td><table align="center"><tr><?php
    if (count($repository_object->rel) > 0) echo "<td class='nobborder'>(" . count($repository_object->rel) . ")</td>"; ?><td class="nobborder"><a href="repository_links.php?id_document=<?php echo $repository_object->id_document ?>" target="repository_frame"><img src="images/linked2.gif" border=0 alt="Linked Elements" title="Linked Elements"></a></td></tr></table></td>
								<?php
    if ($search_str != "") { ?>
								<td align="center"><?php echo $repository_object->get_relevance() ?>%</td>
								<?php
    } ?>
							</tr>
							<?php
    $i++;
} ?>
						</table>
					</td>
				</tr>
				
				<!-- Pagination -->
				<tr>
					<td class="nobborder">
						<table>
							<tr>
							<td class="nobborder" style="padding-right:10px">Pages:</td>
							<td class="nobborder">
							<?php
for ($i = 1; $i <= $total_pages; $i++) { ?>
							<?php
    if ($i == $pag) { ?>
							<b><?php echo $i
?></b>&nbsp;
							<?php
    } else { ?>
							<a href="<?php echo $_SERVER['PHP_SELF'] ?>?searchstr=<?php echo $search_str ?>&search_bylink=<?php echo $search_bylink ?>&id_document=<?php echo $id_document ?>&pag=<?php echo $i ?>"><?php echo $i ?></a>&nbsp;
							<?php
    } ?>
							<?php
} ?>
							</td></tr>
						</table>
					</td>
				</tr>
			</table>
		</td>
		<td width="500" valign="top" class="nobborder">
			<?php
if ($id_document != "") $frame_src = "repository_document.php?id_document=$id_document";
elseif ($search_bylink != "") $frame_src = "repository_document.php?id_document=" . array_shift($repository_list)->id_document;
else $frame_src = "repository_newdocument.php";
?>
			<IFRAME name="repository_frame" id="repository_frame" src="<?php echo $frame_src
?>" width="100%" height="400" frameborder="0"></IFRAME>
		</td>
	</tr>
</table>
</body>
</html>
