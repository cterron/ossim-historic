<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

  <h2>Policy</h2>

  <table align="center">
    <tr>
      <th>Source</th>
      <th>Dest</th>
      <th>Priority</th>
      <th>Port Group</th>
      <th>Sig Group</th>
      <th>Sensors</th>
      <th>Time Range</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php
    require_once ('classes/Policy.inc');
    require_once ('classes/Host.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    if ($policy_list = Policy::get_list($conn)) {
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

      <!-- asset -->
      <td><?php echo $policy->get_priority(); ?></td>

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
            if ($sig_list = $policy->get_signatures ($conn)) {
                foreach($sig_list as $sig_group) {
                    echo $sig_group->get_sig_group_name() . '<br/>';
                }
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
            if     ($begin_day == 1) $begin_day_char = "Mon";
            elseif ($begin_day == 2) $begin_day_char = "Tue";
            elseif ($begin_day == 3) $begin_day_char = "Wed";
            elseif ($begin_day == 4) $begin_day_char = "Thu";
            elseif ($begin_day == 5) $begin_day_char = "Fri";
            elseif ($begin_day == 6) $begin_day_char = "Sat";
            elseif ($begin_day == 7) $begin_day_char = "Sun";
            
            $end_day = $policy_time->get_end_day();
            if     ($end_day == 1) $end_day_char = "Mon";
            elseif ($end_day == 2) $end_day_char = "Tue";
            elseif ($end_day == 3) $end_day_char = "Wed";
            elseif ($end_day == 4) $end_day_char = "Thu";
            elseif ($end_day == 5) $end_day_char = "Fri";
            elseif ($end_day == 6) $end_day_char = "Sat";
            elseif ($end_day == 7) $end_day_char = "Sun";
            
            echo $begin_day_char . " " .
                 $policy_time->get_begin_hour() . "h - " .
                 $end_day_char . " " .
                 $policy_time->get_end_hour() . "h";
?>
      </td>

      <td><?php echo $policy->get_descr(); ?></td>

      <td>
        <a href="modifypolicyform.php?id=<?php
            echo $policy->get_id()?>">Modify</a>
        <a href="deletepolicy.php?id=<?php
            echo $policy->get_id()?>">Delete</a></td>
      
    </tr>
<?php
        } /* foreach */
    } /* if */

    $db->close($conn);
?>

  <tr>
    <td colspan="9">
        <a href="newpolicyform.php">Insert new policy</a>
    </td>
  </tr>

  </table>
    
</body>
</html>

