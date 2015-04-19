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
                                                                                
  <h1> <?php echo gettext("Network groups"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Net_group.inc';
    require_once 'classes/Net_group_scan.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Security.inc';

    $nessus_action = GET('nessus');
    $nagios_action = GET('nagios_action');
    $net_group_name = GET('net_group_name');
    $order = GET('order');

    ossim_valid($nagios_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Nagios action"));
    ossim_valid($nessus_action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Nessus action"));
    ossim_valid($net_group_name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Net group name"));
    ossim_valid($order, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Order"));

    if (ossim_error()) {
        die(ossim_error());
    }
    
    if ((!empty($nessus_action)) AND (!empty($net_group_name))) 
    {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($nessus_action == "enable") {
            Net_group::enable_nessus($conn, $net_group_name);
        } elseif ($nessus_action = "disable") {
            Net_group::disable_nessus($conn, $net_group_name);
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
      <th> <?php echo gettext("Networks"); ?> </th>
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
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($net_group_list = Net_group::get_list($conn, "ORDER BY $order")) {
        foreach($net_group_list as $net_group) {
            $name = $net_group->get_name();
?>

    <tr>
      <td><?php echo $net_group->get_name(); ?></td>
      <td align="left">
      <?php
            if ($network_list = $net_group->get_networks ($conn)) {
                foreach($network_list as $network) {
                    echo $network->get_net_name() . '<br/>';
                }
            } else {
            	echo "&nbsp;";
            }
?>    
      </td>

      <td><?php echo $net_group->get_threshold_c(); ?></td>
      <td><?php echo $net_group->get_threshold_a(); ?></td>
      <td>
        <?php 
            if (!($rrd_profile = $net_group->get_rrd_profile()))
                echo "None";
            else
                echo $rrd_profile;
        ?>
      </td>

    <td>
<?php
    if($scan_list = Net_group_scan::get_list($conn, 
        "WHERE net_group_name = '$name' AND plugin_id = 3001"))
    {
        $name = stripslashes($name);
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=disable&net_group_name=$name\">ENABLED</a>";
    } else {
        $name = stripslashes($name);
        echo "<a href=\"". $_SERVER["PHP_SELF"] .
            "?nessus=enable&net_group_name=$name\">DISABLED</a>";
    }

?>
</td>

      <td><?php echo $net_group->get_descr(); ?>&nbsp;</td>
      <td><a href="modifynetgroupform.php?name=<?php echo $name ?>"> <?php echo gettext("Modify"); ?> </a>
          <a href="deletenetgroup.php?name=<?php echo $name ?>"> <?php echo gettext("Delete"); ?> </a></td>
    </tr>

<?php
        } /* net_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="11"><a href="newnetgroupform.php"> <?php echo gettext("Insert new network group"); ?> </a></td>
    </tr>
  </table>
    
</body>
</html>

