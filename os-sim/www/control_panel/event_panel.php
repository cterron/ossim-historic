<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAlarms");
?>
<?php
require_once 'classes/Xajax.inc';
require_once 'classes/Host.inc';
require_once 'classes/Plugin.inc';
require_once 'ossim_db.inc';
header('Cache-Control: no-cache');
?>
<?php
$xajax = new xajax();
//$xajax->DebugOn();

$db = new ossim_db();
$conn = $db->connect();

//CONFIG
$max_row = 4;

if (!isset($_SESSION['id']))
    $_SESSION['id'] = "0";

//if (!isset($_SESSION['row_num']))
 //   $_SESSION['row_num'] = 0;

if (!isset($row_num)) {
    global  $row_num;
    $row_num = 0;
}
if (!isset($_SESSION['plugins_to_show']))
    $_SESSION['plugins_to_show'] = array();

//$row_num = $_SESSION['row_num'];

//
//Check if we do need to cache the plugin names into a SESSION var
//
if(!isset($_SESSION['plugin_1505'])) {
    if ($plugin_list = Plugin::get_list($conn, "")) {
        foreach ($plugin_list as $plugin) {
            $id = "plugin_" . $plugin->get_id();
            $_SESSION[$id] = $plugin->get_name();
        }
    }
}

//
//Register xajax functions so they can be called within javascript
//
$xajax->registerFunction("iniExec");
$xajax->registerFunction("stopExec");
$xajax->registerFunction("auxExec");
$xajax->registerFunction("rotate");
$xajax->registerFunction("numEvents");



function rotate()
{
    global $conn;
    global $row_num;
    global $max_row;
    
    //Create xajax response
    $objResponse = new xajaxResponse();
    
    //We get the last event from event_tmp, the ossim server fills this table
    $sql = "SELECT *, inet_ntoa(src_ip) as aux_src_ip, inet_ntoa(dst_ip)
            as aux_dst_ip FROM event_tmp order by id desc limit 1";
    
    if ($aux = &$conn->Execute($sql)) {

        $aux_id = $aux->fields["id"];

        $innerHTML = "";

        // Only proceed if our fetched id is lower than the previous one
        if (intval($_SESSION['id']) < intval($aux_id)){

            $plugin_id_name = "plugin_" . $aux->fields["plugin_id"];
            
            //Here we get how many events have not been shown in the event 
            //viewer between two event wich have beenshown
            $aux_omi = $aux_id -  $_SESSION['id'] - 1;

            $_SESSION['id'] = $aux->fields["id"];
            $innerHTML = " <span class=\"col_event_name\">".$aux->fields["plugin_sid_name"]."</span>";
            $risk = $aux->fields["risk_a"];

          	if ($risk  > 7) {
            	$risk =  "<b><font color=\"red\">$risk</font></b>";
        	} elseif ($risk > 4) {
            	$risk =  "<b><font color=\"orange\">$risk</font></b>";
        	} elseif ($risk >= 1) {
            	$risk =  "<b><font color=\"green\">$risk</font></b>";
        	} else {
            	$risk =  "<b><font color=\"lightgreen\">$risk</font></b>";
        	}
            $innerHTML .=  "<span class=\"col_risk\">$risk</span>";
           
            $sensor = Host::ip2hostname($conn, $aux->fields["sensor"], true, true); 
            $src_ip = Host::ip2hostname($conn, $aux->fields["aux_src_ip"], false, true);
            $dst_ip = Host::ip2hostname($conn, $aux->fields["aux_dst_ip"], false, true);
	    $dst_ip  == "0.0.0.0" ? $dst_ip = "N/A" : $dst_ip;

            $innerHTML .= "<span
            class=\"col_plugin_name\">rownumber".$row_num." ale".$_SESSION[$plugin_id_name]."</span>
                           <span class=\"col_date\">".$aux->fields["timestamp"]."</span>
                           <span class=\"col_sensor\">".$sensor.":".$aux->fields["interface"]."</span>
                           <span class=\"col_source_ip\">".$src_ip.":".$aux->fields["src_port"]."</span>
                           <span class=\"col_dest_ip\">". $dst_ip.":".$aux->fields["dst_port"]."</span>
                           <span class=\"col_priority\">".$aux->fields["priority"]."</span>
                           <span class=\"col_ommitted\">".$aux_omi."</span>";

            $objResponse->addInsertAfter("row_aux", "div", "row_1");
            $objResponse->addAssign("row_1", "innerHTML", $innerHTML);
            $objResponse->addAssign("row_1", "className", "event_row");
            if ($row_num++ % 2) {
                $objResponse->addAssign("row_1", "style.background", "#EFEFEF");
            }
            $objResponse->addScript("Element.hide('row_1');");
            $objResponse->addScript("Effect.Appear('row_1', {from: 0.1, to: 0.8});");
            foreach (range($row_num, 1) as $n) {
                $objResponse->addAssign("row_$n", 'id', 'row_'.($n+1));
            }
            if (intval($row_num) > intval($max_row)){
                $last = "row_".($row_num + 1);
                $objResponse->addScript("Effect.DropOut('$last', {queue: 'end'});");
                $objResponse->addRemove($last);
        }
        }
    }
        return $objResponse->getXML();

}

function auxExec($exec_id)
{
    $_SESSION['exec_id'] = $exec_id;
    $objResponse = new xajaxResponse();
    return $objResponse->getXML();
}

function iniExec($timeout)
{
    $objResponse = new xajaxResponse();
    $objResponse->addScript("var exec_id = setInterval(xajax_rotate,".$timeout."); xajax_auxExec(exec_id);");
    return $objResponse->getXML();
}

function stopExec()
{
    $objResponse = new xajaxResponse();
    $objResponse->addScript("clearInterval(".$_SESSION["exec_id"].");");
    unset($_SESSION["exec_id"]);
    return $objResponse->getXML();
}
$xajax->setRequestURI($_SERVER["REQUEST_URI"]);
$xajax->processRequests();
?>



<html>
<head>
    <title>Event viewer</title>
    <script src="../js/prototype.js" type="text/javascript"></script>
    <script src="../js/scriptaculous/scriptaculous.js" type="text/javascript"></script>
    <?= $xajax->printJavascript('', '../js/xajax.js'); ?>
    <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<style type="text/css">
    .main {
	z-index:1;
        width: 80%;
    }
    .plugins {
	position: absolute;
	background-color: lightgrey;
	right: 1%;
	top: 80px;
        width: 20%;
    }
    .header_row{
        clear: both;
    }
    .event_row {
        clear: both;
        border-top: 1px solid #ccc;
        min-height: 3.3em; 
    }

    .col_event_name {
        float: left;
		color: blue;
        padding-top: 4px;
        width: 20%;
        display: block;
    }
    .col_risk {
        float: left;
        display: block;
        width: 5%;
    }
    .col_plugin_name {
        float: left;
        width: 10%;
        display: block;
    }
    .col_date {
        float: left;
        width: 10%;
        display: block;
    }
    .col_sensor {
        float: left;
        width: 11%;
        display: block;
    }
    .col_source_ip {
        float: left;
        width: 12%;
        display: block;
    }
    .col_dest_ip {
        float: left;
        width: 12%;
        display: block;
    }
     .col_priority {
        float: left;
        width: 10%;
        display: block;
    }
     .col_ommited {
        float: left;
        width: 10%;
        display: block;
    }
  </style>
</head>
<body>

<input type=button  Onclick="xajax_iniExec(500);" value="<?= _("start"); ?>">
<input type=button  Onclick="xajax_stopExec();" value="<?= _("stop"); ?>">
<SELECT NAME="speed " Onchange="xajax_stopExec(); xajax_iniExec(this.value);">  
 <OPTION VALUE="3000"> <?= _("Slow"); ?>
 <OPTION VALUE="2000"> <?= _("Medium"); ?>
 <OPTION VALUE="500"> <?= _("Fast"); ?>
</SELECT>

    <br>
<div name="main" class="main">
	<div id="row_header" class="header_row">
	<span class="col_event_name" style="color: grey;"><b> Event Name </b></span>
	<span class="col_risk"><b>Risk</b></span>
	<span class="col_plugin_name"><b>Plugin name</b></span>
	<span class="col_date"><b>Date</b></span>
	<span class="col_sensor"><b>Sensor</b></span>
	<span class="col_source_ip"><b>Source IP</b></span>
	<span class="col_dest_ip"><b>Dest IP</b></span>
	<span class="col_priority"><b>Priority</b></span>
	<span class="col_ommitted"><b>Ommitted</b></span>
	</div>
    <div id="row_aux" class="event_row";></div>
</div>
<div name"plugins" class="plugins">
<ul>
<?
	// Should use list from session here
	if (!$plugin_list) $plugin_list = Plugin::get_list($conn, "");
    if ($plugin_list) {
        foreach ($plugin_list as $plugin) {
			// Treat snort plugins, except spade, as part of snort
			if(($plugin->get_id() >= 1100 && $plugin->get_id() < 1104) || ($plugin->get_id() > 1104 && $plugin->get_id() < 1500)) continue;
			echo "<li><span name=\"span_" . $plugin->get_id() . "\">(" . $plugin->get_id() . ") - " . $plugin->get_name() . "</span>\n";
        }
    }
?>
</ul>
</div>
</body>
</html>
