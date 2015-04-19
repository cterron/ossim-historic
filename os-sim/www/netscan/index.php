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
// Get a list of nets from db
require_once ("ossim_db.inc");
$db = new ossim_db();
$conn = $db->connect();
require_once ("classes/Net.inc");
$net_list = Net::get_list($conn);
$db->close($conn);
require_once 'ossim_conf.inc';
$conf = $GLOBALS["CONF"];
$nmap_path = $conf->get_conf("nmap_path");
if (file_exists($nmap_path)) {
    $nmap_exists = 1;
} else {
    $nmap_exists = 0;
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
<head>
  <title> <?php
echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>

  <script>
    // enable text input when manual option is selected
    function check_change() {
        form = document.forms['net_form'];
        if (form.net.value != '')
            form.net_input.disabled = true;
        else
            form.net_input.disabled = false;
        form.net_input.value = form.net.value;
    }
  </script>
  
</head>

<body>

<?php
include ("../hmenu.php");
if (!$nmap_exists) {
    require_once ("ossim_error.inc");
    $error = new OssimError();
    $error->display("NMAP_PATH");
}
?>
  <!-- net selector form -->
  <form name="net_form" method="GET" action="do_scan.php">
  <table align="center">
    <tr>
      <td colspan="3">
        <?php
echo gettext("Please, select the network you want to scan:") ?>
      </td>
    </tr>
    <tr>
      <td>
        <select name="net" onChange="javascript:check_change()">
<?php
if (is_array($net_list) && !empty($net_list)) {
    $first_net = $net_list[0]->get_ips();
    foreach($net_list as $net) {
?>
          <option name="<?php
        echo $net->get_name() ?>" 
                  value="<?php
        echo $net->get_ips() ?>">
            <?php
        echo $net->get_name() ?>
          </option>
<?php
    }
}
?>
          <option name="manual" selected value="">Manual</option>
        </select>
      </td>
      <td><input type="text" value=""
                 name="net_input" enabled /></td>
      <td><input type="submit" class="btn" style="font-size:12px" value="<?php
echo gettext("Scan") ?>" <?php echo (!$nmap_exists) ? "disabled" : "" ?> /></td>
    </tr>
    <tr>
    <td colspan="3">
    <center>
    <input type="checkbox" name="full_scan" value="full_scan"><?php echo _("Enable full scan.") . "<br/><small>" . _("Will be much slower but will include OS, services, service versions and MAC address into the inventory") . "</small>" ?>
    </center>
    </td>
    </tr>
  </table>
  </form>
  <!-- end of net selector form -->

<?php
require_once ('classes/Scan.inc');
if (GET('clearscan')) {
    Scan::del_scan();
}
$scan = Scan::get_scan();
if (is_array($scan)) {
    require_once ('scan_util.php');
    scan2html($scan);
} else {
    echo "<p align=\"center\">";
    echo gettext("NOTE: This tool is a nmap frontend. In order to use all nmap functionality, you need root privileges.");
    echo "<br/>";
    echo gettext("For this purpose you can use suphp, or set suid to nmap binary (chmod 4750 /usr/bin/nmap) changing the group to the one of the web-user (chgrp www-data /usr/bin/nmap).");
    echo "</p>";
}
?>


</body>
</html>
