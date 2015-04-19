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
  <link rel="stylesheet" type="text/css" href="../style/tree.css" />
  <script type="text/javascript" src="../js/jquery-1.3.2.min.js"></script>
  <script type="text/javascript" src="../js/jquery-ui-1.7.custom.min.js"></script>
  <script type="text/javascript" src="../js/jquery.cookie.js"></script>
  <script type="text/javascript" src="../js/jquery.dynatree.js"></script>
  <script type="text/javascript" src="../js/urlencode.js"></script>
  <script type="text/javascript" src="../js/combos.js"></script>
  <script type="text/javascript">
	//var loading = '<br><img src="../pixmaps/theme/ltWait.gif" border="0" align="absmiddle"> Loading tree...';
	var layer = null;
	var nodetree = null;
	var i=1;
	function load_tree(filter) {
		combo = 'hosts';
		if (nodetree!=null) {
			nodetree.removeChildren();
			$(layer).remove();
		}
		layer = '#srctree'+i;
		$('#container').append('<div id="srctree'+i+'" style="width:100%"></div>');
		$(layer).dynatree({
			initAjax: { url: "draw_tree.php", data: {filter: filter} },
			clickFolderMode: 2,
			onActivate: function(dtnode) {
				if (!dtnode.hasChildren()) {
					addto(combo,dtnode.data.url,dtnode.data.url)
				} else {
					var children = dtnode.tree.getAllNodes(dtnode.data.key.replace('.','\\.')+'\\.');
					for (c=0;c<children.length; c++)
						addto(combo,children[c].data.url,children[c].data.url)
				}
			},
			onDeactivate: function(dtnode) {}
		});
		nodetree = $(layer).dynatree("getRoot");
		i=i+1

	}
	function submit_form(form) {
		selectall('hosts');
		form.submit();
	}
	$(function(){
		load_tree("");
	});
  </script>
</head>
<body>

<?php
if (GET('withoutmenu') != "1") include ("../hmenu.php"); ?>

<?php
require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_group.inc');
require_once ('classes/Host_group_scan.inc');
require_once ('classes/Host_sensor_reference.inc');
require_once ('classes/RRD_config.inc');
$db = new ossim_db();
$conn = $db->connect();
$conf = $GLOBALS["CONF"];
$threshold = $conf->get_conf("threshold");
$name = GET('name');
ossim_valid($name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:' . _("name"));
if (ossim_error()) {
    die(ossim_error());
}
$all = array();
$hg_name = $hg_desc = $hg_thra = $hg_thrc = $nessus = $nagios = "";
$host_list = $hg_sensors = array();
if ($name != "") {
    if ($host_group_list = Host_group::get_list($conn, "WHERE name = '$name'")) {
        $host_group = $host_group_list[0];
        $hg_name = $host_group->get_name();
        $hg_desc = $host_group->get_descr();
        $hg_thrc = $host_group->get_threshold_c();
        $hg_thra = $host_group->get_threshold_a();
        $host_list = $host_group->get_hosts($conn);
        $nessus = ($scan_list = Host_group_scan::get_list($conn, "WHERE host_group_name = '$name' AND plugin_id = 3001")) ? "checked" : "";
        $nagios = ($scan_list = Host_group_scan::get_list($conn, "WHERE host_group_name = '$name' AND plugin_id = 2007")) ? "checked" : "";
        $rrd_profile = $host_group->get_rrd_profile();
        if (!$rrd_profile) $rrd_profile = "None";
        $tmp_sensors = $host_group->get_sensors($conn);
        foreach($tmp_sensors as $sensor) $hg_sensors[] = $sensor->get_sensor_name();
    }
}
?>

<form method="post" action="<?php echo ($name != "") ? "modifyhostgroup.php" : "newhostgroup.php" ?>">

<table align="center"><tr><td valign="top" class="nobborder">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php
echo gettext("Name"); ?> </th>
    <td class="left"><input type="text" name="name" size="30" value="<?php echo $hg_name ?>"></td>
  </tr>

  <tr>
    <th> <?php
echo gettext("Hosts"); ?> <br/>
        <font size="-2">
          <a href="newhostform.php">
			<?php
echo gettext("Insert new host"); ?> ?</a><br/>
        </font>
    </th>
	<td class="left nobborder">
		<select id="hosts" name="ips[]" size="20" multiple="multiple" style="width:250px">
		<?php
foreach($host_list as $host) {
    $ip = $host->get_host_ip($conn);
    echo "<option value='$ip'>$ip\n";
} ?>
		</select>
		<input type="button" value=" [X] " onclick="deletefrom('hosts')" class="btn">
	</td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold C"); ?> </th>
    <td class="left">
      <input type="text" name="threshold_c" size="4" value="<?php echo $hg_thrc ?>">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold A"); ?> </th>
    <td class="left">
      <input type="text" name="threshold_a" size="4" value="<?php echo $hg_thra ?>">
    </td>
  </tr>

  <tr>
    <th> <?php
echo gettext("Sensors"); ?> (*)<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php"><?php
echo gettext("Insert new sensor"); ?> ?</a>
        </font>
    </th>
    <td class="left">
<?php
/* ===== sensors ==== */
$i = 1;
if ($sensor_list = Sensor::get_list($conn, "ORDER BY name")) {
    foreach($sensor_list as $sensor) {
        $sensor_name = $sensor->get_name();
        $sensor_ip = $sensor->get_ip();
        if ($i == 1) {
?>
        <input type="hidden" name="<?php
            echo "nsens"; ?>"
            value="<?php
            echo count($sensor_list); ?>">
<?php
        }
        $sname = "sboxs" . $i;
?>
        <input type="checkbox" name="<?php
        echo $sname; ?>"
            value="<?php
        echo $sensor_name; ?>" <?php echo (in_array($sensor_name, $hg_sensors)) ? "checked" : "" ?>>
            <?php
        echo $sensor_ip . " (" . $sensor_name . ")<br>"; ?>
        </input>
<?php
        $i++;
    }
}
?>
    </td>
  </tr>

  <tr>
    <th> <?php
echo gettext("RRD Profile"); ?> <br/>
        <font size="-2">
          <a href="../rrd_conf/new_rrd_conf_form.php">
	  <?php
echo gettext("Insert new profile"); ?> ?</a>
        </font>
    </th>
    <td class="left">
      <select name="rrd_profile">
<?php
foreach(RRD_Config::get_profile_list($conn) as $profile) {
    if (strcmp($profile, "global")) {
        echo "<option value=\"$profile\"" . (($rrd_profile == $profile) ? " selected" : "") . ">$profile</option>\n";
    }
}
?>
        <option value="" selected>
	<?php
echo gettext("None"); ?> </option>
      </select>
    </td>

  </tr>
    <tr>
    <th> <?php
echo gettext("Scan options"); ?> </th>
    <td class="left">
        <input type="checkbox" name="nessus" value="1" <?php echo $nessus ?>>
        <?php
echo gettext("Enable nessus scan"); ?> </input><br/>
        <input type="checkbox" name="nagios" value="1" <?php echo $nagios ?>>
        <?php
echo gettext("Enable nagios scan"); ?> </input>
    </td>
  </tr>

  <tr>
    <th> <?php
echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr" rows="2" cols="30"><?php echo $hg_desc ?></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" class="nobborder" style="text-align:center;padding:10px">
      <input type="button" value="<?php echo ($name != "") ? "Modify" : "OK" ?>" class="btn" style="font-size:12px" onclick="submit_form(this.form)">
      <?php
if ($name == "") { ?><input type="reset" value="Reset" class="btn" style="font-size:12px"><?php
} ?>
    </td>
  </tr>
</table>
</td>
<td class="left nobborder" valign="top">
	Filter: <input type="text" id="filter" name="filter" size=20>&nbsp;<input type="button" value="Apply" onclick="load_tree(this.form.filter.value)" class="btn" style="font-size:12px">
	<div id="container" style="width:350px"></div>
</td>
</tr>
</table>
</form>
<?php
$db->close($conn);
?>

</body>
</html>

