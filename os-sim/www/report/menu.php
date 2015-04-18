<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php
    require_once ('ossim_conf.inc');
    require_once ('ossim_db.inc');
    require_once ('classes/Sensor.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    $conf = new ossim_conf();

    $ip = $_GET["host"];
    $ip_slashed = str_replace(".", "/", $ip);

    $acid_link = $conf->get_conf("acid_link");
    $acid_main_link = $conf->get_conf("acid_link") .
        "acid_stat_ipaddr.php?ip=$ip&netmask=32";
    $interface = $conf->get_conf("ossim_interface");
?>

<br/>
&nbsp;<font style="font-size: 12pt; font-weight: bold;">Host Report</font><br/><br/>

&nbsp;&nbsp;<a href="inventory.php?host=<?php 
    echo $_GET["host"] ?>" target="report">Inventory</a><br/><br/>
    
&nbsp;&nbsp;<a href="metrics.php?host=<?php 
    echo $_GET["host"] ?>" target="report">Metrics</a><br/><br/>
    
&nbsp;&nbsp;<b>Alarms</b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $_GET["host"] ?>&dst_ip=<?php echo $_GET["host"] ?>" 
    target="report">Source or Dest</a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?src_ip=<?php 
    echo $_GET["host"] ?>" target="report">Source</a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../control_panel/alarm_console.php?dst_ip=<?php 
    echo $_GET["host"] ?>" target="report">Destination</a><br/><br/>
    
&nbsp;&nbsp;<b>Alerts</b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo $acid_main_link ?>"
    target="report">Main</a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/acid_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_src&ip_addr[0][2]==&ip_addr[0][3]=".$_GET["host"]."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">Src Unique alerts</a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="<?php echo "$acid_link/acid_stat_alerts.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr[0][1]=ip_dst&ip_addr[0][2]==&ip_addr[0][3]=".$_GET["host"]."&ip_addr_cnt=1&sort_order=time_d" ?>"
    target="report">Dst Unique alerts</a><br/><br/>

&nbsp;&nbsp;<b>Vulnerabilites</b><br/><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/index.php?noimages=1&host=<?php
    echo $_GET["host"] ?>"
    target="report">Vulnmeter</a><br/>
&nbsp;&nbsp;&nbsp;&nbsp;<a href="../vulnmeter/last/<?php 
    echo ereg_replace("\.","_", $_GET["host"]); ?>/index.html"
    target="report">Security Problems</a><br/><br/>

&nbsp;&nbsp;<a href="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/$ip.html" ?>" target="report">Usage</a><br/><br/>
&nbsp;&nbsp;<a href="<?php echo Sensor::get_sensor_link($conn, $ip) . 
    "/plugins/rrdPlugin?action=list&key=interfaces/$interface/hosts/$ip_slashed&title=host%20$ip" ?>" target="report">Anomalies</a><br/><br/>

<?php
    $db->close($conn);
?>

</body>
</html>

