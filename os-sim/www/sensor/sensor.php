<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Sensors</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Plugin.inc';
    require_once 'get_sensor_plugins.php';
    require_once 'get_sensors.php';

    if (!$order = $_GET["order"]) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">Ip</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">Hostname</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order);
          ?>">Priority</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("port", $order);
          ?>">Port</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("connect", $order);
          ?>">Active</a></th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';

    $db = new ossim_db();
    $conn = $db->connect();

    $sensor_list = server_get_sensors();
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
    
    if ($sensor_list = Sensor::get_list($conn, "ORDER BY $order")) {
        foreach($sensor_list as $sensor) {
            $ip = $sensor->get_ip();
            $name = $sensor->get_name();

?>

    <tr>
      <td><a href="sensor_plugins.php?sensor=<?php echo $ip ?>"><?php echo $sensor->get_ip(); ?></a></td>
      <td><?php echo $sensor->get_name(); ?></td>
      <td><?php echo $sensor->get_priority(); ?></td>
      <td><?php echo $sensor->get_port(); ?></td>
      <td><?php 
        if (in_array($sensor->get_ip(),$sensor_stack)){
            echo "<font color=\"green\"><b>YES</b></font>";
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
        <a href="modifysensorform.php?name=<?php echo $name ?>">Modify</a>
        <a href="deletesensor.php?name=<?php echo $name ?>">Delete</a></td>
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
      <td colspan="7"><font color="red"><b>Warning</b></font>:
        the following sensor(s) are being reported as enabled by 
        the server but aren't configured.
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
      <td><font color="green"><b>YES</b></font></td>
      <td>-</td>
      <td><a href="newsensorform.php?ip=<?php echo $ip_diff ?>">Insert</a></td>
    </tr>
    <tr><td colspan="7"></td></tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="10"><a href="newsensorform.php">Insert new sensor</a></td>
    </tr>
    <tr>
      <td colspan="10"><a href="../conf/reload.php?what=sensors">Reload</a></td>
    </tr>
</table>


</body>
</html>

