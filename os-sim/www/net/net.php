<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Networks</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';

    if (!$order = $_GET["order"]) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">Net</a></th>
      <th>Ips</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order);
          ?>">Asset</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_c", $order);
          ?>">Threshold_C</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_a", $order);
          ?>">Threshold_A</a></th>
<!--
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("alert", $order);
          ?>">Alert</a></th>
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("persistence", $order);
          ?>">Persistence</a></th>
-->
      <th>Sensors</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($net_list = Net::get_list($conn, "ORDER BY $order")) {
        foreach($net_list as $net) {
            $name = $net->get_name();
?>

    <tr>
      <td><?php echo $net->get_name(); ?></td>
      <td><?php echo $net->get_ips(); ?></td>
      <td><?php echo $net->get_priority(); ?></td>
      <td><?php echo $net->get_threshold_c(); ?></td>
      <td><?php echo $net->get_threshold_a(); ?></td>
<!--
      <td><?php //if ($net->get_alert()) echo "Yes"; else echo "No" ?></td>
      <td><?php //echo $net->get_persistence() . " min."; ?></td>
-->
      <td><?php
            if ($sensor_list = $net->get_sensors ($conn)) {
                foreach($sensor_list as $sensor) {
                    echo $sensor->get_sensor_name() . '<br/>';
                }
            }
?>    </td>
      <td><?php echo $net->get_descr(); ?></td>
      <td><a href="modifynetform.php?name=<?php echo $name ?>">Modify</a>
          <a href="deletenet.php?name=<?php echo $name ?>">Delete</a></td>
    </tr>

<?php
        } /* net_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="10"><a href="newnetform.php">Insert new network</a></td>
    </tr>
    <tr>
      <td colspan="10"><a href="../conf/reload.php?what=nets">Reload</a></td>
    </tr>
  </table>
    
</body>
</html>

