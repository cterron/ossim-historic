<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php 
    /* get host */
    if (!$ip = $_GET["host"]) {
        echo "<p>Wrong ip</p>";
        exit;
    }
?>

<h1>Metrics - <?php echo $ip ?></h1>

<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Conf.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Control_panel_host.inc');
require_once ('classes/Host.inc');


function bgcolor($value, $max)
{
    if ($value / 5 > $max)      return "red";
    elseif ($value / 3 > $max)  return "orange";
    elseif ($value / 1 > $max)  return "green";
    else                        return "white";
}

function fontcolor($value, $max)
{
    if ($value / 5 > $max)      return "white";
    elseif ($value / 3 > $max)  return "black";
    elseif ($value / 1 > $max)  return "white";
    else                        return "black";
}


$conf = new ossim_conf();
$graph_link = $conf->get_conf("graph_link");

$image1 = "$graph_link?ip=$ip&what=compromise&start=N-24h&end=N&type=host&zoom=1";
$image2 = "$graph_link?ip=$ip&what=compromise&start=N-7D&end=N&type=host&zoom=1";
$image3 = "$graph_link?ip=$ip&what=compromise&start=N-1M&end=N&type=host&zoom=1";
$image4 = "$graph_link?ip=$ip&what=compromise&start=N-1Y&end=N&type=host&zoom=1";

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

/* get thresholds */
if ($list = Host::get_list($conn, "WHERE ip = '$ip'")) {
    $threshold_c = $list[0]->get_threshold_c();
    $threshold_a = $list[0]->get_threshold_a();
} else {
    $framework_conf = Conf::get_conf($conn);
    $threshold_c = $threshold_a = $framework_conf->get_threshold();
}

/* max C */
$list = Control_panel_host::get_list($conn,
    "WHERE id = '$ip' ORDER BY time_range", 3);
    
if ($list[0]) {
    $max_c["day"]   = $list[0]->get_max_c();
    $max_c_date["day"]   = $list[0]->get_max_c_date();
}
if ($list[1]) {
    $max_c["month"] = $list[1]->get_max_c();
    $max_c_date["month"] = $list[1]->get_max_c_date();
}
if ($list[2]) {
    $max_c["year"]  = $list[2]->get_max_c();
    $max_c_date["year"]  = $list[2]->get_max_c_date();
}

/* max A */
$list = Control_panel_host::get_list($conn, 
    "WHERE id = '$ip' ORDER BY time_range", 3);
if ($list[0]) {
    $max_a["day"]   = $list[0]->get_max_a();
    $max_a_date["day"]   = $list[0]->get_max_a_date();
}
if ($list[1]) {
    $max_a["month"] = $list[1]->get_max_a();
    $max_a_date["month"] = $list[1]->get_max_a_date();
}
if ($list[2]) {
    $max_a["year"]  = $list[2]->get_max_a();
    $max_a_date["year"]  = $list[2]->get_max_a_date();
}

/* current C */
$current_c = Host_qualification::get_ip_compromise($conn, $ip);

/* current A */
$current_a = Host_qualification::get_ip_attack($conn, $ip);

?>

    <table align="center">
      <tr>
        <th>Current C Level&nbsp;</th>
        <td bgcolor="<?php echo bgcolor($current_c, $threshold_c) ?>">
          <font color="<?php echo fontcolor($current_c, $threshold_c) ?>">
            <b><?php echo $current_c ?></b></font></td>
      </tr>
      <tr>
        <th>Current A Level&nbsp;</th>
        <td bgcolor="<?php echo bgcolor($current_a, $threshold_a) ?>">
          <font color="<?php echo fontcolor($current_a, $threshold_a) ?>">
            <b><?php echo $current_a ?></b></font></td>
      </tr>
    </table><br/>
    <table align="center">
<?php 
    if ($max_c["day"]) { 
?>
      <tr>
        <th>Max C Level (last day)</th>
        <td bgcolor="<?php echo bgcolor($max_c["day"], $threshold_c) ?>">
          <font color="<?php echo fontcolor($max_c["day"], $threshold_c) ?>">
            <b><?php echo $max_c["day"] ?></b></font></td>
        <td><?php echo $max_c_date["day"] ?></td>
      </tr>
<?php 
    }
    if ($max_a["day"]) {
?>
    
      <tr>
        <th>Max A Level (last day)</th>
        <td bgcolor="<?php echo bgcolor($max_a["day"], $threshold_a) ?>">
          <font color="<?php echo fontcolor($max_a["day"], $threshold_a) ?>">
            <b><?php echo $max_a["day"] ?></b></font></td>
        <td><?php echo $max_a_date["day"] ?></td>
      </tr>
      <tr><td colspan="2"></td></tr>
<?php
    }
    if ($max_c["month"]) {
?>
      <tr>
        <th>Max C Level (last month)</th>
        <td bgcolor="<?php echo bgcolor($max_c["month"], $threshold_c) ?>">
          <font color="<?php echo fontcolor($max_c["month"], $threshold_c) ?>">
            <b><?php echo $max_c["month"] ?></b></font></td>
        <td><?php echo $max_c_date["month"] ?></td>
      </tr>
<?php
    }
    if ($max_a["month"]) {
?>
      <tr>
        <th>Max A Level (last month)</th>
        <td bgcolor="<?php echo bgcolor($max_a["month"], $threshold_a) ?>">
          <font color="<?php echo fontcolor($max_a["month"], $threshold_a) ?>">
            <b><?php echo $max_a["month"] ?></b></font></td>
        <td><?php echo $max_a_date["month"] ?></td>
      </tr>
      <tr><td colspan="2"></td></tr>
<?php
    }
    if ($max_c["year"]) {
?>
      <tr>
        <th>Max C Level (last year)</th>
        <td bgcolor="<?php echo bgcolor($max_c["year"], $threshold_c) ?>">
          <font color="<?php echo fontcolor($max_c["year"], $threshold_c) ?>">
            <b><?php echo $max_c["year"] ?></b></font></td>
        <td><?php echo $max_c_date["year"] ?></td>
      </tr>
<?php
    }
    if ($max_a["year"]) {
?>
      <tr>
        <th>Max A Level (last year)</th>
        <td bgcolor="<?php echo bgcolor($max_a["year"], $threshold_a) ?>">
          <font color="<?php echo fontcolor($max_a["year"], $threshold_a) ?>">
            <b><?php echo $max_a["year"] ?></b></font></td>
        <td><?php echo $max_a_date["year"] ?></td>
      </tr>
<?php
    }
?>
    </table>

    <p align="center">
      <b>Last day</b><br/>
      <img src="<?php echo $image1 ?>"/><br/><br/>
      
      <b>Last week</b><br/>
      <img src="<?php echo $image2 ?>"/><br/><br/>
      
      <b>Last month</b><br/>
      <img src="<?php echo $image3 ?>"/><br/><br/>
      
      <b>Last year</b><br/>
      <img src="<?php echo $image4 ?>"/><br/><br/>
    </p>

<?php
    $db->close($conn);
?>

</body>
</html>

