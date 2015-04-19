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
require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
if (GET('withoutmenu') != "1") include ("../hmenu.php");
?>
<table width="100%"><tr><td align="center">
<!-- <tr>
<td width="100%">
	<div class="greenheader">
		<table border=0 cellpadding=0 cellspacing=0 align="center">
		<tr><td class="whiteheader">Forensics Console</td></tr>
		</table>
	</div>
</td> -->
	<table><tr>
	<td>
		<div id="plotareaglobal" class="plot" style="text-align:center;margin:0px 15px 0px 0px;display:none;"></div>
	</td>
	<td style="padding:0px 0px 10px 20px">
		<form style="margin:0px" action="../control_panel/event_panel.php" method="get">
		<input type="hidden" name="hmenu" value="Forensics"><input type="hidden" name="smenu" value="RT Events">
		<input type="submit" class="button" value="Real Time" name="submit" style="font-weight:bold">
		</form>
	</td>
	</tr>
	</table>
</td>
</tr>
</table>
