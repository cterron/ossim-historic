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
require_once ('classes/Session.inc');
Session::logcheck("MenuEvents", "EventsVulnerabilities");
?>

<html>
<head>
  <title> <?php
echo gettext("Vulnmeter"); ?> </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

<?php
require_once ('ossim_conf.inc');
require_once ('ossim_sql.inc');
require_once ('classes/Security.inc');
require_once ('classes/Util.inc');
$action = REQUEST('action');
$scan_date = REQUEST('scan_date');
ossim_valid($scan_date, OSS_ALPHA, OSS_PUNC, 'illegal:' . _("Scan date"));
ossim_valid($action, OSS_ALPHA, 'illegal:' . _("Action"));
if (ossim_error()) {
    die(ossim_error());
}
switch ($action) {
    case 'delete':
        print _("Deleting") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;

    case 'archive':
        print _("Archiving") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;

    case 'restore':
        print _("Restoring") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;

    default:
        require_once ("ossim_error.inc");
        $error = new OssimError();
        $error->display("UNK_ACTION");
}
$conf = $GLOBALS["CONF"];
$address = $conf->get_conf("frameworkd_address");
$port = $conf->get_conf("frameworkd_port");
/* create socket */
$socket = socket_create(AF_INET, SOCK_STREAM, 0);
if ($socket < 0) {
    require_once ("ossim_error.inc");
    $error = new OssimError();
    $error->display("CRE_SOCKET", array(
        socket_strerror($socket)
    ));
}
/* connect */
$result = @socket_connect($socket, $address, $port);
if (!$result) {
    require_once ("ossim_error.inc");
    $error = new OssimError();
    $error->display("FRAMW_NOTRUN", array(
        $address . ":" . $port
    ));
}
$in = 'nessus action="' . $action . '" report="' . $scan_date . '"' . "\n";
$out = '';
socket_write($socket, $in, strlen($in));
$pattern = '/nessus \w+ ack ([^\s]*)/ ';
while ($out = socket_read($socket, 255, PHP_BINARY_READ)) {
    if (preg_match($pattern, $out, $regs)) {
        print gettext("Successfully") . " " . gettext($action . "d") . " " . Util::timestamp2date($regs[1]);
        print "<br><a href=\"index.php\"> " . gettext("Back") . " </a>";
        socket_close($socket);
        exit();
    }
}
socket_close($socket);
?>
