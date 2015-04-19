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
                                                                                
  <h1> <?php echo gettext("Networks"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    require_once 'classes/Net_scan.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Security.inc';
    require_once 'classes/WebIndicator.inc';

    $nessus_action = GET('nessus');
    $nagios_action = GET('nagios');
    $net_name = GET('net_name');
    $order = GET('order');

    ossim_valid($nagios_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Nagios action"));
    ossim_valid($nessus_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Nessus action"));
    ossim_valid($net_name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Net name"));
    ossim_valid($order, OSS_ALPHA, OSS_SCORE, OSS_NULLABLE, OSS_SPACE, 'illegal:'._("Order"));

    if (ossim_error()) {
       die(ossim_error());
    }

    if ((!empty($nessus_action)) AND (!empty($net_name))) 
    {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($nessus_action == "enable") {
            Net::enable_plugin($conn, $net_name, 3001);
        } elseif ($nessus_action == "disable") {
            Net::disable_plugin($conn, $net_name, 3001);
        }
        $db->close($conn);
    }
    
    if ((!empty($nagios_action)) AND (!empty($net_name))) 
    {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($nagios_action == "enable") {
            Net::enable_plugin($conn, $net_name, 2007);
        } elseif ($nagios_action == "disable") {
            Net::disable_plugin($conn, $net_name, 2007);
        }
        $db->close($conn);
    }

    if (empty($order)) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>"> <?php echo gettext("Net"); ?> </a></th>
      <th> <?php echo gettext("Ips"); ?> </th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("asset", $order);
          ?>"> <?php echo gettext("Asset"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_c", $order);
          ?>"> <?php echo gettext("Threshold_C"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold_a", $order);
          ?>"> <?php echo gettext("Threshold_A"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("rrd_profile", $order);
          ?>"> <?php echo gettext("RRD Profile"); ?> </a></th>
<!--
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("alert", $order);
          ?>">Alert</a></th>
      <th><a href="<?php //echo $_SERVER["PHP_SELF"]?>?order=<?php
            //echo ossim_db::get_order("persistence", $order);
          ?>">Persistence</a></th>
-->
      <th> <?php echo gettext("Sensors"); ?> </th>
      <th> <?php echo gettext("Scan types"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
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
      <td><?php echo $net->get_asset(); ?></td>
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
    if($scan_list = Net_scan::get_list($conn, "WHERE net_name = '$name' AND plugin_id = 3001"))
    {

        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=disable&net_name=$name\"> " . _("Nessus ENABLED") . " </a><br/>";
    } else {
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=enable&net_name=$name\"> " . _("Nessus DISABLED") . " </a><br/>";
    }

    if($scan_list = Net_scan::get_list($conn, "WHERE net_name = '$name' AND plugin_id = 2007"))
    {

        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nagios=disable&net_name=$name\"> " . _("Nagios ENABLED") . " </a>";
    } else {
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nagios=enable&net_name=$name\">" . _("Nagios DISABLED") . "</a>";
    }



?>
</td>

      <td><?php echo $net->get_descr(); ?></td>
      <td><a href="modifynetform.php?name=<?php echo $name ?>"> <?php echo gettext("Modify"); ?> </a>
          <a href="deletenet.php?name=<?php echo $name ?>"> <?php echo gettext("Delete"); ?> </a></td>
    </tr>

<?php
        } /* net_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="11"><a href="newnetform.php"> <?php echo gettext("Insert new network"); ?> </a></td>
    </tr>
    <tr>
      <td colspan="11"><a href="../conf/reload.php?what=nets&back=<?php echo urlencode($_SERVER["REQUEST_URI"]); ?>"> <?php
if (WebIndicator::is_on("Reload_nets")) {
    echo "<font color=red>&gt;&gt;&gt; " . gettext("Reload") . " &lt;&lt;&lt;</color>";
} else {
	echo gettext("Reload");
} ?> </a></td>
    </tr>
  </table>
    
</body>
</html>

