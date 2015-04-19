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
require_once 'classes/Security.inc';
require_once 'ossim_db.inc';

$db = new ossim_db();
$conn = $db->connect();
$map = ($_SESSION["riskmap"]!="") ? $_SESSION["riskmap"] : 1;
?>
<html>
<head>
<title><?= _("Alarms") ?> - <?= _("View")?></title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<link rel="stylesheet" type="text/css" href="./custom_style.css">
<style type="text/css">
	.itcanbemoved { position:absolute; }
</style>
</head>
<body leftmargin=5 topmargin=5 class="ne1">
 <?
	$maps = explode("\n",`ls -1 'maps' | grep -v CVS`);
	$i=0; $n=0; $txtmaps = ""; $linkmaps = "";
	foreach ($maps as $ico) if (trim($ico)!="") {
	        if(!getimagesize("maps/" . $ico)){ continue;}
		$n = str_replace("map","",str_replace(".jpg","",$ico));
		$txtmaps .= "<td><a href='view.php?map=$n'><img src='maps/$ico' border=".(($map==$n) ? "2" : "0")." width=150 height=150></a></td>";
		$i++; if ($i % 5 == 0) {
			$txtmaps .= "</tr><tr>";
		}
	}
 ?> 
 <table align=center><tr><? echo $txtmaps ?></tr></table>
</body>
</html>
