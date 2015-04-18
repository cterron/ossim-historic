<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php
if (!$sensor = $_GET["sensor"])
{
    echo "<p align=\"center\">Argument missing</p>";
    exit();
}

require_once ('ossim_conf.inc');
$conf = new ossim_conf();

# get ntop port from default ntop entry at
# /etc/ossim/framework/ossim.conf
# a better solution ??
list($proto, $ip, $port) = split(':', $conf->get_conf("ntop_link"));

require_once ('ossim_db.inc');
require_once ('classes/Sensor.inc');

$db = new ossim_db();
$conn = $db->connect();
?>

<form method="GET" action="menu.php">
<input type="hidden" name="proto" value="<?php echo $proto ?>"/>
<input type="hidden" name="port" value="<?php echo $port ?>"/>
Sensor:&nbsp;
<select name="sensor" onChange="submit()">
<?php
if ($sensor_list = Sensor::get_list($conn)) {
    foreach ($sensor_list as $s) {
?>
  <option 
<?php 
    if ($sensor == $s->get_ip()) echo " SELECTED ";
?>
    value="<?php echo $s->get_ip() ?>"><?php 
        echo $s->get_name() ?></option>
<?php
    }
}
?>
</select>
</form>



<?php
require_once ('ossim_conf.inc');
$conf = new ossim_conf();
?>

<a href="<?php echo "$proto://$sensor:$port"?>/trafficStats.html"
       target="ntop">Global</a></br>
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataProtos.html"
       target="ntop">Protocols</a><br/><br/>

<b>Services</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataIP.html"
   target="ntop">By host: Total</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataSentIP.html"
   target="ntop">By host: Sent</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataReceivedIP.html"
   target="ntop">By host: Recv</a><br/>
   
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipProtoDistrib.html"
  target="ntop">Service statistic</a><br/>

&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipProtoUsage.html"
  target="ntop">By client-server</a><br/><br/>

<b>Throughput</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataThpt.html?col=1"
   target="ntop">By host: Total</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataSentThpt.html?col=1"
   target="ntop">By host: Sent</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataReceivedThpt.html?col=1"
   target="ntop">By host: Recv</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/thptStats.html?col=1"
   target="ntop">Total (Graph)</a><br/><br/>

<b>Matrix</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipTrafficMatrix.html"
   target="ntop">Data Matrix</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/dataHostTraffic.html"
   target="ntop">Time Matrix</a><br/><br/>
   
<b>Gateways, VLANs</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/localRoutersList.html"
   target="ntop">Gateways</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/vlanList.html"
   target="ntop">VLANs</a><br/><br/>

<a href="<?php echo "$proto://$sensor:$port"?>/localHostsInfo.html"
target="ntop">OS and Users</a><br/>

<a href="<?php echo "$proto://$sensor:$port"?>/domainTrafficStats.html"
target="ntop">Domains</a><br/>


</body>
</html>

