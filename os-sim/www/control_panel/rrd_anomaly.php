<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAnomalies");
?>

<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>


<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Host.inc');
require_once ('classes/Net.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Util.inc');
require_once ('classes/RRD_config.inc');
require_once ('classes/RRD_anomaly.inc');
require_once ('classes/RRD_anomaly_global.inc');

function echo_values($val, $max, $ip, $image) {

    global $acid_link;
    global $acid_prefix;

    if ($val - $max > 0) {
        echo "<a href=\"". Util::get_acid_info($ip, $acid_link, $acid_prefix) . 
            "\"><font color=\"#991e1e\">$val</font></a>/" . 
            "<a href=\"$image\">" . intval($val * 100 / $max) ."</a>%";
    } else {
        echo "<a href=\"". Util::get_acid_info($ip, $acid_link, $acid_prefix) .
             "\">$val</a>/" . 
            "<a href=\"$image\">" . intval($val * 100 / $max) ."</a>%";
    } 
}

/* get conf */
$conf = $GLOBALS["CONF"];
$graph_link = $conf->get_conf("graph_link");
$acid_link = $conf->get_conf("acid_link");
$acid_prefix = $conf->get_conf("event_viewer");
$ntop_link = $conf->get_conf("ntop_link");
$nagios_link = $conf->get_conf("nagios_link");

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

?>

<body>

  <h1 align="center"> <?php echo gettext("RRD Anomalies"); ?> </h1>
  <table align="center" width="100%">
    <table width="100%">
    <tr>
    <td colspan = 8>

<form action="handle_anomaly.php" method="GET">
<?php

$where_clause = "where acked = 0";
switch (GET('acked')){
    case -1:
    $where_clause = "";
    break;
    case 0:
    $where_clause = "where acked = 0";
    break;
    case 1:
    $where_clause = "where acked = 1";
    break;
}
?>

<tr></tr>
<tr><th colspan="8"><?php echo gettext("RRD anomalies");?>
<a name="Anomalies" href="<?php echo $_SERVER["PHP_SELF"]?>?#Anomalies" title=" <?php echo gettext("Fix"); ?> "><img src="../pixmaps/Hammer2.png" width="24" border="0"></a>
</th>
</tr>
<tr>

    <th colspan=4>&nbsp;</th>
<th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=1"> <?php echo gettext("Acknowledged"); ?> </A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=0"> <?php echo gettext("Not Acknowledged"); ?> </A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=-1"> <?php echo gettext("All"); ?> </A></th>
</tr>
<tr>
<th> <?php echo gettext("Host"); ?> </th><th> <?php echo gettext("What"); ?> </th><th> <?php echo gettext("When"); ?> </th>
<th> <?php echo gettext("Not acked count (hours)"); ?> </th><th> <?php echo gettext("Over threshold (absolute)"); ?> </th>
<th align="center"> <?php echo gettext("Ack"); ?> </th>
<th align="center"> <?php echo gettext("Delete"); ?> </th>
</tr>
<?php

$perl_interval = 4; // Host perl is being executed every 15 minutes
$count = RRD_anomaly::get_list_count($conn);




if ($event_list = RRD_anomaly::get_list($conn, $where_clause, "order by
anomaly_time desc","0", $count)) {
    foreach($event_list as $event) {
    $ip = $event->get_ip();
?>
<tr>
<th>
<A HREF="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A></th><td> <?php echo $event->get_what();?></td>
<td> <?php echo $event->get_anomaly_time();?></td>
<td> <?php echo round(($event->get_count())/$perl_interval);?>h. </td>
<td><font color="red"><?php echo 0;//echo ($event->get_over()/$rrd_temp->get_col($event->get_what(),"threshold"))*100;?>%</font>/<?php echo $event->get_over();?></td>
<td align="center"><input type="checkbox" name="ack,<?php echo $ip?>,<?php
echo $event->get_what();?>"></input></td>
<td align="center"><input type="checkbox" name="del,<?php echo $ip?>,<?php
echo $event->get_what();?>"></input></td>
</tr>
<?php }}?>
<tr>
<td align="center" colspan="7">
<input type="submit" value=" <?php echo gettext("OK"); ?> ">
<input type="reset" value=" <?php echo gettext("reset"); ?> ">
</td>
</tr>
</form>
<br/>
 </table>

<?php
$db->close($conn);
?>

</body>
</html>
