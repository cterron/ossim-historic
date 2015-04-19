<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyNetworks");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Insert new network</h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('ossim_conf.inc');
    require_once ('classes/Sensor.inc');
    require_once ('classes/Net_sensor_reference.inc');
    require_once ('classes/RRD_config.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = new ossim_conf();
    $threshold = $conf->get_conf("threshold");
?>

<form method="post" action="newnet.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Name</th>
    <td class="left"><input type="text" name="name" size="30"></td>
  </tr>
  <tr>
    <th>Ips</th>
    <td class="left">
       <i>example: 192.168.0.0/24,192.168.1.0/24</i><br/>
      <input type="text" name="ips" size="30">
    </td>
  </tr>
  <tr>
    <th>Priority</th>
    <td class="left">
      <select name="priority">
   <!-- <option value="0">0</option> -->
        <option value="1">1</option>
        <option selected value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Threshold C</th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_c" size="4">
    </td>
  </tr>
  <tr>
    <th>Threshold A</th>
    <td class="left">
      <input type="text" value="<?php echo $threshold ?>" 
             name="threshold_a" size="4">
    </td>
  </tr>
  <tr>
    <th>RRD Profile<br/>
        <font size="-2">
          <a href="../rrd_conf/new_rrd_conf_form.php">Insert new profile?</a>
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
        <option value="" selected>None</option>
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
    <th>Sensors<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">Insert new sensor?</a>
        </font>
    </th>
    <td class="left">
<?php
                                                                                
    /* ===== sensors ==== */
    $i = 1;
    if ($sensor_list = Sensor::get_list($conn)) {
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
    <th> Scan options </th>
    <td class="left">
        <input type="checkbox" name="nessus" value="1"> Enable nessus scan </input>
    </td> 
  </tr>


  <tr>
    <th>Description</th>
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

