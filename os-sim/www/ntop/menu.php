<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsNetwork");
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
require_once ("classes/Security.inc");

$sensor = GET('sensor');
$interface = GET('interface');
$proto = GET('proto');

ossim_valid($sensor, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Sensor"));
ossim_valid($interface, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("interface"));
ossim_valid($proto, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("proto"));

if (ossim_error()) {
    die(ossim_error());
}

require_once ('ossim_db.inc');
$db = new ossim_db();
$conn = $db->connect();

require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
$ntop_default = parse_url($conf->get_conf("ntop_link"));

require_once ('classes/Sensor.inc');

/* ntop link */
$scheme = $ntop_default["scheme"] ? $ntop_default["scheme"] : "http";
$port = $ntop_default["port"] ? $ntop_default["port"] : "3000";
$ntop = "$scheme://$sensor:$port";

?>

<!-- change sensor -->
<form method="GET" action="menu.php">
<?php echo gettext("Sensor"); ?>:&nbsp;
<select name="sensor" onChange="submit()">

<?php
    /*
     * default option (ntop_link at configuration)
     */
/*
    $option = "<option ";
    if ($sensor == $ntop_default["host"])
        $option .= " SELECTED ";
    $option .= ' value="'. $ntop_default["host"] . '">default</option>';
    print "$option\n";
*/

    /* Get highest priority sensor first */
    $tmp = Sensor::get_list($conn, "ORDER BY priority DESC LIMIT 1");
    if (is_array($tmp)) {
        $first_sensor = $tmp[0];
        $option  = "<option value='". $first_sensor->get_ip() ."'>";
        $option .= $first_sensor->get_name() . "</option>";
        print $option;
    }


    $sensor_list = Sensor::get_list($conn, "ORDER BY name");
    if (is_array($sensor_list)) {
        foreach ($sensor_list as $s) {

            /* don't show highest priority sensor again.. */
            if ($s->get_ip() != $first_sensor->get_ip()) {

                /*
                 * one more option for each sensor (at policy->sensors)
                 */
                $option = "<option ";
                if ($sensor == $s->get_ip())
                    $option .= " SELECTED ";
                $option .= ' value="'. $s->get_ip() . '">'. $s->get_name() .
                    '</option>';
                print "$option\n";
            }
        }
    }
?>
</select>
</form>
<!-- end change sensor -->



<!-- interface selector -->
<?php

require_once ('classes/Sensor_interfaces.inc');
require_once ('classes/SecurityReport.inc');


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
<?php echo gettext("Interface"); ?>:&nbsp;
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

$db->close($conn);

?>

</select>
</form>
<!-- end interface selector -->



<a href="<?php echo "$ntop/trafficStats.html" ?>" 
   target="ntop"><?php echo gettext("Global"); ?></a><br/>
<a href="<?php echo "$ntop/sortDataProtos.html"?>"
   target="ntop"><?php echo gettext("Protocols"); ?> </a><br/><br/>

<b> <?php echo gettext("Services"); ?> </b><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataIP.html?showL=0" ?>"
   target="ntop"><?php echo gettext("By host: Total"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataIP.html?showL=1" ?>"
   target="ntop"><?php echo gettext("By host: Sent"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataIP.html?showL=2" ?>"
   target="ntop"><?php echo gettext("By host: Recv"); ?></a><br/>
   
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/ipProtoDistrib.html" ?>"
   target="ntop"><?php echo gettext("Service statistic"); ?></a><br/>

&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/ipProtoUsage.html" ?>"
   target="ntop"><?php echo gettext("By client-server"); ?></a><br/><br/>

<b><?php echo gettext("Throughput"); ?></b><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataThpt.html?col=1&showL=0" ?>"
   target="ntop"><?php echo gettext("By host: Total"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataThpt.html?col=1&showL=1" ?>"
   target="ntop"><?php echo gettext("By host: Sent"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/sortDataThpt.html?col=1&showL=2" ?>"
   target="ntop"><?php echo gettext("By host: Recv"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/thptStats.html?col=1" ?>"
   target="ntop"><?php echo gettext("Total (Graph)"); ?></a><br/><br/>

<b> <?php echo gettext("Matrix"); ?> </b><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/ipTrafficMatrix.html" ?>"
   target="ntop"><?php echo gettext("Data Matrix"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/dataHostTraffic.html" ?>"
   target="ntop">
   <?php echo gettext("Time Matrix"); ?> </a><br/><br/>
   
<b> <?php echo gettext("Gateways, VLANs"); ?> </b><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/localRoutersList.html" ?>"
   target="ntop"><?php echo gettext("Gateways"); ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$ntop/vlanList.html" ?>"
   target="ntop"><?php echo gettext("VLANs"); ?></a><br/><br/>

<a href="<?php echo "$ntop/localHostsFingerprint.html" ?>"
   target="ntop"><?php echo gettext("OS and Users"); ?></a><br/>

<a href="<?php echo "$ntop/domainStats.html" ?>"
   target="ntop"><?php echo gettext("Domains"); ?></a><br/>


</body>
</html>

