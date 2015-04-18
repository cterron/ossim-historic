<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify host</h1>

<?php
    require_once 'classes/Host.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$ip = $_GET["ip"]) {
        echo "<p>Wrong ip</p>";
        exit;
    }
    
    if ($host_list = Host::get_list($conn, "WHERE ip = '$ip'")) {
        $host = $host_list[0];
    }

?>

<form method="post" action="modifyhost.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Hostname</th>
    <td class="left">
      <input type="text" name="hostname"
             value="<?php echo $host->get_hostname(); ?>"></td>
  </tr>
  <tr>
    <th>IP</th>
        <input type="hidden" name="ip" 
               value="<?php echo $host->get_ip(); ?>">
    <td class="left">
      <b><?php echo $host->get_ip(); ?></b>
    </td>
  </tr>
  <tr>
    <th>Asset</th>
    <td class="left">
      <select name="asset">
        <option
        <?php if ($host->get_asset() == 0) echo " SELECTED "; ?>
          value="0">0</option>
        <option
        <?php if ($host->get_asset() == 1) echo " SELECTED "; ?>
          value="1">1</option>
        <option
        <?php if ($host->get_asset() == 2) echo " SELECTED "; ?>
          value="2">2</option>
        <option
        <?php if ($host->get_asset() == 3) echo " SELECTED "; ?>
          value="3">3</option>
        <option
        <?php if ($host->get_asset() == 4) echo " SELECTED "; ?>
          value="4">4</option>
        <option
        <?php if ($host->get_asset() == 5) echo " SELECTED "; ?>
          value="5">5</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Threshold C</th>
    <td class="left">
      <input type="text" name="threshold_c" size="4"
             value="<?php echo $host->get_threshold_c(); ?>"></td>
  </tr>
  <tr>
    <th>Threshold A</th>
    <td class="left">
      <input type="text" name="threshold_a" size="4"
             value="<?php echo $host->get_threshold_a(); ?>"></td>
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
    <th>NAT</th>
    <td class="left">
        <input type="text" name="nat"
               value="<?php echo $host->get_nat(); ?>">
    </td>
  </tr>
  <tr>
    <th>Sensors<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">Insert new sensor?</a>
        </font><br/>
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
    <th>Description</th>
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

</body>
</html>
<?php
    $db->close($conn);
?>
