<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
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

#
# get ntop port from default ntop entry at
# /etc/ossim/framework/ossim.conf
# a better solution ??
#
$url_parsed = parse_url($conf->get_conf("ntop_link"));
$port = $url_parsed["port"];
$proto = $url_parsed["scheme"];

require_once ('ossim_db.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Sensor_interfaces.inc');
require_once ('classes/SecurityReport.inc');

$db = new ossim_db();
$conn = $db->connect();
$interface = $_GET["interface"];

if ($interface){
$fd = @fopen("http://$sensor:$port/switch.html", "r");
if($fd != NULL){
while(!feof($fd)){
$buffer = fgets($fd, 4096);
if(ereg ("VALUE=([0-9]+)[^0-9]*$interface.*", $buffer, $regs)){
$fd2 = @fopen("http://$sensor:$port/switch.html?interface=$regs[1]", "r");
if($fd2 != NULL) fclose($fd2);
}
}
fclose($fd);
}
}
?>


<form method="GET" action="menu.php">
<input type="hidden" name="proto" value="<?php echo $proto ?>"/>
<input type="hidden" name="port" value="<?php echo $port ?>"/>
Sensor:&nbsp;
<select name="sensor" onChange="submit()">
<?php
if ($sensor_list = Sensor::get_list($conn, "ORDER BY name")) {
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

<form method="GET" action="menu.php">
Interface:&nbsp;
<input type="hidden" name="proto" value="<?php echo $proto ?>"/>
<input type="hidden" name="port" value="<?php echo $port ?>"/>
<input type="hidden" name="sensor" value="<?php echo $sensor?>"/>
<select name="interface" onChange="submit()">

<?php
if ($sensor_list = Sensor::get_list($conn)) {
    foreach ($sensor_list as $s) {
        if ($sensor == $s->get_ip()){ 
        if($sensor_interface_list = Sensor_interfaces::get_list($conn, $s->get_name())){
            foreach($sensor_interface_list as $s_int){
?>
<option 
<?php 
if(!($interface) && ($s_int-> get_main() == 1)){
echo "SELECTED";
} elseif ($interface == $s_int->get_interface()){ 
echo "SELECTED";
} 
?> value="<?php
echo $s_int->get_interface();?>"><?php echo
SecurityReport::Truncate($s_int->get_name(),30,"..."); ?></option>
<?php
            }
            }
        }
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
       target="ntop">
       <?php echo gettext("Global"); ?> </a></br>
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataProtos.html"
       target="ntop">
       <?php echo gettext("Protocols"); ?> </a><br/><br/>

<b> <?php echo gettext("Services"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataIP.html?showL=0"
   target="ntop">
   <?php echo gettext("By host: Total"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataIP.html?showL=1"
   target="ntop">
   <?php echo gettext("By host: Sent"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataIP.html?showL=2"
   target="ntop">
   <?php echo gettext("By host: Recv"); ?> </a><br/>
   
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipProtoDistrib.html"
  target="ntop">
  <?php echo gettext("Service statistic"); ?> </a><br/>

&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipProtoUsage.html"
  target="ntop">
  <?php echo gettext("By client-server"); ?> </a><br/><br/>

<b> <?php echo gettext("Throughput"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataThpt.html?col=1&showL=0"
   target="ntop">
   <?php echo gettext("By host: Total"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataThpt.html?col=1&showL=1"
   target="ntop">
   <?php echo gettext("By host: Sent"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/sortDataThpt.html?col=1&showL=2"
   target="ntop">
   <?php echo gettext("By host: Recv"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/thptStats.html?col=1"
   target="ntop">
   <?php echo gettext("Total (Graph)"); ?> </a><br/><br/>

<b> <?php echo gettext("Matrix"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/ipTrafficMatrix.html"
   target="ntop">
   <?php echo gettext("Data Matrix"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/dataHostTraffic.html"
   target="ntop">
   <?php echo gettext("Time Matrix"); ?> </a><br/><br/>
   
<b> <?php echo gettext("Gateways, VLANs"); ?> </b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/localRoutersList.html"
   target="ntop">
   <?php echo gettext("Gateways"); ?> </a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$proto://$sensor:$port"?>/vlanList.html"
   target="ntop">
   <?php echo gettext("VLANs"); ?> </a><br/><br/>

<a href="<?php echo "$proto://$sensor:$port"?>/localHostsFingerprint.html"
target="ntop">
<?php echo gettext("OS and Users"); ?> </a><br/>

<a href="<?php echo "$proto://$sensor:$port"?>/domainStats.html"
target="ntop">
<?php echo gettext("Domains"); ?> </a><br/>


</body>
</html>

