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
/*******************************************************************************
** Copyright (C) 2008 Alienvault
********************************************************************************
** Authors:
********************************************************************************
** Jaime Blasci <jaime.blasco@alienvault.com>
**
********************************************************************************
*/
include ("base_conf.php");
include ("$BASE_path/includes/base_constants.inc.php");
include ("$BASE_path/includes/base_include.inc.php");
?>
<html>
<head>
<TITLE>Forensics Console : Alert</TITLE><LINK rel="stylesheet" type="text/css" HREF="styles/ossim_style.css">
</head>
<body>
<div class="mainheadertitle">&nbsp;Shellcode Analysis </div>
<?php
// Check role out and redirect if needed -- Kevin
$roleneeded = 10000;
$BUser = new BaseUser();
if (($BUser->hasRole($roleneeded) == 0) && ($Use_Auth_System == 1)) {
    base_header("Location: " . $BASE_urlpath . "/index.php");
    exit();
}
$file = $_GET['file'];
echo '<embed src="' . $file . '" type="image/svg+xml"
 pluginspage="http://www.adobe.com/svg/viewer/install/" 
 style="border: 1px solid black; padding:5px;"/>';
?>
