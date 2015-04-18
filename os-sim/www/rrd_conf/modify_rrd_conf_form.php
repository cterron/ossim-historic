<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify RRD Config</h1>
    
  <h3>Hints</h3>
  <ul>
  <li> Threshold: Absolute value above which is being alerted.
  <li> Priority: Resulting impact if threshold is being exceeded.
  <li> Alpha: Intercept adaption parameter.
  <li> Beta: Slope adaption parameter.
  <li> Persistence: How long has this event to last before we alert. (Hours)
  </ul>

<?php
    require_once 'classes/RRD_conf.inc';
    require_once 'classes/RRD_conf_global.inc';
    require_once 'classes/RRD_data.inc';
    require_once 'classes/Host.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$ip = mysql_escape_string($_GET["ip"])) {
        echo "<p>Wrong ip</p>";
        exit;
    }

    $global = 0;

    if($ip == "global"){
    $global = 1;
        if($rrd_list = RRD_conf_global::get_list($conn, "")) {
            $rrd = $rrd_list[0];
        }
    } else {
        if ($rrd_list = RRD_conf::get_list($conn, "WHERE ip = '$ip'")) {
            $rrd = $rrd_list[0];
        }
    }

    $db->close($conn);
?>

<form method="post" action="modify_rrd_conf.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>IP</th>
    <?php 
    if($global){
    ?>
     <input type="hidden" name="ip" value="global">
    <th class="center">
      <font color="blue"><b>Global</b></font>
    <?php
    } else {
    ?>
        <input type="hidden" name="ip" 
               value="<?php echo $rrd->get_ip(); ?>">
    <th class="center">
     <font color="blue"><b><?php echo Host::ip2hostname($conn,$ip);?></b></font>
<?php } ?>
    </th>
  </tr>
  <tr>
  <th>Modify</th><th> Threshold / Priority / Alpha / Beta / Persistence</th>
  </tr>
    <?php
    if($global) {
        $count_values = count($rrd_values_global);
        $count_names = count($rrd_names_global);
        if($count_values != $count_names){
            print "Consistency check failed, please check RRD_data.inc\n";
            exit;
        }
        $temp_values = &$rrd_values_global;
        $temp_names = &$rrd_names_global;
    } else {
        $count_values = count($rrd_values);
        $count_names = count($rrd_names);
        if($count_values != $count_names){
            print "Consistency check failed, please check RRD_data.inc\n";
            exit;
        }
        $temp_values = &$rrd_values;
        $temp_names = &$rrd_names;
    }
    foreach($temp_names as $key => $value) {
    ?>
    <tr>
    <th><?php print $value?></th>
    <td class="center">

      <input type="text" name="<?php echo $key?>_threshold" size="5" 
             value="<?php echo $rrd->get_col($key, "threshold");?>">
      <input type="text" name="<?php echo $key?>_priority" size="5" 
             value="<?php echo $rrd->get_col($key, "priority");?>">
      <input type="text" name="<?php echo $key?>_alpha" size="5" 
             value="<?php echo $rrd->get_col($key, "alpha"); ?>">
      <input type="text" name="<?php echo $key?>_beta" size="5" 
             value="<?php echo $rrd->get_col($key, "beta"); ?>">
      <input type="text" name="<?php echo $key?>_persistence" size="5" 
             value="<?php echo $rrd->get_col($key, "persistence"); ?>">
    </td>
  </tr>
<?php
    }
    ?>
   <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>
