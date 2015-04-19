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

  <h1> <?php echo gettext("Host groups"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Host_group.inc';
    require_once 'classes/Host_group_scan.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Security.inc';

    $nessus_action = GET('nessus');    
    $host_group_name = GET('host_group_name');
    $order = GET('order');

    ossim_valid($nessus_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Nessus action"));
    ossim_valid($host_group_name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Host group name"));
    ossim_valid($order, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Order"));

    if (ossim_error()) {
        die(ossim_error());
    }
    if ((!empty($nessus_action)) AND (!empty($host_group_name)))
    {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($nessus_action == "enable") {
            Host_group::enable_nessus($conn, $host_group_name);
        } elseif ($nessus_action = "disable") {
            Host_group::disable_nessus($conn, $host_group_name);
        }
        $db->close($conn);
    }


    if (empty($order)) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>"> <?php echo gettext("Host"); ?> </a></th>
      <th> <?php echo gettext("Hosts"); ?> </th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_c", $order);
          ?>"> <?php echo gettext("Threshold_C"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_a", $order);
          ?>"> <?php echo gettext("Threshold_A"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("rrd_profile", $order);
          ?>"> <?php echo gettext("RRD Profile"); ?> </a></th>
      <th> <?php echo gettext("Nessus Scan"); ?> </th>
      <th> <?php echo gettext("Sensors"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    if ($host_group_list = Host_group::get_list($conn, "ORDER BY $order")) {
	foreach($host_group_list as $host_group) {
            $name = $host_group->get_name();

?>

    <tr>
      <td><?php echo $host_group->get_name(); ?></td>
      <td align="left">
      <?php
            if ($host_list = $host_group->get_hosts ($conn)) {
                foreach($host_list as $host) {
              	      echo $host->get_host_name($conn) . '<br/>';
		  }
            } else {
                echo "&nbsp;";	
            }
?>
      </td>

      <td><?php echo $host_group->get_threshold_c(); ?></td>
      <td><?php echo $host_group->get_threshold_a(); ?></td>
      <td>
        <?php
            if (!($rrd_profile = $host_group->get_rrd_profile()))
                echo "None";
            else
                echo $rrd_profile;
        ?>
      </td>
      <td>
      <?php
        if($scan_list = Host_group_scan::get_list($conn,
          "WHERE host_group_name = '$name' AND plugin_id = 3001"))
        {
          $name = stripslashes($name);
          echo "<a href=\"". $_SERVER["PHP_SELF"] .
              "?nessus=disable&host_group_name=$name\">ENABLED</a>";
        } else {
        $name = stripslashes($name);
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=enable&host_group_name=$name\">DISABLED</a>";
        }
      ?>
      </td>

      <td><?php
            if ($sensor_list = $host_group->get_sensors ($conn)) {
                foreach($sensor_list as $sensor) {
                    echo $sensor->get_sensor_name() . '<br/>';
                }
            }
?>    </td>
      <td><?php echo $host_group->get_descr(); ?>&nbsp;</td>
      <td><a href="modifyhostgroupform.php?name=<?php echo $name ?>"> <?php echo gettext("Modify"); ?> </a>
          <a href="deletehostgroup.php?name=<?php echo $name ?>"> <?php echo gettext("Delete"); ?> </a></td>
    </tr>

<?php

        } /* host_list */

    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="11"><a href="newhostgroupform.php"> <?php echo gettext("Insert new host group"); ?> </a></td>
    </tr>
  </table>

</body>
</html>

