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
Session::logcheck("MenuPolicy", "PolicyNetworks");
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
if (GET('withoutmenu') != "1") include ("../hmenu.php"); ?>

<?php
require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Net_sensor_reference.inc');
require_once ('classes/RRD_config.inc');
$db = new ossim_db();
$conn = $db->connect();
$conf = $GLOBALS["CONF"];
$threshold = $conf->get_conf("threshold");
?>

<form method="post" action="newnet.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php
echo gettext("Name"); ?> (*)</th>
    <td class="left"><input type="text" name="name" size="30"></td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Ips"); ?> (*)</th>
    <td class="left">
       <i> <?php
echo gettext("example"); ?>: 192.168.0.0/24,192.168.1.0/24 </i><br/>
      <input type="text" name="ips" size="30">
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Asset"); ?> (*)</th>
    <td class="left">
      <select name="asset">
        <option value="0">0</option>
        <option value="1">1</option>
        <option selected value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
      </select>
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Threshold C"); ?> (*)</th>
    <td class="left">
      <input type="text" value="<?php
echo $threshold ?>" 
             name="threshold_c" size="4">
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("Threshold A"); ?> (*)</th>
    <td class="left">
      <input type="text" value="<?php
echo $threshold ?>" 
             name="threshold_a" size="4">
    </td>
  </tr>
  <tr>
    <th> <?php
echo gettext("RRD Profile"); ?> (*)<br/>
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
        echo "<option value=\"$profile\">$profile</option>\n";
    }
}
?>
        <option value="" selected>
	<?php
echo gettext("None"); ?> </option>
      </select>
    </td>
  </tr>
<!--
  <tr>
    <th>Alert</th>
    <td class="left">
      <select name="alert">
        <option value="1">Yes</option>
        <option selected value="0">No</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Persistence</th>
    <td class="left">
      <input type="text" name="persistence" value="15" size="3"></input>min.
    </td>
  </tr>
-->

  <tr>
    <th> <?php
echo gettext("Sensors"); ?> (*)<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">
	  <?php
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
        $name = "mboxs" . $i;
?>
        <input type="checkbox" name="<?php
        echo $name; ?>"
            value="<?php
        echo $sensor_name; ?>">
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
echo gettext("Scan options"); ?> </th>
    <td class="left">
        <input type="checkbox" name="nessus" value="1"> <?php
echo gettext("Enable nessus scan"); ?> </input><br>
        <input type="checkbox" name="nagios" value="1"> <?php
echo gettext("Enable nagios"); ?> </input>
    </td> 
  </tr>


  <tr>
    <th> <?php
echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr" rows="2" cols="30"></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK" class="btn" style="font-size:12px">
      <input type="reset" value="reset" class="btn" style="font-size:12px">
    </td>
  </tr>
</table>
</form>

<p align="center"><i><?php
echo gettext("Values marked with (*) are mandatory"); ?></b></i></p>

<?php
$db->close($conn);
?>
</body>
</html>

