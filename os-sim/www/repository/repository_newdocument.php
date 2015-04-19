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
if (POST('title') != "" && POST('doctext') != "") {
    // Get a list of nets from db
    require_once ("ossim_db.inc");
    $db = new ossim_db();
    $conn = $db->connect();
    $id_inserted = Repository::insert($conn, POST('title') , POST('doctext') , POST('keywords') , $user);
    $db->close($conn);
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
<table cellpadding=0 cellspacing=2 border=0 width="100%">
	<tr>
		<th>NEW DOCUMENT</th>
	</tr>
	<tr>
		<td class="center">Document inserted with id: <?php echo $id_inserted ?></td>
	</tr>
	<tr><td class="center">Do you want to attach a document file? <input type="button" class="btn" onclick="document.location.href='repository_attachment.php?id_document=<?php echo $id_inserted ?>'" value="YES">&nbsp;<input class="btn" type="button" onclick="parent.document.location.href='index.php'" value="NO"></td></tr>
</table>
<?php
} else { ?>
<html>
<head>
  <title> <?php
    echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <link rel="stylesheet" type="text/css" href="../style/jquery.wysiwyg.css"/>
  <script type="text/javascript" src="../js/jquery-1.3.2.min.js"></script>
  <script type="text/javascript" src="../js/jquery.wysiwyg.js"></script>
  <script type="text/javascript">
	$(document).ready(function() {
		$('#textarea').wysiwyg({
			css : { fontFamily: 'Arial, Tahoma', fontSize : '13px'}
		});
	});
  </script>
</head>

<body style="margin:0">
<table cellpadding=0 cellspacing=2 border=0 width="100%">
	<tr>
		<th>NEW DOCUMENT</th>
	</tr>
	<tr>
		<td>
			<!-- repository insert form -->
			<form name="repository_insert_form" method="POST" action="<?php echo $_SERVER['PHP_SELF'] ?>" enctype="multipart/form-data">
			<table cellpadding=0 cellspacing=2 border=0 class="noborder">
				<tr>
					<td class="nobborder" style="padding-left:5px">Title</td>
				</tr>
				<tr>
					<td class="nobborder" style="padding-left:5px"><input type="text" name="title" style="width:473px" value="<?php echo POST('title') ?>"></td>
				</tr>
				<tr>
					<td class="nobborder" style="padding-left:5px"><?php echo _("Text") ?></td>
				</tr>
				<tr>
					<td class="nobborder" style="padding-left:5px">
						<textarea id="textarea" name="doctext" rows="4" style="width:460px"><?php echo POST('doctext') ?></textarea>
					</td>
				</tr>
				
				<tr>
					<td class="nobborder" style="padding-left:5px"><?php echo _("Keywords") ?></td>
				</tr>
				<tr>
					<td class="nobborder" style="padding-left:5px">
						<textarea name="keywords" cols="73"><?php echo POST('keywords') ?></textarea>
					</td>
				</tr>
				
				<tr><td align="right" class="nobborder" style="padding-left:5px"><input class="btn" type="submit" value="<?php echo _("Save") ?>"></td></tr>
			</table>
			</form>
			<!-- end of repository insert form -->
		</td>
	</tr>
</table>
<?php
} ?>
</body>
</html>
