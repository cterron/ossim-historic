<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>


<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
  <script src="../js/prototype.js" type="text/javascript"></script>
</head>

<body onLoad="Element.hide('sched_form');">

<h1> <?= _("Scheduling information"); ?> </h1>
<?php

require_once ('classes/Security.inc');
require_once ('classes/Plugin_scheduler.inc');
require_once ('classes/Util.inc');
require_once ('classes/Host.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Net_group_scan.inc');
require_once ('classes/Net_group.inc');


$db = new ossim_db();
$conn = $db->connect();

$user = Session::get_session_user();
$conf = $GLOBALS['CONF'];
$conf_threshold = $conf->get_conf('threshold');

$frameworkd_dir = $conf->get_conf('frameworkd_dir');
$donessus_path = $frameworkd_dir . "/DoNessus.py";

if(!is_executable($donessus_path)){
echo "<center><b>";
echo _("DoNessus.py needs to be executable for the scheduler to work.");
echo "<br/>";
echo _("Please ignore this warning in case frameworkd is running on another host.");
echo "</b></center>";
}

$action = REQUEST("action");
$plugin = REQUEST("plugin");
$id = REQUEST("id");

ossim_valid($action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Action"));
ossim_valid($plugin, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Plugin"));
ossim_valid($id, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("ID"));

if (ossim_error()) {
    die(ossim_error());
}
?>
<p>
<?= _("Please adjust incident creation threshold, incidents will only be created for vulnerabilities whose risk level exceeds the threshold."); ?><br/>
<?= _("It is recommended to set a high level at the beginning in order to concentrate on more critical vulnerabilities first, lowering it after having solved/tagged them as false positivies."); ?><br/>
<?= _("Threshold configuration can be found at Configuration->Main, \"vulnerability_incident_threshold\"."); ?>&nbsp;
<?= _("Current risk threshold is:"); ?>
<b>
<?php
    print $conf->get_conf("vulnerability_incident_threshold");
?>
</b>
</p>
<?php

if($action == "insert"){

//$plugin = REQUEST("plugin");
$minute = REQUEST("minute");
$hour = REQUEST("hour");
$day_month = REQUEST("day_month");
$month = REQUEST("month");
$day_week = REQUEST("day_week");
$nsensors = REQUEST('nsensors');

ossim_valid($action, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Action"));
ossim_valid($id, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("ID"));
ossim_valid($minute, OSS_CRONTAB, 'illegal:'._("Minute"));
ossim_valid($hour, OSS_CRONTAB, 'illegal:'._("Hour"));
ossim_valid($day_month, OSS_CRONTAB, 'illegal:'._("Day of month"));
ossim_valid($month, OSS_CRONTAB, 'illegal:'._("Month"));
ossim_valid($day_week, OSS_CRONTAB, 'illegal:'._("Day of week"));

if (ossim_error()) {
    die(ossim_error());
}



   if (ossim_error()) { die(ossim_error()); }

    $sensors = array();
    for ($i = 0; $i < $nsensors; $i++) {
	if(REQUEST("sensor$i") != null){
	array_push($sensors, REQUEST("sensor$i"));
	}
    }

    if (!count($sensors)) {
        die(ossim_error(_("At least one Sensor required")));
    }

Plugin_scheduler::insert($conn, $plugin, $minute, $hour, $day_month, $month, $day_week, $sensors);

?>
<center><b><?= _("Successfully inserted"); ?></b></center>
<center><a href="<?= $_SERVER["PHP_SELF"]?>"><?= _("Back"); ?></a></center>
        
<?

} else {

if($action == "delete"){
Plugin_scheduler::delete($conn, $id);
}
?>

<?php
// Get schedule list
$schedules = Plugin_scheduler::get_list($conn, "");

?>
<table width="100%" align="left">
<tr>
<th><?= _("Plugin ID"); ?></th>
<th><?= _("Minute"); ?></th>
<th><?= _("Hour"); ?></th>
<th><?= _("Day of Month"); ?></th>
<th><?= _("Month"); ?></th>
<th><?= _("Day of week"); ?></th>
<th><?= _("Sensors"); ?></th>
<th><?= _("Action"); ?></th>
</tr>
<?php

foreach($schedules as $schedule){
$id = $schedule->get_plugin();
$sensors = Plugin_scheduler::get_sensors($conn, $schedule->get_id());
if ($plugin_list = Plugin::get_list($conn, "WHERE id = $id")) {
	$plugin_name = $plugin_list[0]->get_name();
} else {
	$plugin_name = $id;
}
echo "<tr>\n";
echo "<td>" . $plugin_name . "</td>\n";
echo "<td>" . $schedule->get_minute() . "</td>\n";
echo "<td>" . $schedule->get_hour() . "</td>\n";
echo "<td>" . $schedule->get_day_month() . "</td>\n";
echo "<td>" . $schedule->get_month() . "</td>\n";
echo "<td>" . $schedule->get_day_week() . "</td>\n";
echo "<td>";
foreach($sensors as $sensor){
echo Host::ip2hostname($conn, $sensor->get_sensor_name()) . "<br>";
}
echo "</td>\n";
echo "<td>[ <a href=\"" .  $_SERVER["PHP_SELF"] . "?action=delete&id=" . $schedule->get_id() . "\">" .  _("Delete") . "</a> | <a href=\"do_nessus.php?interactive=no&scheduler_id=" . $schedule->get_id() . "\">" .  _("Scan now") . "</a> ]</td>";
echo "</tr>\n";
}

?>
</table>
<?php

// Get sensor list
        $global_i = 0;
        define("NESSUS", 3001);

        $sensors = Sensor::get_all($conn, "ORDER BY name ASC");
        $sensor_list = array();

        foreach($sensors as $sensor){
                if(Sensor::check_plugin_rel($conn, $sensor->get_ip(), NESSUS)){
                array_push($sensor_list, $sensor);
                }
        }

        $num = count($sensor_list);
        if($num > 20){
           $cols = 5;
        } else {
           $cols = 3;
        }
           $rows = intval($num / $cols) +1 ;

?>
&nbsp;<br/>
<?= _("Warning: scheduling two different scans for the same month, day, hour and minute will yield unexpected results."); ?> <?= _("Of course you can select multiiple sensors for a certain schedule."); ?><br/>
&nbsp;<br/>
<hr noshade>
<center>
<a href="#" onclick="Element.show('sched_form'); return false;"> <?= _("Add another schedule"); ?> </a>
</center>
<div id="sched_form">

        <h3><center> <?= _("Select sensors for this scan"); ?> </center></h3>
<ul>
<?php
$tmp_sensors = Sensor::get_all($conn, "ORDER BY name ASC");
$sensor_list = array();
// Quick & dirty sensor index array for "sensor#" further below
$sensor_index = array();
$tmp_index = 0;

foreach($tmp_sensors as $sensor){
        if(Sensor::check_plugin_rel($conn, $sensor->get_ip(), NESSUS)){
        $sensor_index[$sensor->get_name()] = $tmp_index;
        $tmp_index++;
        array_push($sensor_list, $sensor);
        }
}
$group_scan_list = Net_group_scan::get_list($conn, "WHERE plugin_id = " . NESSUS);
foreach($group_scan_list as $group_scan){
$net_group_sensors = Net_group::get_sensors($conn, $group_scan->get_net_group_name());
echo "\n<script>\n";
echo "var " . $group_scan->get_net_group_name() . " = true;\n";
echo "</script>\n";
$sensor_string = "";
foreach($net_group_sensors as $ng_sensor => $name){
if($sensor_string == ""){
$sensor_string .= $sensor_index[$name];
} else {
$sensor_string .= "," . $sensor_index[$name];
} 
}
print "<li><a href=\"#\" onClick=\"return selectSome('". $group_scan->get_net_group_name() . "','" . $sensor_string . "');\">" . $group_scan->get_net_group_name() . "</a>";
}
?>  
</ul>
        <form action="<?= $_SERVER["PHP_SELF"]?>" method="POST">
<center>
<input type="Submit" value="<?= _("Submit"); ?>">
</center>
        <h4><center> (<?= _("Empty means all"); ?>) </center></h4>
        <center><a href="#" onClick="return selectAll('sensors');"><?= _("Select / Unselect all");?></a></center>
<br/>
        <table width="100%" align="left" border="0"><tr>
        <?php
        for($i=1;$i<=$rows;$i++){
        ?>
        <?php
            for($a=0;$a <$cols && $global_i < $num ;$a++){
                $sensor = $sensor_list[$global_i];
                echo "<td width=\"" . intval(100/$cols) . "%\">";
                $all['sensors'][] = "sensor".$global_i;
                ?>
                <div align="left">
                <input align="left" type="checkbox" id="<?= "sensor".$global_i ?>" name="<?= "sensor".$global_i ?>"
                               value="<?= $sensor->get_ip() ?>" /><?=$sensor->get_name()?></div></td>
                <?php
                $global_i++;
            }
            echo "</tr>\n";
            ?>
            <?php
        }

?>
<tr>
<td colspan="<?= $cols ?>">
<center>
<input type="hidden" name="nsensors" value="<?php echo $global_i ?>" />
<input type="hidden" name="plugin" value="<?php echo NESSUS ?>" />

<hr noshade>
<table width="70%" style="center" border="2">
<tr>
<td>
<center>
<p align="left">
<?= _("Sample would scan the 13th of each month, at 03:00"); ?>
</p>
<table width="400">
<tr><th><?= _("field</th><th>allowed values"); ?></th></tr>
<tr><td colspan="2"> <?= _("Specify your scheduling information using crontab-like syntax"); ?>:</td>
<tr><td colspan="2"><hr noshade></td></tr>
<tr><td><?= _("minute"); ?></td><td>0-59</td></tr>
<tr><td><?= _("hour"); ?></td><td>0-23</td></tr>
<tr><td><?= _("day of month"); ?></td><td>1-31</td></tr>
<tr><td><?= _("month"); ?></td><td>1-12</td></tr>
<tr><td><?= _("day of week"); ?></td><td>0-7</td></tr>
<tr><td colspan="2"><?= _("Use * as wildcard"); ?></td></tr>
</table>
</center>
</td>
<td>
<p align="left"
<form action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="POST">
<input type="hidden" name="action" value="insert">
<?=_('Minute')?><br/><li><input type="text" size=10 name="minute" value="0"><br/>
<?=_('Hour')?> <br/><li><input type="text" size=10 name="hour" value="3"><br/>
<?=_('Day of Month')?> <br/><li><input type="text" size=10 name="day_month" value="13"><br/>
<?=_('Month')?> <br/><li><input type="text" size=10 name="month" value="*"><br/>
<?=_('Day of Week')?> <br/><li><input type="text" size=10 name="day_week" value="*"><br/>
</ul>
<input type="submit" value="<?= _("Submit"); ?>">
</p>
</form>
</td>
</tr>
</table>
</tr></td></table>
</div>

<?php
}
?>
<script>
var check_sensors = true;

function selectAll(category)
{
    if (category == 'sensors') {
    <? foreach ($all['sensors'] as $id) { ?>
        document.getElementById('<?=$id?>').checked = check_sensors;
    <? } ?>
        check_sensors = check_sensors == false ? true : false;
    }
    return false;
}

function selectSome(name, identifiers)
{

arrayOfStrings = identifiers.split(",");
for (var i=0; i < arrayOfStrings.length; i++) {
document.getElementById("sensor" + arrayOfStrings[i]).checked = window[name];
}
window[name] = window[name] == false ? true : false;
return false;
} 
            
</script>

</body>
</html>

