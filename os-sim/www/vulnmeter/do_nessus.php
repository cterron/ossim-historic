<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>

<?php
// Testing some padding here for different browsers, see php flush() man page.
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Update Scan"); ?> </h1>

<?php
    require_once 'classes/Security.inc';

    $status = REQUEST('status');
    $interactive = REQUEST('interactive');
    $nsensors = REQUEST('nsensors');
    $sensors = REQUEST('sensors');
    $scheduler_id = REQUEST('scheduler_id');

    ossim_valid($nsensors, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("nsensors"));
    ossim_valid($status, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Status"));
    ossim_valid($scheduler_id, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Status"));

    if (ossim_error()) {
        die(ossim_error());
    }    

require_once ('ossim_acl.inc');
require_once ('ossim_db.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Net_group_scan.inc');
require_once ('classes/Net_group.inc');

$db = new ossim_db();
$conn = $db->connect();

define("NESSUS", 3001);

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

    function show_form(){
	global $sensor_list;
        global $conn;
	global $sensor_index;

	$global_i = 0;

	$num = count($sensor_list);
	if($num > 20){
	   $cols = 5;
	} else {
	   $cols = 3;
	}
	   $rows = intval($num / $cols) +1 ;

	?>
	<h3><center> <?= _("Select sensors for this scan"); ?> </center></h3>
<ul>
<?php
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
<p>
<?= _("Please adjust incident creation threshold, incidents will only be created for vulnerabilities whose risk level exceeds the threshold."); ?><br/>
<?= _("It is recommended to set a high level at the beginning in order to concentrate on more critical vulnerabilities first, lowering it after having solved/tagged them as false positivies."); ?><br/>
<?= _("Threshold configuration can be found at Configuration->Main, \"vulnerability_incident_threshold\"."); ?>&nbsp;
<?= _("Current ris risk threshold is:"); ?>
<b>
<?php
    require_once ('ossim_conf.inc');
    $conf = $GLOBALS["CONF"];
    print $conf->get_conf("vulnerability_incident_threshold");
?>
</b>
</p>
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
	    echo "</table>\n";

?>
<center>
<input type="hidden" name="nsensors" value="<?php echo $global_i ?>" />
<input type="Submit" value="<?= _("Submit"); ?>">
</center>
</form>
<center><a href="index.php"> <?php echo gettext("Back"); ?> </a></center>
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
<?php
    }

    if($interactive == "yes"){
    	show_form();
	exit();
    }

    $sensors = "";
    for ($i = 0; $i < $nsensors; $i++) {
    	if (ossim_error()) { die(ossim_error()); }
        if ($sensors == "")
            $sensors = POST("sensor$i");
        else
	    if (POST("sensor$i") != "")
                $sensors .= "," . POST("sensor$i");
    } 

    require_once ('ossim_conf.inc');
    $conf = $GLOBALS["CONF"];

    /* Frameworkd's address & port */
    $address = $conf->get_conf("frameworkd_address");
    $port = $conf->get_conf("frameworkd_port");

   /* create socket */
    $socket = socket_create (AF_INET, SOCK_STREAM, 0);
    if ($socket < 0) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("CRE_SOCKET", array(socket_strerror ($socket)));
    }

    /* connect */
    $result = @socket_connect ($socket, $address, $port);
    if (!$result) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("FRAMW_NOTRUN", array($address.":".$port));
    }

    if($status == "reset"){
        $in = 'nessus reset now' . "\n";
        socket_write ($socket, $in, strlen ($in));
	?>
	<center><a href="index.php"> <?php echo gettext("Back"); ?> </a></center>
	<?php
	exit();
    }
    if(strlen($sensors) == 0){
	foreach($sensor_list as $sensor){
        if ($sensors == "")
            $sensors = $sensor->get_ip();
        else
            $sensors .= "," . $sensor->get_ip();

	}
    }

    if($scheduler_id > 0){
    $in = 'nessus start ' . $scheduler_id . "\n";
    } else {
    $in = 'nessus start ' . $sensors . "\n";
    }
    $out = '';
    socket_write ($socket, $in, strlen ($in));
 
    echo str_pad('',1024);  // minimum start for Safari
?>
<center> 
<?php echo gettext("Nessus scan started, depending on number of hosts to be scanned this may take a while"); ?>.
</center>
<center>
<?= _("Scan status:") . " " ?>
<div id="percentage">
<?= "0% " . _("completed.") ?>
</div>
</center>
<?php flush(); ?>
<?php
    $in = 'nessus status get' . "\n";

    while (socket_write($socket, $in, strlen ($in)) && ($out = socket_read ($socket, 255, PHP_BINARY_READ)))
    {
        if($out >0 && $out <100){
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML = '"' . <?php echo rtrim($out); ?> + "<?= "% " . _("completed.") ?>";
</script>
<?php
flush();
        } elseif ( $out < 0 ) {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =  "<?= '"' . _("Error! return was:") . " " ?>" + <?php echo rtrim($out); ?> + "<?= " " . _("Please check your frameworkd logs.") . '"'?>" ;
</script>
<?php
flush();
break;
        } elseif ( $out == 100 ) {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =  "<?= '"' . _("Scan succesfully completed.") . '"' ?>" ; 
</script>
<?php
flush();
break;
        } else {
            if(preg_match("/Error/",$out)){
        ?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =   <?= '"<BR>' . _("An error ocurred, please check your frameworkd & web server logs:") . '<BR><BR><b>' . rtrim($out) . '</b><BR>"' ?>; 
percentage_div.innerHTML += "<BR><a href=\"<?= $_SERVER["PHP_SELF"]?>?status=reset\" > <?= _("Reset") . "<BR>&nbsp;<BR>";?>"; 
</script>
<?php
flush();
break;
            } else {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =   <?= '"<BR>' . _("Frameworkd said:") . '<BR><BR><b>' . rtrim($out) . '</b><BR>&nbsp;<BR>"' ?>; 
</script>

<?php
flush();
break;
            }
        }
        sleep(5);
    }
    socket_close($socket);

?>
<center><a href="index.php"> <?php echo gettext("Back"); ?> </a></center>
 
</body>
</html>

