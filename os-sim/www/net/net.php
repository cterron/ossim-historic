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
                                                                                
  <h1>Networks</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    require_once 'classes/Net_scan.inc';
    require_once 'classes/Plugin.inc';


    if (($nessus_action = $_GET["nessus"]) AND ($net_name = $_GET["net_name"])) 
    {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($nessus_action == "enable") {
            Net::enable_nessus($conn, $net_name);
        } elseif ($nessus_action = "disable") {
            Net::disable_nessus($conn, $net_name);
        }
        $db->close($conn);
    }

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
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("rrd_profile", $order);
          ?>">RRD Profile</a></th>
<!--
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("alert", $order);
          ?>">Alert</a></th>
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("persistence", $order);
          ?>">Persistence</a></th>
-->
      <th>Sensors</th>
      <th>Nessus Scan</th>
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
      <td>
        <?php 
            if (!($rrd_profile = $net->get_rrd_profile()))
                echo "None";
            else
                echo $rrd_profile;
        ?>
      </td>
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

    <td>
<?php
    if($scan_list = Net_scan::get_list($conn, 
        "WHERE net_name = '$name' AND plugin_id = 3001"))
    {

        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=disable&net_name=$name\">ENABLED</a>";
/*
    foreach($scan_list as $scan){
        $id = $scan->get_plugin_id();
        $plugin_name = "";
        if ($plugin_list = Plugin::get_list($conn, "WHERE id = $id")) {
            $plugin_name = $plugin_list[0]->get_name();
            echo "$plugin_name<BR>";
        } else {
            echo $id;
        }
    }
*/
    } else {
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=enable&net_name=$name\">DISABLED</a>";
    }

?>
</td>

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
      <td colspan="11"><a href="newnetform.php">Insert new network</a></td>
    </tr>
    <tr>
      <td colspan="11"><a href="../conf/reload.php?what=nets">Reload</a></td>
    </tr>
  </table>
    
</body>
</html>

