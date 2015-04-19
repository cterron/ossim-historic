<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php
    require_once ('ossim_conf.inc');
    require_once ('ossim_db.inc');
    require_once ('classes/Sensor.inc');
    require_once ('classes/Sensor_interfaces.inc');
    require_once ('classes/Host_vulnerability.inc');
    require_once ('classes/Host.inc');
    require_once ('classes/Net.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = $GLOBALS["CONF"];

    $ip = validateVar($_GET["host"], OSS_IP);
    $iphost = $ip;
    $ip_slashed = str_replace(".", "/", $ip);

    $acid_link = $conf->get_conf("acid_link");
    $acid_prefix = $conf->get_conf("event_viewer");
    $acid_main_link = $conf->get_conf("acid_link") . "/" . $acid_prefix .
        "_stat_ipaddr.php?ip=$ip&netmask=32";
    $interface = $conf->get_conf("ossim_interface");
?>

<br/>
&nbsp;<font style="font-size: 12pt; font-weight: bold;">Host Report</font><br/><br/>

&nbsp;&nbsp;<a href="inventory.php?host=<?php 
    echo $iphost ?>" target="report">
    <?php echo gettext("Inventory"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<a href="metrics.php?host=<?php 
    echo $iphost ?>" target="report">
    <?php echo gettext("Metrics"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<b> <?php echo gettext("Alarms"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $iphost ?>&dst_ip=<?php echo $iphost ?>" 
    target="report">
    <?php echo gettext("Source or Dest"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $iphost ?>" target="report">
    <?php echo gettext("Source"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?dst_ip=<?php 
    echo $iphost ?>" target="report">
    <?php echo gettext("Destination"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<b> <?php echo gettext("Events"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo $acid_main_link ?>"
    target="report">
    <?php echo gettext("Main"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/".$acid_prefix."_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_src&ip_addr[0][2]==&ip_addr[0][3]=".$iphost."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">
    <?php echo gettext("Src Unique events"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/".$acid_prefix."_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_dst&ip_addr[0][2]==&ip_addr[0][3]=".$iphost."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">
    <?php echo gettext("Dst Unique events"); ?> </a><br/><br/>

<?php 
if(Host_vulnerability::in_host_vulnerability($conn, $ip)){
?>
&nbsp;&nbsp;<b>Vulnerabilites</b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/index.php?noimages=1&host=<?php
    echo $iphost ?>"
    target="report">
    <?php echo gettext("Vulnmeter"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/last/<?php 
    echo ereg_replace("\.","_", $iphost); ?>/index.html"
    target="report">
    <?php echo gettext("Security Problems"); ?> </a><br/><br/>
<?php }
?>
&nbsp;&nbsp;<a href="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html" ?>" target="report">
    <?php echo gettext("Usage"); ?> </a><br/><br/>
    <?php
    if((Host::in_host($conn, $ip)) || (Net::isIpInAnyNet($conn, $ip))){
    ?>
&nbsp;&nbsp;<a href="<?php 
$interface = Sensor::get_sensor_interface($conn,$ip);
echo Sensor::get_sensor_link($conn, $ip) . 
    "/plugins/rrdPlugin?action=list&key=interfaces/$interface/hosts/$ip_slashed&title=host%20$ip" ?>" target="report">
    <?php echo gettext("Anomalies"); ?> </a><br/><br/>

<?php
}
    $db->close($conn);
?>

</body>
</html>

