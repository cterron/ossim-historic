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
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuReports", "ReportsHostReport");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
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
include ("../hmenu.php");
require_once 'ossim_db.inc';
require_once 'classes/Host.inc';
require_once 'classes/Host_os.inc';
$order = GET('order') ? GET('order') : 'hostname';
if (!ossim_valid($order, OSS_ALPHA . OSS_SPACE . OSS_PUNC, 'illegal:' . _("Order"))) {
    die(ossim_error());
}
?>

  <table align="center">
    <tr>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("hostname", $order);
?>">
	  <?php
echo gettext("Hostname"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("inet_aton(ip)", $order);
?>">
	  <?php
echo gettext("Ip"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["SCRIPT_NAME"] ?>?order=<?php
echo ossim_db::get_order("asset", $order);
?>">
	  <?php
echo gettext("Asset"); ?> </a></th>
      <th> <?php
echo gettext("OS"); ?> </th>
    </tr>

<?php
$db = new ossim_db();
$conn = $db->connect();
$host_list = Host::get_list($conn, '', "ORDER BY $order");
foreach($host_list as $host) {
    $ip = $host->get_ip();
?>
    <tr>
      <td><a href="../report/index.php?host=<?php echo $ip ?>"><?php echo $host->get_hostname(); ?></a></td>
      <td><?php echo $host->get_ip(); ?></td>
      <td><?php echo $host->get_asset(); ?></td>
      <td>
        <?php
    if ($os_data = Host_os::get_ip_data($conn, $host->get_ip())) {
        $os = $os_data["os"];
        echo $os . " " . Host_os::get_os_pixmap_nodb($os);
    }
?>&nbsp;
      </td>
    </tr>
<?php
} /* foreach */
?>
  </table>
    
</body>
</html>

