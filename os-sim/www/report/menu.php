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
    require_once ('classes/Util.inc');

    require_once 'classes/Security.inc';

    $host = GET('host');
    
    ossim_valid($host, OSS_IP_ADDR, 'illegal:'._("Host"));

    if (ossim_error()) {
        die(ossim_error());
    }

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = $GLOBALS["CONF"];

    $ip_slashed = str_replace(".", "/", $host);

    $acid_link = $conf->get_conf("acid_link");
    $acid_prefix = $conf->get_conf("event_viewer");
    $acid_main_link = $conf->get_conf("acid_link") . "/" . $acid_prefix .
        "_stat_ipaddr.php?ip=$host&netmask=32";
    $interface = $conf->get_conf("ossim_interface");
?>

<br/>
&nbsp;<font style="font-size: 12pt; font-weight: bold;">
<?php echo gettext("Host Report"); ?>
</font><br/><br/>

&nbsp;&nbsp;<a href="inventory.php?host=<?php 
    echo $host ?>&origin=passive" target="report">
    <?php echo gettext("Inventory"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<a href="metrics.php?host=<?php 
    echo $host ?>" target="report">
    <?php echo gettext("Metrics"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<b> <?php echo gettext("Alarms"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $host ?>&dst_ip=<?php echo $host ?>" 
    target="report">
    <?php echo gettext("Source or Dest"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $host ?>" target="report">
    <?php echo gettext("Source"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?dst_ip=<?php 
    echo $host ?>" target="report">
    <?php echo gettext("Destination"); ?> </a><br/><br/>
    
&nbsp;&nbsp;<b> <?php echo gettext("Events"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo $acid_main_link ?>"
    target="report">
    <?php echo gettext("Main"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/".$acid_prefix."_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_src&ip_addr[0][2]==&ip_addr[0][3]=".$host."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">
    <?php echo gettext("Src Unique events"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/".$acid_prefix."_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_dst&ip_addr[0][2]==&ip_addr[0][3]=".$host."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">
    <?php echo gettext("Dst Unique events"); ?> </a><br/><br/>

<?php 
if(Host_vulnerability::in_host_vulnerability($conn, $host)){
?>
&nbsp;&nbsp;<b>Vulnerabilites</b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/index.php?noimages=1&host=<?php echo $host ?>" target="report">
<?php
    $ip_stats = Host_vulnerability::get_list ($conn, "WHERE ip = \"$host\"", "ORDER BY scan_date DESC", $ggregated = true, 1);
        foreach($ip_stats as $host_vuln){
             $scan_date = $host_vuln->get_scan_date();
        }
?>
    <?php echo gettext("Vulnmeter"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/<?php echo date("YmdHis", strtotime($scan_date)); ?>/<?php
    echo ereg_replace("\.","_", $host); ?>/index.html"
    target="report">
    <?php echo gettext("Security Problems"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../incidents/index.php?ref=Vulnerability&status=Open&filter=OK&order_by=life_time&with_text=<?php echo $host; ?>" target="report">
    <?php echo gettext("Incidents"); ?> </a><br/><br/>
<?php }
?>
&nbsp;&nbsp;<a href="<?php echo Sensor::get_sensor_link($conn, $host) .
    "/$host.html" ?>" target="report">
    <?php echo gettext("Usage"); ?> </a><br/><br/>
    <?php
    if((Host::in_host($conn, $host)) || (Net::isIpInAnyNet($conn, $host))){
    ?>
&nbsp;&nbsp;
<a href="<?php 
$interface = Sensor::get_sensor_interface($conn,$host);
echo Sensor::get_sensor_link($conn, $host) . 
    "/plugins/rrdPlugin?action=list&key=interfaces/$interface/hosts/$ip_slashed&title=host%20$host" ?>" target="report">
    <?php echo gettext("Anomalies"); ?> </a><br/><br/>

<?php
}
    $db->close($conn);
?>

</body>
</html>

