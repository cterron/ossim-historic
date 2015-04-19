<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Sensors"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Security.inc';
    require_once 'get_sensor_plugins.php';
    require_once 'get_sensors.php';
    
    $order = GET('order');
    
    ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
  
    if (ossim_error()) {
        die(ossim_error());
    }
  
    if (empty($order))
         $order = "name";
?>

  <table align="center">
  <tr>
  <th><?php echo gettext("Active Sensors");?></th>
  <th><?php echo gettext("Total Sensors");?></th>
  </tr><tr>
  <td><div id="active">0</div></td>
  <td><b><div id="total">0</div></b></td>
  </tr>
  </table>
  <br/>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">
	  <?php echo gettext("Ip"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">
	  <?php echo gettext("Hostname"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order);
          ?>">
	  <?php echo gettext("Priority"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("port", $order);
          ?>">
	  <?php echo gettext("Port"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("connect", $order);
          ?>">
	  <?php echo gettext("Active"); ?> </a></th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';

    $db = new ossim_db();
    $conn = $db->connect();

    $sensor_list = server_get_sensors($conn);
    $sensor_stack = array();
    $sensor_configured_stack = array();
    if($sensor_list){
        foreach ($sensor_list as $sensor_status){
            if(in_array($sensor_status["sensor"],$sensor_stack)) continue;
            if($sensor_status["state"] = "on"){
                array_push($sensor_stack,$sensor_status["sensor"]);
            }
        }
    }

    $active_sensors = 0;
    $total_sensors = 0;
    
    if ($sensor_list = Sensor::get_list($conn, "ORDER BY $order")) {
        foreach($sensor_list as $sensor) {
            $ip = $sensor->get_ip();
            $name = $sensor->get_name();
            $total_sensors++;

?>

    <tr>
      <td><a href="sensor_plugins.php?sensor=<?php echo $ip ?>"><?php echo $sensor->get_ip(); ?></a></td>
      <td><?php echo $sensor->get_name(); ?></td>
      <td><?php echo $sensor->get_priority(); ?></td>
      <td><?php echo $sensor->get_port(); ?></td>
      <td><?php 
        if (in_array($sensor->get_ip(),$sensor_stack)){
            echo "<font color=\"green\"><b>YES</b></font>";
            $active_sensors++;
            array_push($sensor_configured_stack,$sensor->get_ip());
        } else {
            echo "<font color=\"red\"><b>NO</b></font>";
        }
      /*
        if ($sensor->get_connect() == 1) echo "YES";
        else echo "NO";
        */
      ?></td>
      <td><?php echo $sensor->get_descr(); ?></td>
      <td>
<!--        <a href="editsensor.php?ip=<?php //echo $ip ?>">Remote edit</a>* -->
        [ <a href="modifysensorform.php?name=<?php echo $name ?>">
	<?php echo gettext("Modify"); ?> </a> |
        <a href="deletesensor.php?name=<?php echo $name ?>">
	<?php echo gettext("Delete"); ?> </a> |
        <a href="interfaces.php?sensor=<?php echo $name ?>">
	<?php echo gettext("Interfaces"); ?> </a> ]</td>
    </tr>

<?php
        } /* sensor_list */
    } /* foreach */

    $db->close($conn);
?>

<!--
<p><i>* You must share dsa keys between hosts in order to use this
functionality</i><br/><i>(see README.sensors for more details).</i><br><i>Partially broken. Use with care or fix.</i></p>
-->


<?php
    $diff_arr = array_diff($sensor_stack,$sensor_configured_stack);
    if($diff_arr) {
?>
    <tr><td colspan="7"></td></tr>
    <tr>
      <td colspan="7"><font color="red"><b> <?php echo gettext("Warning"); ?> </b></font>:
        <?php echo gettext("the following sensor(s) are being reported as enabled by the server but aren't configured"); ?> .
      </td>
    </tr>
<?php
        foreach($diff_arr as $ip_diff) {
?>
    <tr>
      <td><a href="sensor_plugins.php?sensor=<?php echo $ip_diff ?>">
        <?php echo $ip_diff ?></a></td>
      <td>-</td>
      <td>-</td>
      <td>-</td>
      <td><font color="green"><b> <?php echo gettext("YES"); ?> </b></font></td>
      <td>-</td>
      <td><a href="newsensorform.php?ip=<?php echo $ip_diff ?>"> 
      <?php echo gettext("Insert"); ?> </a></td>
    </tr>
    <tr><td colspan="7"></td></tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="10"><a href="newsensorform.php"> <?php echo gettext("Insert new sensor"); ?> </a></td>
    </tr>
    <tr>
      <td colspan="10"><a href="../conf/reload.php?what=sensors"> <?php echo gettext("Reload"); ?> </a></td>
    </tr>
</table>

<script language="javascript">
active_sensors_div = document.getElementById("active");
total_sensors_div = document.getElementById("total");

<?php
if($active_sensors == 0){
?>
active_sensors_div.innerHTML = "<font color=\"red\">" + <?php echo $active_sensors; ?> + "</font>"; 
<?php
} else {
?>
active_sensors_div.innerHTML = "<font color=\"green\">" + <?php echo $active_sensors; ?> + "</font>"; 
<?php
}
?>
total_sensors_div.innerHTML = <?php echo $total_sensors; ?>;
</script>

</body>
</html>

