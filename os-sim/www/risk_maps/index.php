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
* - check_writable_relative()
* Classes list:
*/
require_once 'classes/Session.inc';
Session::logcheck("MenuControlPanel", "BusinessProcesses");

if (!Session::menu_perms("MenuControlPanel", "BusinessProcessesEdit")) {
print _("You don't have permissions to edit risk indicators");
exit();
}

function check_writable_relative($dir){
$uid = posix_getuid();
$gid = posix_getgid();
$user_info = posix_getpwuid($uid);
$user = $user_info['name'];
$group_info = posix_getgrgid($gid);
$group = $group_info['name'];
$fix_cmd = '. '._("To fix that, execute following commands as root").':<br><br>'.
		   "cd " . getcwd() . "<br>".
                   "mkdir -p $dir<br>".
                   "chown $user:$group $dir<br>".
                   "chmod 0700 $dir";
if (!is_dir($dir)) {
     die(_("Required directory " . getcwd() . "$dir does not exist").$fix_cmd);
}
$fix_cmd .= $fix_extra;

if (!$stat = stat($dir)) {
	die(_("Could not stat configs dir").$fix_cmd);
}
        // 2 -> file perms (must be 0700)
        // 4 -> uid (must be the apache uid)
        // 5 -> gid (must be the apache gid)
if ($stat[2] != 16832 || $stat[4] !== $uid || $stat[5] !== $gid)
        {
            die(_("Invalid perms for configs dir").$fix_cmd);
        }
}

check_writable_relative("./maps");
check_writable_relative("./pixmaps/uploaded");

/*

Requirements: 
- web server readable/writable ./maps
- web server readable/writable ./pixmaps/uploaded
- standard icons at pixmaps/standard
- Special icons at docroot/ossim_icons/


TODO: Rewrite code, beutify, use ossim classes for item selection, convert operations into ossim classes

*/
?>
<html>
<head>
<title><?= _("Risk Maps") ?>  - <?= _("Edit") ?></title>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <script type="text/javascript" src="lytebox.js"></script>
  <script type="text/javascript" src="../js/jquery-1.3.2.min.js"></script>
  <script type="text/javascript" src="../js/greybox.js"></script>
  <link rel="stylesheet" type="text/css" href="custom_style.css">
  <link rel="stylesheet" href="lytebox.css" type="text/css" media="screen" />
  <link rel="stylesheet" type="text/css" href="../style/greybox.css" />
</head>
<? 

require_once 'classes/Security.inc';


$erase_element = $_GET['delete'];
$erase_type = $_GET['delete_type'];
$map = ($_POST["map"] != "") ? $_POST["map"] : (($_GET["map"] != "") ? $_GET["map"] : (($_SESSION["riskmap"]!="") ? $_SESSION["riskmap"] : 1));
$type = ($_GET["type"]!="") ? $_GET["type"] : "host";
$name = $_POST['name'];

ossim_valid($erase_element, OSS_SCORE, OSS_NULLABLE, OSS_ALPHA, OSS_DIGIT, ";,.", 'illegal:'._("erase_element"));
ossim_valid($type, OSS_ALPHA, OSS_DIGIT, 'illegal:'._("type"));
ossim_valid($name, OSS_ALPHA, OSS_NULLABLE, OSS_DIGIT, OSS_SCORE, ".,%", 'illegal:'._("name"));
ossim_valid($map, OSS_DIGIT, 'illegal:'._("type"));

if (ossim_error()) {
die(ossim_error());
}

// Cleanup a bit

$name = str_replace("..","",$name);
$erase_element = str_replace("..","",$erase_element);

if (is_uploaded_file($HTTP_POST_FILES['fichero']['tmp_name'])) {
	$filename = "pixmaps/uploaded/" . $name . ".jpg";
	if(getimagesize($HTTP_POST_FILES['ficheromap']['tmp_name'])){
		move_uploaded_file($HTTP_POST_FILES['fichero']['tmp_name'], $filename);
	}
}
if (is_uploaded_file($HTTP_POST_FILES['ficheromap']['tmp_name'])) {
	$filename = "maps/" . $name . ".jpg";
	if(getimagesize($HTTP_POST_FILES['ficheromap']['tmp_name'])){
		move_uploaded_file($HTTP_POST_FILES['ficheromap']['tmp_name'], $filename);
	}
}
if ($erase_element != "") {
	switch($erase_type){
		case "map":
			if(getimagesize("maps/" . $erase_element)){
			unlink("maps/" . $erase_element);
			}
		break;
		case "icon":
			if(getimagesize("pixmaps/uploaded/" . $erase_element)){
			unlink("pixmaps/uploaded/" . $erase_element);
			}
		break;
		default:
		break;
	}
}

require_once 'ossim_db.inc';
$db = new ossim_db();
$conn = $db->connect();

//$types = array("host","net","host_group","net_group","sensor","server");
$types = array("host","net","sensor","server");

$data_types = array();
foreach ($types as $htype) {
  if($htype == "host"){
		$query = "select hostname as name from $htype order by hostname";
	} else {
		$query = "select name from $htype order by name";
	}
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
		while (!$rs->EOF){
		$data_types[$htype][] = $rs->fields["name"];
		$rs->MoveNext();
		}
	}
}
?>
<script>

function GB_onclose() {
}

function loadLytebox(){
	var cat = document.getElementById('category').value;
	var id = cat + "-0";
	myLytebox.start(document.getElementById(id));
}

function choose_icon(icon){
var cat   = document.getElementById('category').value;
var timg = document.getElementById('chosen_icon');
//var res = "48x48";
//
//for( i = 0; i < document.f.resolution2.length; i++ )
//{
//	if( document.f.resolution2[i].checked == true ){
//		res = document.f.resolution2[i].value;
//		break;
//	}
//}
//icon2 = icon.replace("RESOLUTION",res);
//timg.src= icon2;
timg.src = icon
changed = 1;
}

function toggleLayer( whichLayer )
{
  var elem, vis;
  if( document.getElementById ) // this is the way the standards work
    elem = document.getElementById( whichLayer );
  else if( document.all ) // this is the way old msie versions work
      elem = document.all[whichLayer];
  else if( document.layers ) // this is the way nn4 works
    elem = document.layers[whichLayer];
  vis = elem.style;
  // if the style.display value is blank we try to figure it out here
  if(vis.display==''&&elem.offsetWidth!=undefined&&elem.offsetHeight!=undefined)
    vis.display = (elem.offsetWidth!=0&&elem.offsetHeight!=0)?'block':'none';
  vis.display = (vis.display==''||vis.display=='block')?'none':'block';
}


	template_begin = '<table border=0 cellspacing=0 cellpadding=1><tr><td colspan=2 class=ne1 align=center><i>NAME</i></td></tr><tr><td><img src="ICON" border=0></td><td>'
	template_end = '</td></tr></table>'
	template_rect = '<table border=0 cellspacing=0 cellpadding=0 width="100%" height="100%"><tr><td style="border:1px dotted black">&nbsp;</td></tr></table>'
	txtbbb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtbbr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtbba = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtbbv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtbrb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtbrr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtbra = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtbrv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtbab = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtbar = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtbaa = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtbav = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtbvb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtbvr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtbva = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtbvv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'

	txtrbb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtrbr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtrba = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtrbv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtrrb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtrrr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtrra = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtrrv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtrab = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtrar = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtraa = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtrav = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtrvb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtrvr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtrva = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtrvv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'

	txtabb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtabr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtaba = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtabv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtarb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtarr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtara = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtarv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtaab = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtaar = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtaaa = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtaav = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtavb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtavr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtava = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtavv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'

	txtvbb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtvbr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtvba = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtvbv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtvrb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtvrr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtvra = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtvrv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtvab = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtvar = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtvaa = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtvav = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'
	txtvvb = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/b.gif" border=0></td></tr></table>'
	txtvvr = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/r.gif" border=0></td></tr></table>'
	txtvva = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/a.gif" border=0></td></tr></table>'
	txtvvv = '<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td><td><img src="images/v.gif" border=0></td></tr></table>'

	function findPos(obj) {
		var curleft = curtop = 0;
		if (obj.offsetParent) {
		do {
			curleft += obj.offsetLeft;
			curtop += obj.offsetTop;
		} while (obj = obj.offsetParent);
		return [curleft,curtop];
	}
	}

	var moz = document.getElementById && !document.all;
	var moving = false;
	var resizing = false;
	var dobj;	
	var changed = 0;
	function dragging(e){
		if (moving) {
			x = moz ? e.clientX : event.clientX;
			y = moz ? e.clientY : event.clientY;
			document.f.state.value = "<?= _("moving...") ?>";
			document.f.posx.value = x + window.scrollX
			document.f.posy.value = y + window.scrollY
			dobj.style.left = x + window.scrollX - parseInt(dobj.style.width)/2;
			dobj.style.top = y + window.scrollY - parseInt(dobj.style.height)/2;
			// Check if it's under the wastebin icon
			var waste = document.getElementById("wastebin")
			var waste_pos = [];
			waste_pos  = findPos(waste);
			if ( x>= waste_pos[0] && x<= waste_pos[0] + 48 && y>=waste_pos[1] && y<= waste_pos[1] + 53 ) {
				dobj.style.visibility = 'hidden'
			}
			changed = 1;
			return false;
		}
		if (resizing) {
			x = moz ? e.clientX+10+ window.scrollX : event.clientX+10+ window.scrollX;
			y = moz ? e.clientY+10+ window.scrollY : event.clientY+10+ window.scrollY;
			document.f.state.value = "<?= _("resizing...") ?>";
			document.f.posx.value = x + window.scrollX;
			document.f.posy.value = y + window.scrollY;
			xx = parseInt(dobj.style.left.replace('px','')) + 5;
			yy = parseInt(dobj.style.top.replace('px','')) + 5;
			w = (x > xx) ? x-xx : xx
			h = (y > yy) ? y-yy : yy
			dobj.style.width = w
			dobj.style.height = h 
			changed = 1;
			return false;
		}
	}
	function releasing(e) {
		moving = false;
		resizing = false;
		document.f.state.value = ""
		if (dobj != undefined) dobj.style.cursor = 'pointer'
	}
	function pushing(e) {
		var fobj = moz ? e.target : event.srcElement;
		var button = moz ? e.which : event.button;
		var id = fobj.id;
		while (fobj.tagName.toLowerCase() != "html" && fobj.className != "itcanbemoved") {
			fobj = moz ? fobj.parentNode : fobj.parentElement;
		}			
		if (fobj.className == "itcanbemoved") {
                       var ida = fobj.id.replace("alarma","").replace("rect","");
                       if (document.getElementById('dataname'+ida)) {
                               document.f.url.value = document.getElementById('dataurl'+ida).value
                               document.f.alarm_id.value = ida
				if(!id.indexOf('rect',0)){
                                document.f.alarm_name.value = document.getElementById('dataname'+ida).value
			       	if(document.getElementById('dataicon' + ida) != null){
			       		document.getElementById('chosen_icon').src = document.getElementById('dataicon'+ida).value
				}
				}
                       }
			if (button>1) {
				resizing = true;
				fobj.style.cursor = 'nw-resize'
			} else {
				moving = true;
				fobj.style.cursor = 'move'
			}
			dobj = fobj
			return false;
		}
	}
	document.onmousedown = pushing;
	document.onmouseup = releasing;
	document.onmousemove = dragging;

	function responderAjax(url) {
		var ajaxObject = document.createElement('script');
		ajaxObject.src = url;
		ajaxObject.type = "text/javascript";
		ajaxObject.charset = "utf-8";
		document.getElementsByTagName('head').item(0).appendChild(ajaxObject);
	}
	function urlencode(str) { return escape(str).replace('+','%2B').replace('%20','+').replace('*','%2A').replace('/','%2F').replace('@','%40'); }

	function drawDiv (id,name,valor,icon,url,x,y,w,h) {
		var el = document.createElement('div');
		var the_map= document.getElementById("map_img")
		var map_pos = [];
		map_pos = findPos(the_map);
		el.id='alarma'+id
		el.className='itcanbemoved'
		el.style.position = 'absolute';
		el.style.left = x + map_pos[0];
		el.style.top = y
		el.style.width = w
		el.style.height = h
		var content = template_begin.replace('NAME',name).replace('ICON',icon) + valor + template_end
		el.innerHTML = content;
		el.style.visibility = 'visible'
		document.body.appendChild(el);
                document.getElementById('tdnuevo').innerHTML += '<input type="hidden" name="dataname' + id + '" id="dataname' + id + '" value="' + name + '">\n';
                document.getElementById('tdnuevo').innerHTML += '<input type="hidden" name="dataurl' + id + '" id="dataurl' + id + '" value="' + url + '">\n';
                if(document.getElementById('dataicon' + ida) != null){
                	document.getElementById('tdnuevo').innerHTML += '<input type="hidden" name="dataicon' + id + '" id="dataicon' + id + '" value="' + icon + '">\n';
		}
		document.f.state.value = "<?= _("New") ?>"
	}

	function changeDiv (id,name,url,icon,valor) {
		var content = template_begin.replace('NAME',name).replace('ICON',icon) + valor + template_end
		document.getElementById('alarma'+id).innerHTML = content;
		document.f.state.value = ""
		changed = 1;
	}

	function initDiv () {
		var x = 0;
		var y = 0;
		var el = document.getElementById('map_img');
		var obj = el;
		do {
			x += obj.offsetLeft;
			y += obj.offsetTop;
			obj = obj.offsetParent;
		} while (obj);	
		var objs = document.getElementsByTagName("div");
		var txt = ''
		for (var i=0; i < objs.length; i++) {
			if (objs[i].className == "itcanbemoved") {
				xx = parseInt(objs[i].style.left.replace('px',''));
				objs[i].style.left = xx + x
				yy = parseInt(objs[i].style.top.replace('px',''));
				objs[i].style.top = yy + y;
				objs[i].style.visibility = "visible"
			}
		}
		refresh_indicators()
		// greybox
		$("a.greybox").click(function(){
		   var t = this.title || $(this).text() || this.href;
		   var url = this.href + "?dir=" + document.getElementById('category').value;
		   GB_show(t,url,420,"50%");
		   return false;
		});
	}
	
	function addnew(map,type) {
               document.f.alarm_id.value = ''
		if (type == 'alarm') {
			if (document.f.alarm_name.value != '') {
				var txt = ''
				var robj = document.getElementById("chosen_icon");
				txt = txt + urlencode(robj.src) + ';'
				type = document.f.type.options[document.f.type.selectedIndex].value
				elem = document.f.elem.options[document.f.elem.selectedIndex].value
				txt = txt + urlencode(type) + ';' + urlencode(elem) + ';'
				txt = txt + urlencode(document.f.alarm_name.value) + ';'
				txt = txt + urlencode(document.f.url.value) + ';'
				responderAjax('responder.php?map=' + map + '&data=' + txt)
				document.f.state.value = '<?= _("New Indicator created.") ?>!'
			} else {
				alert("<?= _("Indicator name can't be void") ?>")
			}	
		} else {
			responderAjax('responder.php?map=' + map + '&type=rect&url=' + urlencode(document.f.url.value) )
			document.f.state.value = '<?= _("New rectangle created") ?>'
		}
		changed = 1;
	}

	function drawRect (id,x,y,w,h) {
		var el = document.createElement('div');
		var the_map= document.getElementById("map_img")
		var map_pos = [];
		map_pos = findPos(the_map)
		el.id='rect'+id
		el.className='itcanbemoved'
		el.style.position = 'absolute';
		el.style.left = x + map_pos[0];
		el.style.top = y
		el.style.width = w
		el.style.height = h
		var content = template_rect
		el.innerHTML = content;
		el.style.visibility = 'visible'
		document.body.appendChild(el);
		document.f.state.value = "<?= _("New") ?>"
		changed = 1;
	}
	
	function save(map) {
		var x = 0;
		var y = 0;
		var el = document.getElementById('map_img');
		var obj = el;
		do {
			x += obj.offsetLeft;
			y += obj.offsetTop;
			obj = obj.offsetParent;
		} while (obj);	
		var objs = document.getElementsByTagName("div");
		var txt = ''
		for (var i=0; i < objs.length; i++) {
			if (objs[i].className == "itcanbemoved" && objs[i].style.visibility != "hidden") {
				xx = objs[i].style.left.replace('px','');
				yy = objs[i].style.top.replace('px','');
				txt = txt + objs[i].id + ',' + (xx-x) + ',' + (yy-y) + ',' + objs[i].style.width + ',' + objs[i].style.height + ';';
			}
		}
                responderAjax('save.php?map=' + map + '&id=' + document.f.alarm_id.value + '&name=' + urlencode(document.f.alarm_name.value) + '&url=' + urlencode(document.f.url.value) + '&icon=' + urlencode(document.getElementById("chosen_icon").src) + '&data=' + txt);
		document.f.state.value = "<?= _("Indicators saved.") ?>";
		changed = 0;
	}
	
	function refresh_indicators() {
		responderAjax("refresh.php?map=<? echo $map ?>")
	}
	refresh_indicators();
	setInterval(refresh_indicators,5000);

	
	function chk(fo) {
		if  (fo.name.value=='') {
			alert("Icon requires a name!");
			return false;
		}
		return true;
	}
	function view() { document.location.href = '<? echo $SCRIPT_NAME ?>?map=<? echo $map ?>&type=' + document.f.type.options[document.f.type.selectedIndex].value }	
</script>
<body leftmargin=5 topmargin=5 class=ne1 onload="initDiv()" oncontextmenu="return false;" onunload="checkSaved()">
<head>
<style type="text/css">
	.itcanbemoved { position:absolute; cursor:pointer }
</style>
</head>
<table border=0 cellpadding=0 cellspacing=0><tr>
<td valign=top class=ne1 style="padding-left:5px"/
<a href="javascript:toggleLayer('uploads');"><?= _("Toggle upload area") ?></a> ||
<a href="view.php?map=<?= $map ?>"><?= _("Back to view mode") ?></a>
<div id="uploads">
<h2><?= _("Maps") ?></h2><br>
 <?
	$maps = explode("\n",`ls -1 'maps'`);
	$i=0; $n=0; $mn=0; $txtmaps = ""; $linkmaps = "";
	foreach ($maps as $ico) if (trim($ico)!="") {
		if (is_dir("maps/" . $ico) || !getimagesize("maps/" . $ico)) { continue; }
		$n = intval(str_replace("map","",str_replace(".jpg","",$ico)));
		if ($mn<$n) $mn = $n;
		$txtmaps .= "<td><a href='$SCRIPT_NAME?map=$n'><img src='maps/$ico' border=".(($map==$n) ? "2" : "0")." width=100 height=100></a></td><td><a href='$SCRIPT_NAME?map=$map&delete_type=map&delete=".urlencode("$ico")."'><img src='images/delete.png' border=0></a></td>";
		$linkmaps .= "<td><a href='javascript:;' onclick='document.f.url.value=\"view.php?map=$n\"'><img src='maps/$ico' border=0 width=50 height=50 style='border:1px solid #cccccc' alt='$ico' title='$ico'></a></td>";
		$i++; if ($i % 3 == 0) {
			$txtmaps .= "</tr><tr>";
			$linkmaps .= "</tr><tr>";
		}
	}
 ?> 
 <form action="index.php" method=post name=f1 enctype="multipart/form-data">
<?= _("Upload map file") ?>: <input type=hidden value="<? echo $map ?>" name=map>
 <input type=hidden name=name value="map<? echo ($mn+1) ?>"><input type=file class=ne1 size=15 name=ficheromap>
 <input type=submit value="<?= _("Upload") ?>" class="btn" style="font-size:12px">
 </form> 
 <?= _("Choose a map to edit:") ?>
 <table><tr><? echo $txtmaps ?></tr></table>	
 <center>
 <hr noshade width="50%">
 </center>
 <h2><?= _("Icons") ?></h2><br> 
 <form action="index.php" method=post name=f2 enctype="multipart/form-data" onsubmit="return chk(document.f2)">
<?= _("Name Icon") ?>: 
&nbsp;
&nbsp;
&nbsp;
&nbsp;
<input type=text class=ne1 size=20 name=name><br/>
<?= _("Upload icon file") ?>: <input type=file class=ne1 size=10 name=fichero>
<input type=hidden value="<? echo $map ?>" name=map>
<input type=submit value="<?= _("Upload") ?>" class="btn" style="font-size:12px">
 </form>
</div>
<div style="display:none">
<form name="f" action="modify.php"><input type=hidden name="alarm_id" value=""> x <input type=text size=1 name=posx> y <input type=text size=1 name=posy> <input type=text size=30 name=state style="border:1px solid white">

 </div>
 <hr noshade>
 <h2><?= _("Create new indicator") ?></h2>
<table border="0" width="100%"><tr><td>
<?php
$docroot = "/var/www/";
$resolution = "128x128";
$icon_cats = explode("\n",`ls -1 '$docroot/ossim_icons/Regular/'`);
print "<SELECT id=\"category\" name=\"categories\">";
print "<option value=\"standard\">Default Icons";
print "<option value=\"custom\">Own Uploaded";
foreach($icon_cats as $ico_cat){
if(!$ico_cat)continue;
print "<option value=\"$ico_cat\">$ico_cat";
}
print "</select>";

/*
$resolutions = array("16x16", "24x24", "32x32", "48x48", "72x72", "128x128", "256x256");
print "<br/>";
$i = 0;
foreach($resolutions as $ress){
print "<input type='radio' name='resolution2' value='$ress' ".($ress==$resolution ? "checked" : "")."><small>$ress</small>";
$i++; if ($i % 3 == 0) echo "</br>";
}*/
?>
</td>
<td rowspan="2" align="center" valign="middle" width="40%"><img src="pixmaps/standard/default.png" name="chosen_icon" id="chosen_icon"></td>
</tr>
<tr>
<td align="center"><a href="chooser.php" title="Icon browser" class="greybox" style="font-size:14px">Browse all</a> / <a href="javascript:loadLytebox()" id="lytebox_misc" title="Icon chooser" style="font-size:14px">Choose from list</a><br/>
</td></tr></table>
<?php
if(0){
?>
 <table>
 <!-- iconos -->
 <tr><td class=ne1 colspan=2>
	<table><tr>
	<?
		$ico_std = explode("\n",`ls -1 'pixmaps/standard'`);
		$i=0;
		foreach ($ico_std as $ico) if (trim($ico)!="") {
		        if(is_dir("pixmaps/standard/" . $ico) || !getimagesize("pixmaps/standard/" . $ico)){ continue;}
			echo "<td><img src='pixmaps/standard/$ico' border=0></td><td align=center><input type=radio name=icon value='pixmaps/standard/$ico'".(($i==0) ? " checked" : "")."></td>";
			$i++; if ($i % 6 == 0) echo "</tr><tr>";
		}
		$ico_std = explode("\n",`ls -1 'pixmaps/uploaded'`);
		foreach ($ico_std as $ico) if (trim($ico)!="") {
		        if(is_dir("pixmaps/uploaded/" . $ico) || !getimagesize("pixmaps/uploaded/" . $ico)){ continue;}
			echo "<td><img src='pixmaps/uploaded/$ico' border=0></td><td align=center><input type=radio name=icon value='pixmaps/uploaded/$ico'><br><a href='$SCRIPT_NAME?map=$map&delete_type=icon&delete=".urlencode("$ico")."'><img src='images/delete.png' border=0></a><a href=\"pixmaps/uploaded/$ico\" rel=\"lytebox[test]\" title=\"&lt;a href=&apos;javascript:alert(&quot;placeholder&quot;);&apos;&gt;Click HERE!&lt;/a&gt;\">AAAAA</a></td>";
			$i++; if ($i % 6 == 0) echo "</tr><tr>";
		}		
	?>
	</tr></table>
 </td></tr>
 <?
 } // end if(0)
 ?>
 <!-- types -->
 <br>
 <table width="100%">
 <tr>
 <td class=ne1>
	<table width="100%" border="0" class="noborder">
	<tr>
		<td class=ne11> <?= _("Type") ?> </td>
		<td>
			<select name="type" onchange="view()"><? foreach ($types as $type_class) echo "<option value=\"$type_class\" ".(($type_class==$type) ? "selected" : "").">$type_class"; ?></select>
			<select name="elem"><? foreach ($data_types[$type] as $name) echo "<option value='$name'>$name"; ?></select>
		</td>
	</tr>
	<tr>
		<td class=ne11> <?= _("Indicator Name"); ?> </td>
		<td><input type=text size=30 name="alarm_name" class=ne1></td>
	</tr>
	<tr>
		<td class=ne11> <?= _("URL"); ?> </td>
		<td><input type=text size=30 name="url" class=ne1></td>
	</tr>
	<tr>
		<td class=ne1><i> <?= _("Choose map to link") ?> </i></td>
		<td><table><tr><? echo $linkmaps ?></tr></table></td>
	</tr>
	
	<tr>
		<td colspan="2"><input type=button value="<?= _("New Indicator") ?>" onclick="addnew('<? echo $map ?>','alarm')" class="btn" style="font-size:12px"> <input type=button value="<?= _("New Rect") ?>" onclick="addnew('<? echo $map ?>','rect')" class="btn" style="font-size:12px"> <input type=button value="<?= _("Save Changes") ?>" onclick="save('<? echo $map ?>')" class="btn" style="font-size:12px"></td>
	</tr>	
	</table>
 </td>
<tr><td id="tdnuevo"></td></tr>
</tr></table>
<?
	$query = "select * from risk_indicators where name <> 'rect' AND map= ?";
	$params = array($map);
        if (!$rs = &$conn->Execute($query, $params)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
               echo "<input type=\"hidden\" name=\"dataname".$rs->fields["id"]."\" id=\"dataname".$rs->fields["id"]."\" value=\"".$rs->fields["name"]."\">\n";
               echo "<input type=\"hidden\" name=\"datanurl".$rs->fields["id"]."\" id=\"dataurl".$rs->fields["id"]."\" value=\"".$rs->fields["url"]."\">\n";
               echo "<input type=\"hidden\" name=\"dataicon".$rs->fields["id"]."\" id=\"dataicon".$rs->fields["id"]."\" value=\"".$rs->fields["icon"]."\">\n";
		echo "<div id=\"alarma".$rs->fields["id"]."\" class=\"itcanbemoved\" style=\"visibility:hidden;left:".$rs->fields["x"]."px;top:".$rs->fields["y"]."px;height:".$rs->fields["h"]."px;width:".$rs->fields["w"]."px\">";
		echo "<table border=0 cellspacing=0 cellpadding=1><tr><td colspan=2 class=ne align=center><i>".$rs->fields["name"]."</i></td></tr><tr><td><img src=\"".$rs->fields["icon"]."\" border=0></td><td>";
		echo "<table border=0 cellspacing=0 cellpadding=1><tr><td class=ne11>R</<td><td class=ne11>V</<td><td class=ne11>A</<td></tr><tr><td><img src='images/b.gif' border=0></td><td><img src='images/b.gif' border=0></td><td><img src='images/b.gif' border=0></td></tr></table>";
		echo "</td></tr></table></div>\n";
                $rs->MoveNext();
	}
	}
	$query = "select * from risk_indicators where name='rect' AND map = ?";
	$params = array($map);

        if (!$rs = &$conn->Execute($query, $params)) {            
		print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
               echo "<input type=\"hidden\" name=\"dataname".$rs->fields["id"]."\" id=\"dataname".$rs->fields["id"]."\" value=\"".$rs->fields["name"]."\">\n";
               echo "<input type=\"hidden\" name=\"datanurl".$rs->fields["id"]."\" id=\"dataurl".$rs->fields["id"]."\" value=\"".$rs->fields["url"]."\">\n";
		echo "<div id=\"rect".$rs->fields["id"]."\" class=\"itcanbemoved\" style=\"visibility:visible;left:".$rs->fields["x"]."px;top:".$rs->fields["y"]."px;height:".$rs->fields["h"]."px;width:".$rs->fields["w"]."px\">";
		echo "<table border=0 cellspacing=0 cellpadding=0 width=\"100%\" height=\"100%\"><tr><td style=\"border:1px dotted black\">&nbsp;</td></tr></table>";
		echo "</div>\n";
                $rs->MoveNext();
		}
	}
	
	$conn->close();
?>
<?php

$uploaded_dir = "pixmaps/uploaded/";
$uploaded_link = "pixmaps/uploaded/";

$icons = explode("\n",`ls -1 '$uploaded_dir'`);
print "<div style=\"display:none;\">";
$i = 0;
foreach($icons as $ico){
if(!$ico)continue;
if(is_dir($uploaded_dir . "/" .  $ico) || !getimagesize($uploaded_dir . "/" . $ico)){ continue;}
print "<a href=\"$uploaded_link/$ico\" id=\"custom-$i\" rel=\"lytebox[custom]\" title=\"&lt;a href=&apos;javascript:choose_icon(&quot;$uploaded_link/$ico&quot;);&apos;&gt;" . htmlspecialchars(_("Choose this one")) . ".&lt;/a&gt;\">custom</a>";
$i++;
}


$standard_dir = "pixmaps/standard/";
$standard_link = "pixmaps/standard/";

$icons = explode("\n",`ls -1 '$standard_dir'`);
print "<div style=\"display:none;\">";
$i = 0;
foreach($icons as $ico){
if(!$ico)continue;
if(is_dir($standard_dir . "/" . $ico) || !getimagesize($standard_dir . "/" . $ico)){ continue;}
print "<a href=\"$standard_link/$ico\" id=\"standard-$i\" rel=\"lytebox[standard]\" title=\"&lt;a href=&apos;javascript:choose_icon(&quot;$standard_link/$ico&quot;);&apos;&gt;" . htmlspecialchars(_("Choose this one")) . ".&lt;/a&gt;\">standard</a>";
$i++;
}

print "</div>\n";

/*
$docroot = "/var/www/";
$resolution = "128x128";
$icon_cats = explode("\n",`ls -1 '$docroot/ossim_icons/Regular/'`);
foreach($icon_cats as $ico_cat){
if(!$ico_cat)continue;
$icons = explode("\n",`ls -1 '$docroot/ossim_icons/Regular/$ico_cat/$resolution/'`);
print "<div style=\"display:none;\">";
$i = 0;
foreach($icons as $ico){
if(is_dir("$docroot/ossim_icons/Regular/$ico_cat/$resolution/$ico") || !getimagesize("$docroot/ossim_icons/Regular/$ico_cat/$resolution/$ico")){ continue;}
if(!$ico)continue;
print "<a href=\"/ossim_icons/Regular/$ico_cat/$resolution/$ico\" id=\"$ico_cat-$i\" rel=\"lytebox[$ico_cat]\" title=\"&lt;a href=&apos;javascript:choose_icon(&quot;/ossim_icons/Regular/$ico_cat/RESOLUTION/$ico&quot;);&apos;&gt;Choose this one.&lt;/a&gt;\">$ico_cat</a>";
$i++;
}
print "</div>\n";
}
*/
?>


</form>
</td>
<td width="48" valign=top>
	<img src='images/wastebin.gif' id="wastebin" border=0>
</td>
<td valign=top id="map">
	<img src="maps/map<? echo $map ?>.jpg" id="map_img" border=0>
</td>

</tr>
</table>
<script>

function checkSaved(){
// Disable, this seems to break something
if(changed){
//(if(0){
var x=window.confirm("<?= _("Unsaved changes, want to save them before exiting?"); ?>");
if(x){
save('<? echo $map ?>');
return true;
} else {
return true;
}
}
}
</script>
</body>
</html>
