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
                                                                                <?php 
    
    require_once ('classes/Security.inc');
    if (REQUEST('scan')) {
        echo "<h1>" . gettext("Insert new scan") . "</h1>";
        echo "<p>";
        echo gettext("Please, fill these global properties about the hosts you've scaned");
        echo ":</p>";
    } else {
        echo "<h1>" . gettext("Insert new host") . "</h1>";
    }
?>

<?php
    require_once ('ossim_db.inc');
    require_once ('ossim_conf.inc');
    require_once ('classes/Sensor.inc');
    require_once ('classes/RRD_config.inc');

    $ip = REQUEST('ip');
    $ips = REQUEST('ips');
    $scan = REQUEST('scan');
   
    ossim_valid($ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("ip"));
    ossim_valid($ips, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("ips"));
    ossim_valid($scan, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("scan"));

    if (ossim_error()) {
        die(ossim_error());
    }

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = $GLOBALS["CONF"];
    $threshold = $conf->get_conf("threshold");

    $action = "newhost.php";
    
    if (REQUEST('scan')) {
        $ip = REQUEST('target');
        $action = "../netscan/scan_db.php";
    }
?>

    <form method="post" action="<?php echo $action ?>">
    <table align="center">
      <input type="hidden" name="insert" value="insert">

<?php
    if (empty($scan)) {
?>
  <tr>
    <th> <?php echo gettext("Hostname"); ?> (*)</th>
    <td class="left"><input type="text" name="hostname"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("IP"); ?> (*)</th>
    <td class="left">
      <input type="text" value="<?php echo $ip ?>" name="ip">
    </td>
  </tr>
<?php
    }
?>
  <tr>
    <th> <?php echo gettext("Asset"); ?> (*)</th>
    <td class="left">
      <select name="asset">
        <option value="0">
	<?php echo gettext("0"); ?> </option>
        <option value="1">
	<?php echo gettext("1"); ?> </option>
        <option selected value="2">
	<?php echo gettext("2"); ?> </option>
        <option value="3">
	<?php echo gettext("3"); ?> </option>
        <option value="4">
	<?php echo gettext("4"); ?> </option>
        <option value="5">
	<?php echo gettext("5"); ?> </option>
      </select>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold C"); ?> (*)</th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_c" size="4">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold A"); ?> (*)</th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_a" size="4">
    </td>
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
        if (strcmp($profile, "global")) 
        {
            echo "<option value=\"$profile\">$profile</option>\n";
        }
    }
?>
        <option value="" selected>
	<?php echo gettext("None"); ?> </option>
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
  </tr>
    <th> <?php echo gettext("NAT"); ?> </th>
    <td class="left">
      <input type="text" name="nat">
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
        <input type="checkbox" name="<?php echo $name;?>"
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
      <input type="checkbox" name="nessus" value="1"> <?php echo gettext("Enable nessus scan"); ?> </input><br/>
      <input type="checkbox" name="nagios" value="1"> <?php echo gettext("Enable nagios"); ?> </input>
    </td>
  </tr>
<?php
    if (empty($scan)) {
?>
  <tr>
    <th> <?php echo gettext("OS"); ?> </th>
    <td class="left">
      <select name="os"
      >
        <option value="unknown">
	<?php echo gettext("Unknown"); ?> </option>
        <option value="windows">
	<?php echo gettext("Windows"); ?> </option>
        <option value="linux">
	<?php echo gettext("Linux"); ?> </option>
        <option value="bsd">
	<?php echo gettext("BSD"); ?> </option>
        <option value="mac">
	<?php echo gettext("Mac"); ?> </option>
        <option value="sun">
	<?php echo gettext("Sun"); ?> </option>
        <option value="plan9">
	<?php echo gettext("Plan9"); ?> </option> <!-- gdiaz's tribute :) -->
        <option value="unknown">
	<?php echo gettext("Other"); ?> </option>
      </select>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Mac"); ?> </th>
    <td class="left"><input type="text" name="mac" /></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Mac Vendor"); ?></th>
    <td class="left"><input type="text" name="mac_vendor" /></td>
  </tr>
<?php
    } else {
?>
        <input type="hidden" name="ips" value="<?php echo $ips ?>" />
<?php
        for ($i = 0; $i < $ips; $i++) {
?>
        <input type="hidden" name="ip_<?php echo $i ?>" 
            value="<?php echo POST("ip_$i") ?>" />
<?php
        } /* foreach */
    } /* if ($scan) */
?>
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr" rows="2" cols="20"></textarea>
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

