<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPolicy");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Policy"); ?> </h1>

<?php
    require_once ('classes/Policy.inc');
    require_once ('classes/Host.inc');
    require_once ('ossim_db.inc');
    $order = 'priority DESC';

?>

  <table align="center">
    <tr>
      <th> <?php echo gettext("Source"); ?> </th>
      <th> <?php echo gettext("Dest"); ?> </th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order);
          ?>">
	  <?php echo gettext("Priority"); ?> </a></th>
      <th> <?php echo gettext("Port Group"); ?> </th>
      <th> <?php echo gettext("Plugin Group"); ?> </th>
      <th> <?php echo gettext("Sensors"); ?> </th>
      <th> <?php echo gettext("Time Range"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Store"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();

    if ($policy_list = Policy::get_list($conn, "ORDER BY $order")) {
        foreach ($policy_list as $policy) {
?>

    <tr>
      <!-- source -->
      <td>
<?php
            if ($source_host_list = $policy->get_hosts ($conn, 'source')) {
                foreach($source_host_list as $source_host) {
                    echo Host::ip2hostname($conn, 
                                           $source_host->get_host_ip()) . 
                                           '<br/>';
                }
            }
            if ($source_net_list = $policy->get_nets ($conn, 'source')) {
                foreach($source_net_list as $source_net) {
                    echo $source_net->get_net_name() . '<br/>';
                }
            }
?>
      </td>
      
      <!-- dest -->
      <td>
<?php
            if ($dest_host_list = $policy->get_hosts ($conn, 'dest')) {
                foreach($dest_host_list as $dest_host) {
                    echo Host::ip2hostname($conn, 
                                           $dest_host->get_host_ip()) . 
                                           '<br/>';
                }
            }
            if ($dest_net_list = $policy->get_nets ($conn, 'dest')) {
                foreach($dest_net_list as $dest_net) {
                    echo $dest_net->get_net_name() . '<br/>';
                }
            }
?>
      </td>

      <!-- Priority -->
      <td>
      <?php 
      $priority = $policy->get_priority(); 

      if($priority == -1){
      echo _("Do not change");
      } else {
      echo $priority;
      }
      ?>
      </td>

      <!-- port group -->
      <td>
<?php
            if ($port_list = $policy->get_ports ($conn)) {
                foreach($port_list as $port_group) {
                    echo $port_group->get_port_group_name() . '<br/>';
                }
            }
?>
      </td>

      <!-- signature group -->
      <td>
<?php
            foreach($policy->get_plugingroups($conn, $policy->get_id()) as $group) {
                    echo $group['name'] . '<br/>';
            }
?>
      </td>
      
      <!-- sensors -->
      <td>
<?php
            if ($sensor_list = $policy->get_sensors ($conn)) {
                foreach($sensor_list as $sensor) {
                    echo $sensor->get_sensor_name() . '<br/>';
                }
            }
?>
      </td>

      <td>
<?php
            $policy_time = $policy->get_time($conn);
            
            $begin_day = $policy_time->get_begin_day();
            if     ($begin_day == 1) $begin_day_char = _("Mon");
            elseif ($begin_day == 2) $begin_day_char = _("Tue");
            elseif ($begin_day == 3) $begin_day_char = _("Wed");
            elseif ($begin_day == 4) $begin_day_char = _("Thu");
            elseif ($begin_day == 5) $begin_day_char = _("Fri");
            elseif ($begin_day == 6) $begin_day_char = _("Sat");
            elseif ($begin_day == 7) $begin_day_char = _("Sun");
            
            $end_day = $policy_time->get_end_day();
            if     ($end_day == 1) $end_day_char = _("Mon");
            elseif ($end_day == 2) $end_day_char = _("Tue");
            elseif ($end_day == 3) $end_day_char = _("Wed");
            elseif ($end_day == 4) $end_day_char = _("Thu");
            elseif ($end_day == 5) $end_day_char = _("Fri");
            elseif ($end_day == 6) $end_day_char = _("Sat");
            elseif ($end_day == 7) $end_day_char = _("Sun");
            
            echo $begin_day_char . " " .
                 $policy_time->get_begin_hour() . "h - " .
                 $end_day_char . " " .
                 $policy_time->get_end_hour() . "h";
?>
      </td>

      <td><?php echo $policy->get_descr(); ?></td>
      <td><?php if($policy->get_store() == 1){
      echo _("Yes");
      } elseif ($policy->get_store() == 0){
      echo _("No");
      }?></td>

      <td>
        <a href="modifypolicyform.php?id=<?php
            echo $policy->get_id()?>">
	    <?php echo gettext("Modify"); ?> </a>
        <a href="deletepolicy.php?id=<?php
            echo $policy->get_id()?>">
	    <?php echo gettext("Delete"); ?> </a></td>
      
    </tr>
<?php
        } /* foreach */
    } /* if */

    $db->close($conn);
?>

  <tr>
    <td colspan="10">
        <a href="newpolicyform.php"> <?php echo gettext("Insert new policy"); ?> </a>
    </td>
  </tr>
  <tr>
    <td colspan="10"><a href="../conf/reload.php?what=policies"> <?php echo gettext("Reload"); ?> </a></td>
  </tr>
  </table>
    
</body>
</html>

