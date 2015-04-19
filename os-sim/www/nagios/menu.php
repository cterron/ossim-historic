<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsAvailability");
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

ossim_valid($sensor, OSS_IP_ADDR, 'illegal:'._("Sensor"));

if (ossim_error()) {
    die(ossim_error());
}
                                    
require_once ('ossim_db.inc');
$db = new ossim_db();
$conn = $db->connect();

require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
$nagios_default = parse_url($conf->get_conf("nagios_link"));

require_once ('classes/Sensor.inc');
$sensor_list = Sensor::get_list($conn, "ORDER BY name");

/* nagios link */
$scheme = $nagios_default["scheme"] ? $nagios_default["scheme"] : "http";
$path = $nagios_default["path"] ? $nagios_default["path"] : "/nagios/";
$nagios = "$scheme://$sensor/$path";

$db->close($conn);
?>


<!-- change sensor -->
<form method="GET" action="menu.php">
Sensor:&nbsp;
<select name="sensor" onChange="submit()">

<?php
    /*
     * default option (nagios_link at configuration)
     */
    $option = "<option ";
    if ($sensor == $nagios_default["host"])
        $option .= " SELECTED ";
    $option .= ' value="'. $nagios_default["host"] . '">default</option>';
    print "$option\n";

    if (is_array($sensor_list)) {
        foreach ($sensor_list as $s) {

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
?>
</select>
</form>
<!-- end change sensor -->



<b>Monitoring</b><br/>
<!--
&nbsp;&nbsp;&nbsp;
<a href="<?php // echo "$nagios/cgi-bin/tac.cgi" ?>"
   target="nagios">Tactical Overview</a><br/>
-->
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?host=all" ?>"
   target="nagios"><?php echo gettext("Service Detail") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?hostgroup=all&style=hostdetail" ?>"
   target="nagios"><?php echo gettext("Host Detail") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?hostgroup=all" ?>"
   target="nagios"><?php echo gettext("Status Overview") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?hostgroup=all&style=grid" ?>"
   target="nagios"><?php echo gettext("Status Grid") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/statusmap.cgi?host=all" ?>"
   target="nagios"><?php echo gettext("Status Map") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?host=all&servicestatustypes=248" ?>"
   target="nagios"><?php echo gettext("Service Problems") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/status.cgi?hostgroup=all&style=hostdetail&hoststatustypes=12" ?>"
   target="nagios"><?php echo gettext("Host Problems") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/outages.cgi" ?>"
   target="nagios"><?php echo gettext("Network Outages") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/extinfo.cgi?&type=3" ?>"
   target="nagios"><?php echo gettext("Comments") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/extinfo.cgi?&type=6" ?>"
   target="nagios"><?php echo gettext("Downtime") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/extinfo.cgi?&type=0" ?>"
   target="nagios"><?php echo gettext("Process Info") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/extinfo.cgi?&type=4" ?>"
   target="nagios"><?php echo gettext("Performance Info") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/extinfo.cgi?&type=7" ?>"
   target="nagios"><?php echo gettext("Scheduling Queue") ?></a><br/><br/>

<b>Reporting</b><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/trends.cgi" ?>"
   target="nagios"><?php echo gettext("Trends") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/avail.cgi" ?>"
   target="nagios"><?php echo gettext("Availability") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/histogram.cgi" ?>"
   target="nagios"><?php echo gettext("Event Histogram") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/history.cgi?host=all" ?>"
   target="nagios"><?php echo gettext("Event History") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/summary.cgi" ?>"
   target="nagios"><?php echo gettext("Event Summary") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/notifications.cgi?contact=all" ?>"
   target="nagios"><?php echo gettext("Notifications") ?></a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo "$nagios/cgi-bin/showlog.cgi" ?>"
   target="nagios"><?php echo gettext("Performance Info") ?></a><br/>

</body>
</html>

