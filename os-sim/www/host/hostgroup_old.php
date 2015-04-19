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
Session::logcheck("MenuPolicy", "PolicyHosts");
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

  <h1> <?php
echo gettext("Host groups"); ?> </h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Host_group.inc';
require_once 'classes/Host_group_scan.inc';
require_once 'classes/Host_group_reference.inc';
require_once 'classes/Plugin.inc';
require_once 'classes/Security.inc';
require_once ("classes/Repository.inc");
$nessus_action = GET('nessus');
$nagios = GET('nagios');
$host_group_name = GET('host_group_name');
$order = GET('order');
ossim_valid($nessus_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("Nessus action"));
ossim_valid($nagios, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("Nagios action"));
ossim_valid($host_group_name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:' . _("Host group name"));
ossim_valid($order, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:' . _("Order"));
if (ossim_error()) {
    die(ossim_error());
}
$db = new ossim_db();
$conn = $db->connect();
if ((!empty($nessus_action)) AND (!empty($host_group_name))) {
    if ($nessus_action == "enable") {
        Host_group::enable_nessus($conn, $host_group_name);
    } elseif ($nessus_action = "disable") {
        Host_group::disable_nessus($conn, $host_group_name);
    }
    $db->close($conn);
}
$hosts_list = Host_group_reference::get_list($conn, $host_group_name);
$iter = 0;
foreach($hosts_list as $host) $hosts[$iter++] = $host->host_ip;
if ($nagios == "disable") {
    if (Host_group_scan::in_host_group_scan($conn, $host_group_name, 2007)) {
        foreach($hosts as $h) {
            if (Host_group_scan::can_delete_host_from_nagios($conn, $h, $host_group_name)) {
                require_once 'classes/NagiosConfigs.inc';
                $q = new NagiosAdm();
                $q->delHost(new NagiosHost($h, $h, ""));
                $q->close();
            }
        }
        Host_group_scan::delete($conn, $host_group_name, 2007);
    }
}
if ($nagios == "enable") {
    if (Host_group_scan::in_host_group_scan($conn, $host_group_name, 2007)) Host_group_scan::delete($conn, $host_group_name, 2007);
    Host_group_scan::insert($conn, $host_group_name, 2007);
    require_once 'classes/NagiosConfigs.inc';
    $q = new NagiosAdm();
    $q->addNagiosHostGroup(new NagiosHostGroup($host_group_name, $hosts, $sensors));
    $q->close();
}
if (empty($order)) $order = "name";
?>

  <table class="nobborder" align="center"><tr><td class="nobborder" valign="top">
  
  <table align="center">
    <tr>
      <th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("name", $order);
?>"> <?php
echo gettext("Host Group"); ?> </a></th>
      <th> <?php
echo gettext("Hosts"); ?> </th>
      <th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("threshold_c", $order);
?>"> <?php
echo gettext("Threshold_C"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("threshold_a", $order);
?>"> <?php
echo gettext("Threshold_A"); ?> </a></th>
      <th><a href="<?php
echo $_SERVER["PHP_SELF"] ?>?order=<?php
echo ossim_db::get_order("rrd_profile", $order);
?>"> <?php
echo gettext("RRD Profile"); ?> </a></th>
      <th> <?php
echo gettext("Scan Types"); ?> </th>
      <th> <?php
echo gettext("Sensors"); ?> </th>
      <th> <?php
echo gettext("Description"); ?> </th>
      <th> <?php
echo gettext("Knowledge DB"); ?> </th>
      <th> <?php
echo gettext("Action"); ?> </th>
    </tr>

<?php
if ($host_group_list = Host_group::get_list($conn, "ORDER BY $order")) {
    foreach($host_group_list as $host_group) {
        $name = $host_group->get_name();
?>

    <tr <?php
        if (GET('group') == $name) echo " bgcolor='#eeeeee'" ?>>
      <td><?php echo $name ?></td>
      <td align="left">
      <?php
        if ($host_list = $host_group->get_hosts($conn)) {
            foreach($host_list as $host) {
                echo $host->get_host_name($conn) . '<br/>';
            }
        } else {
            echo "&nbsp;";
        }
?>
      </td>

      <td><?php
        echo $host_group->get_threshold_c(); ?></td>
      <td><?php
        echo $host_group->get_threshold_a(); ?></td>
      <td>
        <?php
        if (!($rrd_profile = $host_group->get_rrd_profile())) echo "None";
        else echo $rrd_profile;
?>
      </td>
      <td>
      <?php
        if ($scan_list = Host_group_scan::get_list($conn, "WHERE host_group_name = '$name' AND plugin_id = 3001")) {
            $name = stripslashes($name);
            echo "<a href=\"" . $_SERVER["PHP_SELF"] . "?nessus=disable&host_group_name=$name\">Nessus ENABLED</a>";
        } else {
            $name = stripslashes($name);
            echo "<a href=\"" . $_SERVER["PHP_SELF"] . "?nessus=enable&host_group_name=$name\">Nessus DISABLED</a>";
        }
?>
      <br/>
      <?php
        if ($scan_list = Host_group_scan::get_list($conn, "WHERE host_group_name = '$name' AND plugin_id = 2007")) {
            $name = stripslashes($name);
            echo "<a href=\"" . $_SERVER["PHP_SELF"] . "?nagios=disable&host_group_name=$name\">Nagios ENABLED</a>";
        } else {
            $name = stripslashes($name);
            echo "<a href=\"" . $_SERVER["PHP_SELF"] . "?nagios=enable&host_group_name=$name\">Nagios DISABLED</a>";
        }
?>
      </td>

      <td><?php
        if ($sensor_list = $host_group->get_sensors($conn)) {
            foreach($sensor_list as $sensor) {
                echo $sensor->get_sensor_name() . '<br/>';
            }
        }
?>    </td>
      <td><?php
        echo $host_group->get_descr(); ?>&nbsp;</td>
	  <td><?php
        if (Repository::have_linked_documents($conn, $host_group->get_name() , 'host_group')) { ?><a href="<?php echo $_SERVER["PHP_SELF"] ?>?group=<?php echo urlencode($name) ?>"><img src="../repository/images/icon_search.gif" border=0></a><?php
        } ?>&nbsp;</td>
      <td><a href="modifyhostgroupform.php?name=<?php
        echo $name ?>"> <?php
        echo gettext("Modify"); ?> </a>
          <a href="deletehostgroup.php?name=<?php
        echo $name ?>"> <?php
        echo gettext("Delete"); ?> </a></td>
    </tr>

<?php
    } /* host_list */
} /* foreach */
?>
    <tr>
      <td colspan="11"><a href="newhostgroupform.php"> <?php
echo gettext("Insert new host group"); ?> </a></td>
    </tr>
  </table>
  
  </td>
  <td class="nobborder" valign="top">
		<?php
if (GET('group') != "") {
    $keyname = GET('group');
    $type = 'host_group';
    include ("../repository/repository_host.php");
}
?>
  </td>
  </tr>
  </table>
  

</body>
</html>

<?php
$db->close($conn);
?>
