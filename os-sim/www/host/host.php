<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Hosts</h1>

<?php 
    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_os.inc';
    require_once 'classes/Host_scan.inc';
    require_once 'classes/Plugin.inc';

    if (!$order = $_GET["order"]) $order = "hostname"; 
    if (!$search = $_POST["search"]) 
        $search = "";
    else 
        $search = "WHERE ip like '%$search%' OR hostname like '%$search%'";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("hostname", $order);
          ?>">Hostname</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">Ip</a></th>
      <th>NAT</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("asset", $order);
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
      <th><a href="<?php // echo $_SERVER["PHP_SELF"]?>?order=<?php
            // echo ossim_db::get_order("alert", $order);
          ?>">Alert</a></th>
      <th><a href="<?php // echo $_SERVER["PHP_SELF"]?>?order=<?php
            // echo ossim_db::get_order("persistence", $order);
          ?>">Persistence</a></th>
-->
      <th>Sensors</th>
      <th>Scantype</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($host_list = Host::get_list($conn, "$search", "ORDER BY $order")) {
        foreach($host_list as $host) {
            $ip = $host->get_ip();
?>

    <tr>
      <td><a href="../report/index.php?host=<?php 
        echo $ip ?>"><?php echo $host->get_hostname(); ?></a>
      <?php echo Host_os::get_os_pixmap($conn, $host->get_ip()); ?>
      </td>
      <td><?php echo $host->get_ip(); ?></td>
      <td><?php if ($nat = $host->get_nat()) echo $nat; else echo "-" ?></td>
      <td><?php echo $host->get_asset(); ?></td>
      <td><?php echo $host->get_threshold_c(); ?></td>
      <td><?php echo $host->get_threshold_a(); ?></td>
      <td>
        <?php 
            if (!($rrd_profile = $host->get_rrd_profile()))
                echo "None";
            else
                echo $rrd_profile;
        ?>
      </td>
<!--      <td><?php /* if ($host->get_alert()) echo "Yes"; else echo "No" */?></td> -->
<!--      <td><?php /* echo $host->get_persistence() . " min."; */ ?></td> -->
      <!-- sensors -->
      <td><?php
            if ($sensor_list = $host->get_sensors ($conn)) {
                foreach($sensor_list as $sensor) {
                    echo $sensor->get_sensor_name() . '<br/>';
                }
            }
?>    </td>
    <td>
<?php
if($scan_list = Host_scan::get_list($conn, "WHERE host_ip = inet_aton('$ip')")){
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
} else {
echo "None";
}

?>
    </td>
      <td><?php echo $host->get_descr(); ?></td>
      <td>
          <a href="modifyhostform.php?ip=<?php echo $ip ?>">Modify</a>
          <a href="deletehost.php?ip=<?php echo $ip ?>">Delete</a>
      </td>
    </tr>

<?php
        } /* host_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="12"><a href="newhostform.php">Insert new host</a></td>
    </tr>
    <tr>
      <td colspan="12"><a href="../conf/reload.php?what=hosts">Reload</a></td>
    </tr>
  </table>

  <br/><br/>
  <table align="center">
  <form action="<?php echo $_SERVER["PHP_SELF"]?>" method="post">
    <tr>
      <th>Search</th>
      <td><input type="text" name="search"></td>
    </tr>
    <tr><td colspan="2"><input type="submit" value="OK"></td></tr>
  </form>
  </table>
    
</body>
</html>

