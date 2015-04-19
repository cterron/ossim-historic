<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Modify host"); ?> </h1>

<?php
    require_once 'classes/Host.inc';
    require_once 'classes/Host_scan.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/RRD_config.inc';
    require_once 'classes/Security.inc';
    

    $ip = GET('ip');
    ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("ip"));

    if (ossim_error()) {
        die(ossim_error());
    }

    $db = new ossim_db();
    $conn = $db->connect();

    if ($host_list = Host::get_list($conn, "WHERE ip = '$ip'")) {
        $host = $host_list[0];
    }


    /* print SELECTED for html-select when os is matched */
    function match_os($pattern, $os)
    {
        $pattern = "/$pattern/i";
        if (preg_match($pattern, $os))
            echo " SELECTED ";
    }

?>

<form method="post" action="modifyhost.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Hostname"); ?> (*)</th>
    <td class="left">
      <input type="text" name="hostname"
             value="<?php echo $host->get_hostname(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("IP"); ?> (*)</th>
        <input type="hidden" name="ip"
               value="<?php echo $host->get_ip(); ?>">
    <td class="left">
      <b><?php echo $host->get_ip(); ?></b>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Asset"); ?> (*)</th>
    <td class="left">
      <select name="asset">
        <option
        <?php if ($host->get_asset() == 0) echo " SELECTED "; ?>
          value="0">
	  <?php echo gettext("0"); ?> </option>
        <option
        <?php if ($host->get_asset() == 1) echo " SELECTED "; ?>
          value="1">
	  <?php echo gettext("1"); ?> </option>
        <option
        <?php if ($host->get_asset() == 2) echo " SELECTED "; ?>
          value="2">
	  <?php echo gettext("2"); ?> </option>
        <option
        <?php if ($host->get_asset() == 3) echo " SELECTED "; ?>
          value="3">
	  <?php echo gettext("3"); ?> </option>
        <option
        <?php if ($host->get_asset() == 4) echo " SELECTED "; ?>
          value="4">
	  <?php echo gettext("4"); ?> </option>
        <option
        <?php if ($host->get_asset() == 5) echo " SELECTED "; ?>
          value="5">
	  <?php echo gettext("5"); ?> </option>
      </select>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold C"); ?> (*)</th>
    <td class="left">
      <input type="text" name="threshold_c" size="4"
             value="<?php echo $host->get_threshold_c(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold A"); ?> (*)</th>
    <td class="left">
      <input type="text" name="threshold_a" size="4"
             value="<?php echo $host->get_threshold_a(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("RRD Profile"); ?> (*)<br/>
        <font size="-2">
          <a href="../rrd_conf/new_rrd_conf_form.php">
	  <?php echo gettext("Insert new profile"); ?> ?</a>
        </font>
    </th>
    <td class="left">
      <select name="rrd_profile">
<?php
    foreach (RRD_Config::get_profile_list($conn) as $profile)
    {
        $host_profile = $host->get_rrd_profile();
        if (strcmp($profile, "global")) 
        {
            $option = "<option value=\"$profile\"";
            if (0 == strcmp($host_profile, $profile))
                $option .= " SELECTED ";
            $option .= ">$profile</option>\n";
            echo $option;
        }
    }
?>
        <option value="" 
            <?php if (!$host_profile) echo " SELECTED " ?>>
	    <?php echo gettext("None"); ?> </option>
      </select>
    </td>
  </tr>
<!--
  <tr>
    <th>Alert</th>
    <td class="left">
      <select name="alert">
        <option <?php // if ($host->get_alert() == 1) echo " SELECTED "; ?>
            value="1">Yes</option>
        <option <?php // if ($host->get_alert() == 0) echo " SELECTED "; ?>
            value="0">No</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Persistence</th>
    <td class="left">
      <input type="text" name="persistence" size="3"
             value="<?php //echo $host->get_persistence(); ?>">min.
    </td>
  </tr>
-->
  <tr>
    <th> <?php echo gettext("NAT"); ?> </th>
    <td class="left">
        <input type="text" name="nat"
               value="<?php echo $host->get_nat(); ?>">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Sensors"); ?> (*)<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">
	  <?php echo gettext("Insert new sensor"); ?> ?</a>
        </font>
    </th> 
    <td class="left">
<?php

    /* ===== sensors ==== */
    $i = 1;
    if ($sensor_list = Sensor::get_list($conn, "ORDER BY name")) {
        foreach($sensor_list as $sensor) {
            $sensor_name = $sensor->get_name();
            $sensor_ip =   $sensor->get_ip();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "nsens"; ?>"
            value="<?php echo count($sensor_list); ?>">
<?php
            }
            $name = "mboxs" . $i;
?>
        <input type="checkbox"
<?php
            if (Host_sensor_reference::in_host_sensor_reference
                                       ($conn, $host->get_ip(), $sensor_name))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $sensor_name; ?>">
            <?php echo $sensor_ip . " (" . $sensor_name . ")<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>
    <tr>
    <th> <?php echo gettext("Scan options"); ?> </th>
    <td class="left">
        <input type="checkbox"
        <?php
        if(Host_scan::in_host_scan($conn, $host->get_ip(), 3001)){
            echo " CHECKED ";
        }
        ?>
        name="nessus" value="1">
	<?php echo gettext("Enable nessus scan"); ?> </input><br>
        <input type="checkbox"
        <?php
        if(Host_scan::in_host_scan($conn, $host->get_ip(), 2007)){
            echo " CHECKED ";
        }
        ?>
        name="nagios" value="1">
	<?php echo gettext("Enable nagios"); ?> </input>

    </td>
    </tr>
  <tr>
    <th> <?php echo gettext("OS"); ?> </th>
    <td class="left">
      <select name="os"
      >
        <option value="unknown">
	<?php echo gettext("Unknown"); ?> </option>

        <option 
            <?php match_os("win", $host->get_os($conn)) ?> 
            value="windows">
	<?php echo gettext("Windows"); ?> </option>

        <option 
            <?php match_os("linux", $host->get_os($conn)) ?> 
            value="linux">
	        <?php echo gettext("Linux"); ?> </option>

        <option 
            <?php match_os("bsd", $host->get_os($conn)) ?> 
            value="bsd">
	        <?php echo gettext("BSD"); ?> </option>

        <option 
            <?php match_os("mac", $host->get_os($conn)) ?> 
            value="mac">
	        <?php echo gettext("Mac"); ?> </option>

        <option 
            <?php match_os("sun", $host->get_os($conn)) ?> 
            value="sun">
	        <?php echo gettext("Sun"); ?> </option>

        <option 
            <?php match_os("plan9", $host->get_os($conn)) ?> 
            value="plan9">
	        <?php echo gettext("Plan9"); ?> </option> <!-- gdiaz's tribute :) -->

        <option 
            value="unknown">
	        <?php echo gettext("Other"); ?> </option>

      </select>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Mac Address"); ?> </th>
    <td class="left">
      <input type="text" name="mac" 
        value="<?php echo $host->get_mac_address($conn); ?>" />
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Mac Vendor"); ?> </th>
    <td class="left">
      <input type="text" name="mac_vendor" 
        value="<?php echo $host->get_mac_vendor($conn); ?>" />
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr"
        rows="2" cols="20"><?php echo $host->get_descr(); ?></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

<p align="center"><i>Values marked with (*) are mandatory</b></i></p>

</body>
</html>
<?php
    $db->close($conn);
?>
