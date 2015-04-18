<html>
<head>
  <title> Control Panel </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center">Anomalies</h1>

<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Host.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Host_mac.inc');
require_once ('classes/Sensor.inc');

function echo_values($val, $max, $ip, $image) {

    global $acid_link;

    if ($val - $max > 0) {
        echo "<a href=\"". get_acid_info($ip, $acid_link) . 
            "\"><font color=\"#991e1e\">$val</font></a>/" . 
            "<a href=\"$image\">" . intval($val * 100 / $max) ."</a>%";
    } else {
        echo "<a href=\"". get_acid_info($ip, $acid_link) .
             "\">$val</a>/" . 
            "<a href=\"$image\">" . intval($val * 100 / $max) ."</a>%";
    } 
}

/* get conf */
$conf = new ossim_conf();
$mrtg_link = $conf->get_conf("mrtg_link");
$graph_link = $conf->get_conf("graph_link");
$acid_link = $conf->get_conf("acid_link");
$ntop_link = $conf->get_conf("ntop_link");
$opennms_link = $conf->get_conf("opennms_link");
$stats_link = $conf->get_conf("stats_link");
$mailstats_link = $conf->get_conf("mailstats_link");

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

?>

  <table align="center" width="100%">
    <table width="100%">
    <tr>
    <td colspan = 8>
    <table align="center" width="100%">
    <tr>
    <th colspan=4><u>RRD anomalies</u> <a name="Anomalies" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#Anomalies" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>
    </th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=1">Acknowledged</A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=0">Not Acknowledged</A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=-1">All</A></th>
    </tr>
    <tr>
    <th> Host </th><th> What </th><th> When </th>
    <th> Not acked count (hours)</th><th> Over threshold (absolute)</th>
    <th align="center">Ack</th>
    <th align="center">Delete</th>
    </tr>

<form action="handle_anomaly.php" method="GET">
<?php
require_once 'ossim_db.inc';
require_once 'classes/RRD_config.inc';
require_once 'classes/RRD_anomaly.inc';
require_once 'classes/RRD_anomaly_global.inc';

$db = new ossim_db();
$conn = $db->connect();
$where_clause = "where acked = 0";
switch ($_GET["acked"]){
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

$perl_interval = 3600 / 300;

if ($alert_list_global = RRD_anomaly_global::get_list($conn, $where_clause,
"order by anomaly_time desc")) {
    foreach($alert_list_global as $alert) {
    $ip = "Global";
    if($rrd_list_temp = RRD_config::get_list($conn, "WHERE ip = 0")) {
    $rrd_temp = $rrd_list_temp[0];
    }
/*    if(($alert->get_count() / $perl_interval) <
    ($rrd_temp->get_col($alert->get_what(), "persistence")) && $_GET["acked"] != -1) {
    continue;
    } */
?>
<tr>
<th> 

<A HREF="<?php echo
"$ntop_link/plugins/rrdPlugin?action=list&key=interfaces/eth0&title=interface%20eth0";?>" target="_blank"> 
<?php echo $ip;?></A> </th><td> <?php echo $rrd_names_global[$alert->get_what()];?></td>
<td> <?php echo $alert->get_anomaly_time();?></td>
<td> <?php echo round(($alert->get_count())/$perl_interval);?>h. </td>
<td><font color="red"><?php echo ($alert->get_over()/$rrd_temp->get_col($alert->get_what(),"threshold"))*100;?>%</font>/<?php echo $alert->get_over();?></td>
<td align="center"><input type="checkbox" name="ack,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
<td align="center"><input type="checkbox" name="del,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
</tr>
<?php }}

?>
<tr><th colspan="8"><hr noshade></th></tr>
<tr>
<th> Host </th><th> What </th><th> When </th>
<th> Not acked count (hours)</th><th> Over threshold (absolute)</th>
<th align="center">Ack</th>
<th align="center">Delete</th>
</tr>
<?php

$perl_interval = 4; // Host perl is being executed every 15 minutes
if ($alert_list = RRD_anomaly::get_list($conn, $where_clause, "order by
anomaly_time desc")) {
    foreach($alert_list as $alert) {
    $ip = $alert->get_ip();
    if($rrd_list_temp = RRD_config::get_list($conn, 
                                             "where ip = inet_ntoa('$ip')"))
    {
        $rrd_temp = $rrd_list_temp[0];
    }
    /*
    if(($alert->get_count() / $perl_interval) < ($rrd_temp->get_col($alert->get_what(), "persistence")) && $_GET["acked"] != -1) {
    continue;
    }
    */


?>
<tr>
<th>
<A HREF="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A></th><td> <?php echo $alert->get_what();?></td>
<td> <?php echo $alert->get_anomaly_time();?></td>
<td> <?php echo round(($alert->get_count())/$perl_interval);?>h. </td>
<td><font color="red"><?php echo 0;//echo ($alert->get_over()/$rrd_temp->get_col($alert->get_what(),"threshold"))*100;?>%</font>/<?php echo $alert->get_over();?></td>
<td align="center"><input type="checkbox" name="ack,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
<td align="center"><input type="checkbox" name="del,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
</tr>
<?php }}?>
<tr>
<td align="center" colspan="7">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    </table>
    <br/>
    <table width="100%">
    <tr>
    <td colspan="8">
    <table width="100%">
    <tr><th colspan="6"><u>OS Changes</u> <a name="OS" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#OS" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>  
        &nbsp;&nbsp;[ <a href="os.php" target="_blank"> Get list </a> ]
    </th></tr>
    <tr><th> Host </th><th colspan="1"> OS </th><th colspan="1"> Previous OS
    </th><th> When </th><th> Ack </th><th> Ignore </th></tr>
<form action="handle_os.php" method="GET">
<?php
if ($host_os_list = Host_os::get_list($conn, "where anom = 1 and os != previous", "")) {
    foreach($host_os_list as $host_os) {
    $ip = $host_os->get_ip();
    $date = $host_os->get_date();
    $os = $host_os->get_os();
    if(ereg("\|",$os)){
    $os = ereg_replace("\|", " or ", $os);
    }
    $previous = $host_os->get_previous();
    if(ereg("\|",$previous)){
    $previous = ereg_replace("\|", " or ", $previous);
    }
    
    ?>

<tr><th>
<A HREF="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A>
</th>
<td colspan="1"><font color="red"><?php echo $os;?></font></td>
<td colspan="1"><?php echo $previous;?></td>
<td colspan="1"><?php echo $date;?></td>
<td>
<?php $encoded = base64_encode("ack" . $os);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>
<td>
<?php $encoded = base64_encode("ignore" . $previous);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>

<?php
}}
?>
<tr>
<td align="center" colspan="6">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    </table>
    <br/>

    <!-- Mac detection -->
    <table width="100%">
    <tr>
    <td colspan="8">
    <table width="100%">
    <tr><th colspan="6"><u>Mac Changes</u> <a name="Mac" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#Mac" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>  
        &nbsp;&nbsp;[ <a href="mac.php" target="_blank"> Get list </a> ]
    </th></tr>
    <tr><th> Host </th><th colspan="1"> Mac </th><th colspan="1"> Previous Mac
    </th><th> When </th><th> Ack </th><th> Ignore </th></tr>
<form action="handle_mac.php" method="GET">
<?php
if ($host_mac_list = Host_mac::get_list($conn, "where anom = 1 and mac != previous", "")) {
    foreach($host_mac_list as $host_mac) {
    $ip = $host_mac->get_ip();
    $date = $host_mac->get_date();
    $mac = $host_mac->get_mac();
    if(ereg("\|",$mac)){
    $mac = ereg_replace("\|", " or ", $mac);
    }
    $previous = $host_mac->get_previous();
    if(ereg("\|",$previous)){
    $previous = ereg_replace("\|", " or ", $previous);
    }
    
    ?>

<tr><th>
<A HREF="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A>
</th>
<td colspan="1"><font color="red"><?php echo $mac;?></font></td>
<td colspan="1"><?php echo $previous;?></td>
<td colspan="1"><?php echo $date;?></td>
<td>
<?php $encoded = base64_encode("ack" . $mac);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>
<td>
<?php $encoded = base64_encode("ignore" . $previous);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>


<?php
}}
?>
<tr>
<td align="center" colspan="6">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    <!-- end Mac detection -->


</table>

<br/>
 </table>

<?php
$db->close($conn);
?>

</body>
</html>


