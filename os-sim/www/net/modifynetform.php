<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Modify network</h2>

<?php
    require_once 'classes/Net.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Net_sensor_reference.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$name = mysql_escape_string($_GET["name"])) {
        echo "<p>Wrong net</p>";
        exit;
    }
    
    if ($net_list = Net::get_list($conn, "WHERE name = '$name'")) {
        $net = $net_list[0];
    }

?>

<form method="post" action="modifynet.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Netname</th>
      <input type="hidden" name="name"
             value="<?php echo $net->get_name(); ?>">
      <td class="left">
        <b><?php echo $net->get_name(); ?></b>
      </td>
  </tr>
  <tr>
    <th>IP</th>
    <td class="left">
        <input type="text" name="ips" 
               value="<?php echo $net->get_ips(); ?>"></td>
  </tr>
  <tr>
    <th>Priority</th>
    <td class="left">
      <select name="priority">
        <option
        <?php if ($net->get_priority() == 0) echo " SELECTED "; ?>
          value="0">0</option>
        <option
        <?php if ($net->get_priority() == 1) echo " SELECTED "; ?>
          value="1">1</option>
        <option
        <?php if ($net->get_priority() == 2) echo " SELECTED "; ?>
          value="2">2</option>
        <option
        <?php if ($net->get_priority() == 3) echo " SELECTED "; ?>
          value="3">3</option>
        <option
        <?php if ($net->get_priority() == 4) echo " SELECTED "; ?>
          value="4">4</option>
        <option
        <?php if ($net->get_priority() == 5) echo " SELECTED "; ?>
          value="5">5</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Threshold C</th>
    <td class="left">
      <input type="text" name="threshold_c" size="4"
             value="<?php echo $net->get_threshold_c(); ?>"></td>
  </tr>
  <tr>
    <th>Threshold A</th>
    <td class="left">
      <input type="text" name="threshold_a" size="4"
             value="<?php echo $net->get_threshold_a(); ?>"></td>
  </tr>
<!--
    <tr>
    <th>Alert</th>
    <td class="left">
      <select name="alert">
        <option <?php //if ($net->get_alert() == 1) echo " SELECTED "; ?>
            value="1">Yes</option>
        <option <?php //if ($net->get_alert() == 0) echo " SELECTED "; ?>
            value="0">No</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Persistence</th>
    <td class="left">
      <input type="text" name="persistence" size="3"
             value="<?php //echo $net->get_persistence(); ?>">min.
    </td>
  </tr>
-->

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
            if (Net_sensor_reference::in_net_sensor_reference
                                       ($conn, $net->get_name(), $sensor_name))
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
        rows="2" cols="20"><?php echo $net->get_descr(); ?></textarea>
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

