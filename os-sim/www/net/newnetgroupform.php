<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyNetworks");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Insert new network group"); ?> </h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('ossim_conf.inc');
    require_once ('classes/Sensor.inc');
    require_once ('classes/Net.inc');
    require_once ('classes/Net_sensor_reference.inc');
    require_once ('classes/RRD_config.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = new ossim_conf();
    $threshold = $conf->get_conf("threshold");
?>

<form method="post" action="newnetgroup.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Name"); ?> </th>
    <td class="left"><input type="text" name="name" size="30"></td>
  </tr>

  <tr>
    <th> <?php echo gettext("Networks"); ?> <br/>
        <font size="-2">
          <a href="newnetform.php">
	  <?php echo gettext("Insert new network"); ?> ?</a>
        </font>
    </th>
    <td class="left">
<?php
                                                                                
    /* ===== Networks ==== */
    $i = 1;
    if ($network_list = Net::get_list($conn)) {
        foreach($network_list as $network) {
            $network_name = $network->get_name();
            $network_ips =   $network->get_ips();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "nnets"; ?>"
            value="<?php echo count($network_list); ?>">
<?php
            }
            $name = "mboxs" . $i;
?>
        <input type="checkbox" name="<?php echo $name;?>"
            value="<?php echo $network_name; ?>">
            <?php echo $network_name . " (" . $network_ips . ")<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold C"); ?> </th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_c" size="4">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold A"); ?> </th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_a" size="4">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("RRD Profile"); ?> <br/>
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
    <tr>
    <th> <?php echo gettext("Scan options"); ?> </th>
    <td class="left">
        <input type="checkbox" name="nessus" value="1">
	<?php echo gettext("Enable nessus scan"); ?> </input>
    </td> 
  </tr>


  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr" rows="2" cols="30"></textarea>
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
<?php
    $db->close($conn);
?>
</body>
</html>

