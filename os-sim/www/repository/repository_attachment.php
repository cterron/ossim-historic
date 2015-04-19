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
$uploads_dir = $conf->get_conf("repository_upload_dir");
$id_document = (GET('id_document') != "") ? GET('id_document') : ((POST('id_document') != "") ? POST('id_document') : "");
if ($id_document == "") exit;
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
// DB connect
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
list($title, $doctext, $keywords) = Repository::get_document($conn, $id_document);
if (is_uploaded_file($HTTP_POST_FILES['atchfile']['tmp_name'])) {
    // Correct format xxxxxxx.yyy
    if (preg_match("/\.(...?.?)$/", $HTTP_POST_FILES['atchfile']['name'])) {
        // Insert file row in DB
        $filename = Repository::attach($conn, $id_document, $HTTP_POST_FILES['atchfile']['name']);
        // Copy uploaded file to filesystem
        $updir = $uploads_dir . "/" . $id_document;
        $upfile = $updir . "/" . $filename;
        if (!is_dir($updir)) mkdir("$updir");
        copy($HTTP_POST_FILES['atchfile']['tmp_name'], $upfile);
    }
    // Incorrect format, can't get file type without extension
    else {
        $fileformat_error = 1;
    }
}
if (GET('id_delete') != "") {
    Repository::delete_attachment($conn, GET('id_delete') , $uploads_dir);
}
$atch_list = Repository::get_attachments($conn, $id_document);
$db->close($conn);
?>
<table cellpadding=0 cellspacing=2 border=0 width="100%">
<form name="repository_insert_form" method="POST" action="<?php echo $_SERVER['PHP_SELF'] ?>" enctype="multipart/form-data">
<input type="hidden" name="id_document" value="<?php echo $id_document ?>">
	<tr>
		<th align=center>ATTACHMENTS for Document: <?php echo $title ?></th>
	</tr>
	
	<?php
if ($fileformat_error) { ?>
	<tr><td class="nobborder">ERROR: Incorrect file format</td></tr>
	<?php
} ?>
	
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
					<td class="nobborder"><a href="<?php echo $_SERVER['PHP_SELF'] ?>?id_document=<?php echo $id_document ?>&id_delete=<?php echo $f['id'] ?>"><img src="images/del.gif" border="0"></a></td>
					<td class="nobborder"><a href="download.php?file=<?php echo urlencode($filepath) ?>&name=<?php echo urlencode($f['name']) ?>"><img src="images/download.gif" border="0"></a></td>
				</tr>
				<?php
} ?>
			</table>
		</td>
	</tr>
	
	<tr>
		<td align=center><input class=ne type=file name="atchfile"><input class="btn" type="submit" value="Upload"></td>
	</tr>
	<tr><td></td></tr>
	
	<tr><td align="center"><input class="btn" type="button" onclick="parent.document.location.href='index.php'" value="Finish"></td></tr>
</form>
</table>
</body>
</html>
