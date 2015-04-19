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
require_once ('classes/CIDR.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <!--<link rel="stylesheet" type="text/css" href="../style/style.css"/>-->
  <link rel="stylesheet" type="text/css" href="../style/tables.css"/>
  <link rel="stylesheet" type="text/css" href="../style/tables_paginate.css"/>
  <script type="text/javascript" src="../js/tablesort.js"></script>
  <script type="text/javascript" src="../js/paginate.js"></script>
  <script type="text/javascript" src="../js/filter.js"></script>
</head>
<body>

  <h1> <?php echo gettext("Hosts"); ?> </h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Host.inc';
require_once 'classes/Host_os.inc';
require_once 'classes/Host_scan.inc';
require_once 'classes/Plugin.inc';
require_once 'classes/CIDR.inc';
require_once 'classes/Security.inc';
require_once 'classes/WebIndicator.inc';
require_once ("classes/Repository.inc");
$order = GET('order');
$search = GET('search');
if (empty($search)) $search = POST('search');
$lsearch = $search;
if (!empty($search))
// The CIDR validation is not working...
if (preg_match("/^\s*([0-9]{1,3}\.){3}[0-9]{1,3}\/(3[0-2]|[1-2][0-9]|[0-9])\s*$/", $search)) {
    $ip_range = CIDR::expand_CIDR($search, "SHORT", "IP");
    ossim_valid($ip_range[0], OSS_IP_ADDR, 'illegal:' . _("search cidr"));
    ossim_valid($ip_range[1], OSS_IP_ADDR, 'illegal:' . _("search cidr"));
} else if (preg_match("/^\s*([0-9]{1,3}\.){3}[0-9]{1,3}\s*$/", $search)) $by_ip = true;
else ossim_valid($search, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_DOT, OSS_DIGIT, 'illegal:' . _("search"));
ossim_valid($order, "()", OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_DIGIT, 'illegal:' . _("order"));
if (ossim_error()) {
    die(ossim_error());
}
if (empty($order)) $order = "hostname";
if (!empty($ip_range)) $search = 'WHERE inet_aton(ip) >= inet_aton("' . $ip_range[0] . '") and inet_aton(ip) <= inet_aton("' . $ip_range[1] . '")';
else if (!empty($by_ip)) $search = "WHERE ip like '%$search%'";
else if (!empty($search)) $search = "WHERE ip like '%$search%' OR hostname like '%$search%'";
?>
<table align="left" class="noborder">
<tr>
<td class="nobborder" valign="top">
  <table cellpadding="0" cellspacing="0" border="0" class="noborder">
	<tr>
		<td class="nobborder" valign="top">
			<table class="noborder" align="left">
				<tr>
					
					<!-- Actions -->
					<td class="nobborder" valign="top">
						<table>
							<tr><th>Actions</th></tr>
							<tr>
						      <td><a href="newhostform.php">
						      <?php echo gettext("Insert new host"); ?> </a></td>
							  <td class="nobborder"></td>
						    </tr>
						    <tr>
						      <td><a href="../conf/reload.php?what=hosts&back=<?php echo urlencode($_SERVER["REQUEST_URI"]); ?>"> <?php
if (WebIndicator::is_on("Reload_hosts")) {
    echo "<font color=red>&gt;&gt;&gt; " . gettext("Reload") . " &lt;&lt;&lt;</color>";
} else {
    echo gettext("Reload");
} ?> </a></td>
							  <td class="nobborder"></td>
						    </tr>
						</table>
					</td>
					
					<!-- Filters -->
					<td class="nobborder" valign="top">
						<form name="ffilter">
						<table>
							<tr><th colspan="4" id="num_filter">Filters</th></tr>
							<tr>
							  <td>Hostname</td>
							  <td>IP</td>
							  <td>Sensor</td>
							  <td></td>
							</tr>
							<tr>
							  <td><input type="text" name="hostname_filter"></td>
							  <td><input type="text" name="ip_filter" style="width:100px"></td>
							  <td><input type="text" name="sensor_filter"></td>
							  <td><input type="button" class="blue" value="Apply" onclick="applyFilter(document.ffilter.hostname_filter.value,document.ffilter.ip_filter.value,document.ffilter.sensor_filter.value)"></td>
							</tr>
						</table>
						</form>
					</td>
					
					<!-- Loading -->
					<td class="nobborder" id="loading_td" nowrap>
						<img src="../pixmaps/ajax-loader.gif" align="absmiddle"> Loading
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
	<td class="nobborder">
	
	<div id="tablediv" style="visibility:hidden;display:none;width:100%">
	<table cellpadding="0" cellspacing="0" id="theTable" style="width:800px;align:left" class="sortable-onload-0r no-arrow colstyle-alt rowstyle-alt paginate-10 max-pages-7 paginationcallback-callbackTest-calculateTotalRating sortcompletecallback-callbackTest-calculateTotalRating">
	
	<tr>
	  <th class="sortable-text"><?php echo gettext("Hostname"); ?></th>
      <th class="sortable-text"><?php echo gettext("Ip") ?></th>
      <th class="sortable-numeric"><?php echo gettext("Asst") ?></th>
      <th> <?php echo gettext("Sensors"); ?> </th>
      <th> <?php echo gettext("Scantype"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
	  <th> <?php echo gettext("Knowledge DB"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php
$db = new ossim_db();
$conn = $db->connect();
if ($host_list = Host::get_list($conn, "$search", "ORDER BY $order")) {
    foreach($host_list as $host) {
        $ip = $host->get_ip();
?>

    <tr>
      <td><a href="../report/index.php?host=<?php echo $ip
?>"><?php echo $host->get_hostname(); ?></a>
      <?php echo Host_os::get_os_pixmap($conn, $host->get_ip()); ?>
      </td>
      <td><?php echo $host->get_ip(); ?></td>
      <td align="center"><?php echo $host->get_asset(); ?></td>
      <!-- sensors -->
      <td><?php
        if ($sensor_list = $host->get_sensors($conn)) {
            foreach($sensor_list as $sensor) {
                echo $sensor->get_sensor_name() . '<br/>';
            }
        }
?>    </td>
    <td>
<?php
        if ($scan_list = Host_scan::get_list($conn, "WHERE host_ip = inet_aton('$ip')")) {
            foreach($scan_list as $scan) {
                $id = $scan->get_plugin_id();
                $plugin_name = "";
                if ($plugin_list = Plugin::get_list($conn, "WHERE id = $id")) {
                    $plugin_name = $plugin_list[0]->get_name();
                    echo ucfirst($plugin_name) . "<BR>";
                } else {
                    echo "$id<BR>";
                }
            }
        } else {
            echo gettext("None");
        }
?>
    </td>
      <td><?php echo $host->get_descr(); ?>&nbsp;</td>
	  <td class="nohover" align="center">
		<?php
        if ($linkedocs = Repository::have_linked_documents($conn, $host->get_ip() , 'host')) { ?><a href="../report/index.php?host=<?php echo $ip ?>" class="blue">[<?php echo $linkedocs ?>]</a>&nbsp;<?php
        } ?>
		<a href="addrepository.php?id_host=<?php echo $ip
?>&name_host=<?php echo $host->get_hostname() ?>" target="addcontent" onclick="insert_ip_filter('<?php echo $ip ?>')" style="hover{border-bottom:0px}"><img src="../repository/images/edit.gif" border=0 align="absmiddle"></a>
		<a href="../repository/index.php" style="hover{border-bottom:0px}"><img src="../repository/images/editdocu.gif" border=0 align="absmiddle"></a>
	  </td>
      <td nowrap>
          <a href="modifyhostform.php?ip=<?php echo $ip ?>"> <?php echo gettext("Modify"); ?> </a> | 
          <a href="deletehost.php?ip=<?php echo $ip ?>"> <?php echo gettext("Delete"); ?> </a>
      </td>
    </tr>

<?php
    } /* host_list */
} /* foreach */
$db->close($conn);
?>
	</table>
	</div>
	</td>
	</tr>
	</table>
	</td><tr>
	
	<tr><td valign="top" class="noborder" style="padding-top:10px">
		<IFRAME src="" frameborder="0" name="addcontent" id="addcontent"></IFRAME>
	</td></tr>
	
	</tr>
  </table>
</td>
</tr>
</table>

</body>
</html>

