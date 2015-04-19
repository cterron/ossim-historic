<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* Classes list:
*/
require_once ("classes/Util.inc");
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelSEM");
require_once ('../graphs/charts.php');
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT"); // Date in the past
// Open Source
require_once "ossim_conf.inc";
$conf = $GLOBALS["CONF"];
$version = $conf->get_conf("ossim_server_version", FALSE);
if (!preg_match("/.*pro.*/i",$version) && !preg_match("/.*demo.*/i",$version)) {
	echo "<html><body><a href='http://www.alienvault.com/information.php?interest=ProfessionalSIEM' target='_blank' title='Proffesional SIEM'><img src='../pixmaps/sem_pro.png' border=0></a></body></tml>";
	exit;
}
//
$param_query = $_GET["query"] ? $_GET["query"] : "";
$param_start = $_GET["start"] ? $_GET["start"] : "";
$param_end = $_GET["end"] ? $_GET["end"] : "";
$config = parse_ini_file("everything.ini");
$uniqueid = uniqid(rand() , true);
?>
<?php
$help_entries["help_tooltip"] = _("Click on this icon to active <i>contextual help</i> mode. Then move your mouse over items and you\'ll see how to use them.<br/>Click here again to disable that mode.");
$help_entries["search_box"] = _("This is the main searchbox. You can type in stuff and it will be searched inside the \'data\' field. Special keywords can be used to restrict search on specific fields:<br/><ul><li>sensor</li><li>src_ip</li><li>dst_ip</li><li>plugin_id</li><li>src_port</li><li>dst_port</li></ul><br/>Examples:<ul><li>plugin_id=4004 and root</li><li>plugin_id!=4004 and not root</li></ul>");
$help_entries["saved_searches"] = _("You can save queries using the Save button near the search box. Here you can recover them and/or delete them.");
$help_entries["close_all"] = _("This will close the graphs below as well as the cache status. Used for a quick <i>tidy up</i>.");
$help_entries["cache_status"] = _("Depending on the amount of time you query on and your log volume, cache can be grow rapidly. Use this to check the status and clean/delete as needed.");
$help_entries["graphs"] = _("Graphs will be recalculated based on the searchbox data, but take some time. Collapse this part for faster searching. You can add query criteria by clicking on various graph regions. Charst aren\'t drawn if the query results in more than 500000 events.");
$help_entries["result_box"] = _("This is the main result box. Each line is a log entry, and can be reordered based on date. You can click anywhere on the log lines to add the highlighted text to the search criteria.");
$help_entries["clear"] = _("Use this to clear the search criteria.");
$help_entries["play"] = _("Submit your query for processing.");
$help_entries["date_ack"] = _("Acknowledge your date setting in order to recalculate the query.");
$help_entries["save_button"] = _("Use this button to save your current search for later re-use. Saved searches can be viewed by clicking on the saved searches drop-down in the upper left corner.");
$help_entries["date_frame"] = _("Choose between various pre-defined dates to query on. They will be recalculated each time the page is loaded.");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="">
<head>
<link rel="stylesheet" href="../style/style.css"/>
<script type="text/javascript" src="jquery-1.3.2.min.js"></script>

<style type="text/css">

a {text-decoration:none}

#searchbox{
	font-size: 1.5em;
	margin: 0.5em;
}

#dhtmltooltip{
position: absolute;
width: 150px;
border: 2px solid black;
padding: 2px;
background-color: lightyellow;
visibility: hidden;
z-index: 100;
}

img{
	vertical-align:middle;
}
small {
	font:12px arial;
}

#maintable{
border:0;
background-color: white;
}
#searchtable{
background-color: white;
}
.negrita { font-weight:bold; font-size:14px; }

</style>

<script>

var first_load = 1;
var byDateStart="";
var byDateEnd="";

function bold_dates(which_one){
	$('#date1,#date2,#date3,#date4,#date5').removeClass('negrita');
	if (which_one) $('#'+which_one).addClass('negrita');
}

function display_info ( var1, var2, var3, var4, var5, var6 ){
// Handle clicks on graphs
	var combined = var6 + "=" + var4;
	SetSearch(combined);
	hideLayer("by_date");
}

function MakeRequest()
{
	// Used for main query
	document.getElementById('loading').style.display = "block";
	var str = escape(document.getElementById('searchbox').value);
	var offset = parseInt(document.getElementById('offset').value);
	var start = escape(document.getElementById('start').value);
	var end = escape(document.getElementById('end').value);
	var sort = escape(document.getElementById('sort').value);
	$.ajax({
		type: "GET",
		url: "process.php?query=" + str + "&offset=" + offset + "&start=" + start + "&end=" + end + "&sort=" + sort + "&uniqueid=<?php echo $uniqueid
?>",
		data: "",
		success: function(msg) {
			document.getElementById('loading').style.display = "none";
			HandleResponse(msg);
		}
	});
}

function RequestLines()
{
	// Used for main query
	document.getElementById('loading').style.display = "block";
	var start = escape(document.getElementById('start').value);
	var end = escape(document.getElementById('end').value);
	$.ajax({
		type: "GET",
		url: "wcl.php?start=" + start + "&end=" + end + "&uniqueid=<?php echo $uniqueid
?>",
		data: "",
		success: function(msg) {
			document.getElementById('loading').style.display = "none";
			document.getElementById('numlines').innerHTML = msg;
		}
	});
}

function KillProcess() 
{
	$.ajax({
		type: "GET",
		url: "killprocess.php?uniqueid=<?php echo $uniqueid
?>",
		data: "",
		success: function(msg) {
			alert("Processes stoped!");
		}
	});
}

function HandleQuery(response){
// Print query listing
	document.getElementById('saved_searches').innerHTML = response;
}

function MakeRequest2(query, action)
{
// Used for query saving
	$.ajax({
		type: "GET",
		url: "manage_querys.php?query=" + query + "&action=" + action,
		data: "",
		success: function(msg) {
			HandleQuery(msg);
		}
	});
}

function DeleteQuery(query){
// delete saved query from list
	MakeRequest2(query,"delete");
}

function AddQuery(){
// Add saved query to list
	var query = escape(document.getElementById('searchbox').value);
	MakeRequest2(query,"add");
}


function toggleLayer( whichLayer )
{
  var elem, vis;
  if( document.getElementById ) // this is the way the standards work
    elem = document.getElementById( whichLayer );
  else if( document.all ) // this is the way old msie versions work
      elem = document.all[whichLayer];
  else if( document.layers ) // this is the way nn4 works
    elem = document.layers[whichLayer];
  vis = elem.style;
  // if the style.display value is blank we try to figure it out here
  if(vis.display==''&&elem.offsetWidth!=undefined&&elem.offsetHeight!=undefined)
    vis.display = (elem.offsetWidth!=0&&elem.offsetHeight!=0)?'block':'none';
  vis.display = (vis.display==''||vis.display=='block')?'none':'block';
}

function hideLayer( whichLayer )
{
  var elem, vis;
  if( document.getElementById ) // this is the way the standards work
    elem = document.getElementById( whichLayer );
  else if( document.all ) // this is the way old msie versions work
      elem = document.all[whichLayer];
  else if( document.layers ) // this is the way nn4 works
    elem = document.layers[whichLayer];
  vis = elem.style;
  // if the style.display value is blank we try to figure it out here
  vis.display = 'none';
}


function closeLayer( whichLayer )
{
  var elem, vis;
  if( document.getElementById ) // this is the way the standards work
    elem = document.getElementById( whichLayer );
  else if( document.all ) // this is the way old msie versions work
      elem = document.all[whichLayer];
  else if( document.layers ) // this is the way nn4 works
    elem = document.layers[whichLayer];
  vis = elem.style;
  // if the style.display value is blank we try to figure it out here
  vis.display = 'none';
}


function SetSearch(content)
{
// Add to search bar, perform search
  var saved = document.getElementById('searchbox').value;
  document.getElementById('searchbox').value = saved.replace(/\s.*\=.*/,"") + " " + content;
  MakeRequest();
}

function ReplaceSearch(content)
{
// Replace search bar, perform search
  document.getElementById('searchbox').value = content;
  MakeRequest();
}

function ClearSearch()
{
// Clear search bar, perform search
  document.getElementById('searchbox').value = "";
  document.getElementById('offset').value = "0";
  document.getElementById('sort').value = "none";
  MakeRequest();
}

function IncreaseOffset(amount)
{
// Pagination
  var offset = parseInt(document.getElementById('offset').value);
  document.getElementById('offset').value = offset + amount;
  MakeRequest();
}

function DateAsc()
{
// Sorting
  document.getElementById('sort').value = "date";
  MakeRequest();
}

function DateDesc()
{
// Sorting
  document.getElementById('sort').value = "date_desc";
  MakeRequest();
}


function DecreaseOffset(amount)
{
// Pagination
	var offset = parseInt(document.getElementById('offset').value);
	document.getElementById('offset').value = offset - amount;
	MakeRequest();
}

function setFixed(start, end, gtype, datef)
{
// Gets fixed time ranges from day, month, etc... buttons
	document.getElementById('start').value = start;
	document.getElementById('start_aaa').value = start;
	document.getElementById('end').value = end;
	document.getElementById('end_aaa').value = end;
	if (gtype != '' && datef != '') {
		UpdateByDate("forensic.php?graph_type="+gtype+"&cat="+datef);
	}
	RequestLines();
	MakeRequest();
}

function setFixed2()
{
// Gets fixed time ranges from calendar popups
// If not entered manually hour information will be missing so..
	var start_pad = "";
	var end_pad = "";
	if(document.getElementById('start_aaa').value.length == 10){
		var start_pad = " 00:00:00";
	}
	if(document.getElementById('end_aaa').value.length == 10){
		var end_pad = " 00:00:00";
	}

	document.getElementById('start').value = document.getElementById('start_aaa').value + start_pad;
	document.getElementById('end').value = document.getElementById('end_aaa').value + end_pad;
	RequestLines();
	MakeRequest();
}


function HandleResponse(response)
{
// Main response handler for event lines
	document.getElementById('ResponseDiv').innerHTML = response;
	if(first_load == 1){
		first_load = 0;
	} else {
		var cont = document.getElementById('test').innerHTML;
		document.getElementById('test').innerHTML = "";
		document.getElementById('test').innerHTML = cont;
	}
}

function HandleCacheResponse(response)
{
// Handle Gauge and cache information
  var responses = response.split(":");
  if(responses[0] == "pct"){
    gauge.needle.setValue(responses[1]);
  } else {
  document.getElementById('gauge_text').innerHTML = response;
  }
}

function showTip(text, color, width){
	if(document.body.style.cursor == 'help'){
		ddrivetip(text,color,width);
	}
}

function hideTip(){
	if(document.body.style.cursor == 'help'){
		hideddrivetip();
	}
}

function toggleCursor(){
	if(document.body.style.cursor == 'help'){
		document.body.style.cursor = document.getElementById('cursor').value;} else {
			document.body.style.cursor = "help";
		}
}

function HandleStatsByDate(response)
{
	//document.getElementById('by_date').innerHTML=response.replace(/so.write\([^\)]+\)/,'so.write("by_date")');
	var cont=document.getElementById('by_date').innerHTML;
	document.getElementById('by_date').innerHTML="";
	document.getElementById('by_date').innerHTML=cont;
  	if(first_load != 1)
	{
		hideLayer("test");
	}
}

function UpdateByDate(urlres)
{
	$.ajax({
		type: "GET",
		url: urlres,
		data: "",
		success: function(msg) {
			HandleStatsByDate(msg);
		}
	});
}


function graph_by_date( col ,row ,value, category, series, t_year, t_month)
{
    var urlres = "forensic.php";
    var month;
    var year;
    var day;
    var hour;

    document.getElementById('searchbox').value = "";
    document.getElementById('offset').value = "0";
    document.getElementById('sort').value = "none";
  switch(row)
  {
    case 1:
      urlres = urlres+ "?graph_type=year&cat=" + category;

      year=category.replace(/^ *| *$/g,"");
      byDateStart=year+"-01-01";
      byDateEnd=year+"-12-31";
      document.getElementById('start').value = byDateStart+" 00:00:00";
      document.getElementById('start_aaa').value = byDateStart+" 00:00:00";
      document.getElementById('end').value = byDateEnd+ " 23:59:59";
      document.getElementById('end_aaa').value = byDateEnd+" 23:59:59";
      RequestLines(); MakeRequest();
      bold_dates();
    break;
    case 2:
      urlres = urlres + "?graph_type=month&cat=" + category;

      month=monthToNumber(category.replace(/,.*$/,""));
      year=category.replace(/^.*, /,"");
      byDateStart=year+"-"+month+"-01";
      lastmonthday = new Date((new Date(year, month, 1))-1).getDate();
      byDateEnd=year+"-"+month+"-"+lastmonthday;
      document.getElementById('start').value = byDateStart+" 00:00:00";
      document.getElementById('start_aaa').value = byDateStart+" 00:00:00";
      document.getElementById('end').value = byDateEnd+ " 23:59:59";
      document.getElementById('end_aaa').value = byDateEnd+" 23:59:59";
      RequestLines(); MakeRequest();
      bold_dates();
    break;
    case 3:
      urlres = urlres + "?graph_type=day&cat=" + category;

      month=monthToNumber(category.replace(/ .*$/,""));
      year=category.replace(/^.*, /,"");
      day=category.replace(/^[^ ]+ /,"");
      day=day.replace(/,.*$/,"");
      if(day.length==1)
      	day="0"+day;
      byDateStart=year+"-"+month+"-"+day;
      byDateEnd=year+"-"+month+"-"+day;
      document.getElementById('start').value = byDateStart+" 00:00:00";
      document.getElementById('start_aaa').value = byDateStart+" 00:00:00";
      document.getElementById('end').value = byDateEnd+ " 23:59:59";
      document.getElementById('end_aaa').value = byDateEnd+" 23:59:59";
      RequestLines(); MakeRequest();
      bold_dates();
      //alert("day: "+ day +" month: "+month+ " year: "+year);
    break;
    default:
      //Dont create another graph... refresh the search and stop here
      hour=category.replace(/[^\d]+/,"");
      hour=hour.replace(/[^\d]+/,"");
      document.getElementById('start_aaa').value = document.getElementById('start').value = byDateStart+" "+hour+":00:00";
      document.getElementById('end_aaa').value = document.getElementById('end').value = byDateEnd+ " "+hour+":59:59";
      RequestLines(); 
      MakeRequest();
      bold_dates();
      return;
    break;
  }
  UpdateByDate(urlres);
}
function monthToNumber(m)
{

	switch(m)
	{
		case "Jan":
			return "01";
			break;
		case "Feb":
			return "02";
			break;
		case "Mar":
			return "03";
			break;
		case "Apr":
			return "04";
			break;
		case "May":
			return "05";
			break;
		case "Jun":
			return "06";
			break;
		case "Jul":
			return "07";
			break;
		case "Aug":
			return "08";
			break;
		case "Sep":
			return "09";
			break;
		case "Oct":
			return "10";
			break;
		case "Nov":
			return "11";
			break;
		case "Dec":
			return "12";
			break;
		default:
			return 0;
			break;
	}
}

function SubmitForm() { document.forms[0].submit(); } 

function EnterSubmitForm(evt) {
  var evt = (evt) ? evt : ((event) ? event : null);
  if (evt.keyCode == 13) SubmitForm();
} 

function doQuery() {
  //hideLayer("by_date");
  SubmitForm();
}

function CalendarOnChange() {
	bold_dates('');
	setFixed2();
}

$(document).ready(function(){
	//UpdateByDate('forensic.php?graph_type=all&cat=');
	$('#date4').addClass('negrita');
	UpdateByDate('forensic.php?graph_type=month&cat=<?php echo urlencode(date("M, Y")) ?>');
	$("#start_aaa,#end_aaa").change(function(objEvent){
		CalendarOnChange();
	});
});

</script>
</head>
<body>
<?php
include ("../hmenu.php"); ?>
<a href="javascript:toggleLayer('by_date');"><img src="<?php echo $config["toggle_graph"]; ?>" border="0" title="Toggle Graph by date"> <small><font color="black">Graphs by dates</font></small></a>
<center>
<div id="by_date">
<a href="javascript:UpdateByDate('forensic.php?graph_type=all&cat=&uniqueID=<?php echo $uniqueid ?>');"><small><font color="black">Click to show the main chart</font></small></a>
<br/>
<?php
if (preg_match("/msie/i", $_SERVER['HTTP_USER_AGENT'])) { ?>
<IFRAME src="chart.php?gr=<?php echo urlencode("forensic_source.php?" . $_SERVER["QUERY_STRING"] . "&uniqueid=$uniqueid") ?>&w=1150&h=250" frameborder="0" style="width:1150px;height:250px;overflow:hidden"></IFRAME>
<?php
} else { ?>
<IFRAME src="chart.php?gr=<?php echo urlencode("forensic_source.php?" . $_SERVER["QUERY_STRING"] . "&uniqueid=$uniqueid") ?>&w=1150&h=230" frameborder="0" style="width:1150px;height:230px;overflow:hidden"></IFRAME>
<?php
} ?>
</div>
</center>
<div id="help" style="position:absolute; right:30px; top:5px";>
<img src="<?php echo $config["help_graph"] ?>" border="0" onMouseover="ddrivetip('<?php echo $help_entries["help_tooltip"]; ?>','lighblue', 300)" onMouseout="hideddrivetip()" onClick="toggleCursor()">
</div>
<! -- Misc internal vars -->
<form id="search" action="javascript:MakeRequest();">
<input type="hidden" id="cursor" value="">
<script>
document.getElementById('cursor').value = document.body.style.cursor;
</script>
<input type="hidden" id="offset" value="0">
<?php // Possible sort values: none, date, date_desc
 ?>
<input type="hidden" id="sort" value="none">
<input type="hidden" id="start" value="<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - ((24 * 60 * 60) * 31)) ?>">
<?php // Temporary fix until the server logs right
 ?>
<input type="hidden" id="end" value="<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>">
<!--
<div id="compress">
<center><a href="javascript:closeLayer('entiregauge');closeLayer('test');closeLayer('compress');" onMouseOver="showTip('<?php echo $help_entries["close_all"] ?>','lightblue','300')" onMouseOut="hideTip()"><font color="black"><?php echo _("Click here in order to compress everything") ?></a></center>
</div>
-->
<div id="saved_searches" style="display:none; position:absolute; background-color:#FFFFFF">
<?php
require_once ("manage_querys.php");
?>
</div>
<table cellspacing="0" width="100%" border="0" id="maintable">
<tr>
<td nowrap>
	<table cellspacing="0" width="100%" border="0" id="searchtable">
		<tr><td colspan="4" align="center" style="vertical-align:middle;" valign="middle" nowrap>
		
	<!--
	<a href="javascript:toggleLayer('saved_searches');" onMouseOver="showTip('<?php echo $help_entries["saved_searches"] ?>','lightblue','300')" onMouseOut="hideTip()"><img src="<?php echo $config["toggle_graph"]; ?>" border="0" title="Toggle Graph"> <small><font color="#AAAAAA"><?php echo _("Saved Searches") ?></font></small></a>

		
		<a href="javascript:AddQuery()" onMouseOver="showTip('<?php echo $help_entries["saved_searches"] ?>','lightblue','300')" onMouseOut="hideTip()"><img src="<?php echo $config["save_graph"] ?>" border="0" style="vertical-align:middle; padding-left:5px; padding-right:5px;"></a>
		-->
		<input type="text" id="searchbox" size="60" style="vertical-align:middle;" onKeyUp="return EnterSubmitForm(event)" onMouseOver="showTip('<?php echo $help_entries["search_box"] ?>','lightblue','300')" onMouseOut="hideTip()"><a onMouseOver="showTip('<?php echo $help_entries["play"] ?>','lightblue','300')" onMouseOut="hideTip()" href="javascript:doQuery()" title="<?php echo _("Submit Query") ?>"><img src="<?php echo $config["play_graph"]; ?>" border="0" align="middle" style="padding-left:5px; padding-right:5px;"></a>
		
		
	    <a href="javascript:ClearSearch()" onMouseOver="showTip('<?php echo $help_entries["clear"] ?>','lightblue','300')" onMouseOut="hideTip()"><font color="#999999"><small><?php echo _("Clear Query"); ?></small></font></a>

		
		</td></tr>
		<tr><td width="20">&nbsp;</td><td align="center" valign="middle" nowrap onMouseOver="showTip('<?php echo $help_entries["date_frame"] ?>','lightblue','300')" onMouseOut="hideTip()">
		Last: [ 
		<!-- <a href="javascript:setFixed('<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - (60 * 60)) ?>','<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>','day','<?php echo urlencode(date("M d, Y")) ?>');" onClick="javascript:bold_dates('date1');"><small id="date1"><?php echo _("Hour") ?></small></a>
		| -->
		<a href="javascript:setFixed('<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - (24 * 60 * 60)) ?>','<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>','day','<?php echo urlencode(date("M d, Y")) ?>');" onClick="javascript:bold_dates('date2');"><small id="date2"><?php echo _("24h"); ?></small></a>
		|
		<a href="javascript:setFixed('<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - ((24 * 60 * 60) * 7)) ?>','<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>','month','<?php echo urlencode(date("M, Y")) ?>');" onClick="javascript:bold_dates('date3');"><small id="date3"><?php echo _("Week") ?></small></a>
		|
		<a href="javascript:setFixed('<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - ((24 * 60 * 60) * 31)) ?>','<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>','month','<?php echo urlencode(date("M, Y")) ?>');" onClick="javascript:bold_dates('date4');"><small id="date4"><?php echo _("Month") ?></small></a>
		|
		<a href="javascript:setFixed('<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - ((24 * 60 * 60) * 365)) ?>','<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>','year','<?php echo urlencode(date("Y")) ?>');" onClick="javascript:bold_dates('date5');"><small id="date5"><?php echo _("Year") ?></small></a>
		]
		</td>
		<td valign="top" align="center" valign="middle" nowrap>
<?php
Util::draw_js_calendar(array(
    'input_name' => 'document.forms[0].start_aaa',
    true
) , false, "hideLayer('test')") ?>
<?php
if ($param_start != "" && $param_end != "" && date_parse($param_start) && date_parse($param_end)) {
?>
		<input type="text" size="17" id="start_aaa" name="start_aaa" value="<?php echo $param_start; ?>" style="vertical-align:middle;">
<small>-></small>
		<input type="text" size="17" id="end_aaa" name="end_aaa" value="<?php echo $param_end; ?>" style="vertical-align:middle;">

<?php
} else {
?>
		<input type="text" size="20" id="start_aaa" name="start_aaa" value="<?php echo strftime("%Y-%m-%d %H:%M:%S", time() - ((24 * 60 * 60) * 31)) ?>" style="vertical-align:middle;">
<small>-></small>
		<input type="text" size="20" id="end_aaa" name="end_aaa" value="<?php echo strftime("%Y-%m-%d %H:%M:%S", time()); ?>" style="vertical-align:middle;">
<?php
}
?>
<?php
Util::draw_js_calendar(array(
    'input_name' => 'document.forms[0].end_aaa',
    true
) , false, "hideLayer('test')") ?>
		<!-- <a href="javascript:setFixed2();" onClick="javascript:bold_dates('');" onMouseOver="showTip('<?php echo $help_entries["date_ack"] ?>','lightblue','300')" onMouseOut="hideTip()" title="<?php echo _("Fix Date") ?>"><img border="0" src="<?php echo $config["ok_graph"] ?>" style="padding-left:10px"></a> -->
		</td>
		<td nowrap width="150" align="middle" valign="center">
			<div id="numlines" style="vertical-align:middle; padding-right:10px">&nbsp;</div>
		</td>
		</tr>
	</table>
	</td>
	<td nowrap width="150" style="padding-left:15px" valign="top">
			<div id="loading" style="display:none; vertical-align:middle; padding-right:10px; padding-top:10px;"><img src="<?php echo $config["loading_graph"]; ?>" align="middle" style="vertical-align:middle;"> Loading... <a href="javascript:;" onclick="KillProcess()">Stop</a></div>
	</td>
	</tr>
	</table>
</form>
<a href="javascript:toggleLayer('test');"><img src="<?php echo $config["toggle_graph"]; ?>" border="0" title="Toggle Graph"> <small><font color="black"><?php echo _("Graphs") ?></font></small></a>
<center>
<div id="test" onMouseOver="showTip('<?php echo $help_entries["graphs"] ?>','lightblue','300')" onMouseOut="hideTip()" style="z-index:50;display:none">
<IFRAME src="chart.php?gr=<?php echo urlencode("storage_graphs4.php?label=" . _("Hosts") . "&what=sensor&uniqueid=$uniqueid") ?>" frameborder="0" style="width:250px;height:250px;overflow:hidden"></IFRAME>
<IFRAME src="chart.php?gr=<?php echo urlencode("storage_graphs.php?label=" . _("Event%20Types") . "&what=plugin_id&uniqueid=$uniqueid") ?>" frameborder="0" style="width:250px;height:250px;overflow:hidden"></IFRAME>
<IFRAME src="chart.php?gr=<?php echo urlencode("storage_graphs2.php?label=" . _("Sources") . "&what=src_ip&uniqueid=$uniqueid") ?>" frameborder="0" style="width:250px;height:250px;overflow:hidden"></IFRAME>
<IFRAME src="chart.php?gr=<?php echo urlencode("storage_graphs3.php?label=" . _("Destinations") . "&what=dst_ip&uniqueid=$uniqueid") ?>" frameborder="0" style="width:250px;height:250px;overflow:hidden"></IFRAME>
</div>
</center>
<hr>
<div id="ResponseDiv" onMouseOver="showTip('<?php echo $help_entries["result_box"] ?>','lightblue','300')" onMouseOut="hideTip()">
</div>
<script>
<?php
if ($param_start != "" && $param_end != "" && date_parse($param_start) && date_parse($param_end)) {
    print "setFixed('$param_start', '$param_end', '', '');\n";
} else {
    print "RequestLines();MakeRequest();\n";
}
?>
</script>

<div id="dhtmltooltip"></div>

<script type="text/javascript">

/***********************************************
* Cool DHTML tooltip script- © Dynamic Drive DHTML code library (www.dynamicdrive.com)
* This notice MUST stay intact for legal use
* Visit Dynamic Drive at http://www.dynamicdrive.com/ for full source code
***********************************************/

var offsetxpoint=-60 //Customize x offset of tooltip
var offsetypoint=20 //Customize y offset of tooltip
var ie=document.all
var ns6=document.getElementById && !document.all
var enabletip=false
if (ie||ns6)
	var tipobj=document.all? document.all["dhtmltooltip"] : document.getElementById? document.getElementById("dhtmltooltip") : ""

	function ietruebody(){
		return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
	}

	function ddrivetip(thetext, thecolor, thewidth){
		if (ns6||ie){
			if (typeof thewidth!="undefined") tipobj.style.width=thewidth+"px"
				if (typeof thecolor!="undefined" && thecolor!="") tipobj.style.backgroundColor=thecolor
					tipobj.innerHTML=thetext
					enabletip=true
					return false
		}
	}

	function positiontip(e){
		if (enabletip){
			var curX=(ns6)?e.pageX : event.clientX+ietruebody().scrollLeft;
			var curY=(ns6)?e.pageY : event.clientY+ietruebody().scrollTop;
			//Find out how close the mouse is to the corner of the window
			var rightedge=ie&&!window.opera? ietruebody().clientWidth-event.clientX-offsetxpoint : window.innerWidth-e.clientX-offsetxpoint-20
			var bottomedge=ie&&!window.opera? ietruebody().clientHeight-event.clientY-offsetypoint : window.innerHeight-e.clientY-offsetypoint-20

			var leftedge=(offsetxpoint<0)? offsetxpoint*(-1) : -1000

			//if the horizontal distance isn't enough to accomodate the width of the context menu
			if (rightedge<tipobj.offsetWidth)
				//move the horizontal position of the menu to the left by it's width
				tipobj.style.left=ie? ietruebody().scrollLeft+event.clientX-tipobj.offsetWidth+"px" : window.pageXOffset+e.clientX-tipobj.offsetWidth+"px"
				else if (curX<leftedge)
					tipobj.style.left="5px"
					else
						//position the horizontal position of the menu where the mouse is positioned
						tipobj.style.left=curX+offsetxpoint+"px"

						//same concept with the vertical position
						if (bottomedge<tipobj.offsetHeight)
							tipobj.style.top=ie? ietruebody().scrollTop+event.clientY-tipobj.offsetHeight-offsetypoint+"px" : window.pageYOffset+e.clientY-tipobj.offsetHeight-offsetypoint+"px"
							else
								tipobj.style.top=curY+offsetypoint+"px"
								tipobj.style.visibility="visible"
		}
	}

	function hideddrivetip(){
		if (ns6||ie){
			enabletip=false
			tipobj.style.visibility="hidden"
			tipobj.style.left="-1000px"
			tipobj.style.backgroundColor=''
			tipobj.style.width=''
		}
	}

	document.onmousemove=positiontip

	</script>
</body>
</html>
