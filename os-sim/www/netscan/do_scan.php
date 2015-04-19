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
?>

<html>
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php
require_once 'classes/Security.inc';
$net = GET('net');
$full_scan = GET('full_scan');
$net_input = GET('net_input');
ossim_valid($net, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("Net"));
ossim_valid($net_input, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("Net"));
if (ossim_error()) {
    die(ossim_error());
}
if (empty($net)) $net = $net_input;
require_once ('classes/Scan.inc');
$scan = new Scan($net);
echo gettext("Scanning network") . " ($net), " . gettext("please wait") . "..<br/>";
flush();
if ($full_scan) {
    $scan->do_scan(TRUE);
} else {
    $scan->do_scan(FALSE);
}
echo gettext("Scan completed") . ".<br/><br/>";
echo "<a href=\"index.php\">" . gettext("Click here to show the results") . "</a>";
$scan->save_scan();
?>

</body>
</html>


