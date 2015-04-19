<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/Upgrade.inc';
require_once 'classes/WebIndicator.inc';
Session::logcheck("MainMenu", "Index", "session/login.php");

$upgrade = new Upgrade();
?>
<html>
<head>
<title>OSSIM (Open Source Security Information Management)</title>
<link rel="stylesheet" type="TEXT/CSS" href="style/top.css">
</head>

<script LANGUAGE="JavaScript">
<!--
var newwindow;
function new_wind(url,name)
{
	newwindow=window.open(url,name,'height=768,width=1024,scrollbars=yes');
	if (window.focus) {newwindow.focus()}
}
 //-->
</script>


<body marginwidth=0 marginweight=0 topmargin=0 leftmargin=0 bgcolor=white>

<table border=0 cellpadding=0 cellspacing=0 width="100%">
<tr><td>
  <table border=0 cellpadding=0 cellspacing=0 width="100%"><tr>
  <td width=227><img src="pixmaps/top/fondo1.jpg" width=227 height=61 border=0></td>
  <td background="pixmaps/top/ry.gif">&nbsp;</td>
  <td width=493 align=right><img src="pixmaps/top/fondo2.jpg" width=493 height=61 border=0></td>
  </tr></table>
</td></tr>

<?
  require_once ('ossim_conf.inc');
  $conf = $GLOBALS["CONF"];

  $ntop_link = $conf->get_conf("ntop_link");
  $language = $conf->get_conf("language");
  $uc_languages = array("de_DE", "en_GB", "es_ES", "fr_FR", "pt_BR");
  $sensor_ntop = parse_url($ntop_link);

  $nagios_link = $conf->get_conf("nagios_link");
  $sensor_nagios = parse_url($nagios_link);
  if (!isset($sensor_nagios['host'])) {
    $sensor_nagios['host'] = $_SERVER['SERVER_NAME'];
  }

  $menu = array();
  if (Session::am_i_admin() && $upgrade->needs_upgrade()) {
    $menu["Upgrade"][] = array(
        "name" => gettext("System Upgrade Needed"),
        "id" => "System Upgrade Needed",
        "url"  => "upgrade/index.php"
    );
  }
  $placeholder = gettext("Dashboard");
  $placeholder = gettext("Events");
  $placeholder = gettext("Monitors");
  $placeholder = gettext("Incidents");
  $placeholder = gettext("Reports");
  $placeholder = gettext("Policy");
  $placeholder = gettext("Correlation");
  $placeholder = gettext("Configuration");
  $placeholder = gettext("Tools");
  $placeholder = gettext("Logout");

  // Passthrough Vars
  $status = "Open";
  if(GET('status') != null) $status = GET('status');

  /* Dashboard */
  if (Session::menu_perms("MenuControlPanel", "ControlPanelExecutive"))
    $menu["Dashboard"][] = array (
      "name" => gettext("Executive Panel"),
      "id" => "Executive Panel",
      "url" => "panel/"
    );
  if (Session::menu_perms("MenuControlPanel", "ControlPanelMetrics"))
    $menu["Dashboard"][] = array (
      "name" => gettext("Aggregated Risk"),
      "id" => "Metrics",
      "url" => "control_panel/global_score.php"
    );
  if (Session::menu_perms("MenuControlPanel", "ControlPanelAlarms"))
    $menu["Dashboard"][] = array (
      "name" => gettext("Alarms"),
      "id" => "Alarms",
      "url" => "control_panel/alarm_console.php?&hide_closed=1"
    );
  if (Session::menu_perms("MenuControlPanel", "BusinessProcesses"))
    $menu["Dashboard"][] = array (
      "name" => gettext("Business Processes"),
      "id" => "Business Processes",
      "url" => "business_processes/index.php"
    );

  if (Session::menu_perms("MenuControlPanel", "Help"))
    $menu["Dashboard"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:control_panel','Dashboard Help');"
    );

  /* Incidents */
  if (Session::menu_perms("MenuIncidents", "IncidentsIncidents"))
    $menu["Incidents"][] = array (
      "name" => gettext("Incidents"),
      "id" => "Incidents",
      "url" => "incidents/index.php?status=$status"
    );
  if (Session::menu_perms("MenuIncidents", "IncidentsTypes"))
    $menu["Incidents"][] = array (
      "name" => gettext("Types"),
      "id" => "Types",
      "url" => "incidents/incidenttype.php"
    );
  if (Session::menu_perms("MenuIncidents", "IncidentsTags"))
    $menu["Incidents"][] = array (
      "name" => gettext("Tags"),
      "id" => "Tags",
      "url" => "incidents/incidenttag.php"
    );
  if (Session::menu_perms("MenuIncidents", "IncidentsReport"))
    $menu["Incidents"][] = array (
      "name" => gettext("Report"),
      "id" => "Report",
      "url" => "report/incidentreport.php"
    );
  if (Session::menu_perms("MenuIncidents", "Help"))
    $menu["Incidents"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:incidents','Help');"
    );


	/* Events */
  if (Session::menu_perms("MenuEvents", "EventsForensics"))
    $menu["Events"][] = array (
      "name" => gettext("Forensics"),
      "id" => "Forensics",
      "url" => $conf->get_conf("acid_link") . "/".$conf->get_conf("event_viewer")."_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
    );
  if (Session::menu_perms("MenuEvents", "EventsVulnerabilities"))
    $menu["Events"][] = array (
      "name" => gettext("Vulnerabilities"),
      "id" => "Vulnerabilities",
      "url" => "vulnmeter/index.php"
    );
  if (Session::menu_perms("MenuEvents", "EventsAnomalies"))
    $menu["Events"][] = array (
      "name" => gettext("Anomalies"),
      "id" => "Anomalies",
      "url" => "control_panel/anomalies.php"
    );
  if (Session::menu_perms("MenuEvents", "EventsRT"))
    $menu["Events"][] = array (
      "name" => gettext("RT Events"),
      "id" => "RT Events",
      "url" => "control_panel/event_panel.php"
    );
  if (Session::menu_perms("MenuEvents", "EventsViewer"))
    $menu["Events"][] = array (
      "name" => gettext("Event Viewer"),
      "id" => "Events Viewer",
      "url" => "event_viewer/index.php"
    );
  if (Session::menu_perms("MenuEvents", "Help Events"))
    $menu["Events"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:events','Event Help');"
    );





  /* Monitors */
  if (Session::menu_perms("MenuMonitors", "MonitorsRiskmeter"))
    $menu["Monitors"][] = array (
      "name" => gettext("Riskmeter"),
      "id" => "Riskmeter",
      "url" => "riskmeter/index.php"
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsSession"))
    $menu["Monitors"][] = array (
      "name" => gettext("Session"),
      "id" => "Session",
      "url" => "ntop/session.php?sensor=" . $sensor_ntop["host"]
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsNetwork"))
    $menu["Monitors"][] = array (
      "name" => gettext("Network"),
      "id" => "Network",
      "url" => "ntop/index.php?sensor=" . $sensor_ntop["host"]
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsAvailability"))
    $menu["Monitors"][] = array (
      "name" => gettext("Availability"),
      "id" => "Availability",
      "url" => "nagios/index.php?sensor=" . $sensor_nagios["host"]
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsSensors"))
    $menu["Monitors"][] = array (
      "name" => gettext("Sensors"),
      "id" => "Sensors",
      "url" => "sensor/sensor_plugins.php"
    );
  if (Session::menu_perms("MenuMonitors", "Help"))
    $menu["Monitors"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:monitors','Help');"
    );


  /* Reports */
  if (Session::menu_perms("MenuReports", "ReportsHostReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Host Report"),
      "id" => "Host Report",
      "url" => "report/report.php"
    );
  if (Session::menu_perms("MenuReports", "ReportsAlarmReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Alarm Report"),
      "id" => "Alarm Report",
      "url" => "report/sec_report.php?section=all&type=alarm"
    );
  if (Session::menu_perms("MenuReports", "ReportsSecurityReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Security Report"),
      "id" => "Security Report",
      "url" => "report/sec_report.php?section=all"
    );
  if (Session::menu_perms("MenuReports", "ReportsPDFReport"))
    $menu["Reports"][] = array (
      "name" => gettext("PDF Report"),
      "id" => "PDF Report",
      "url" => "report/pdfreportform.php"
    );
  if (Session::menu_perms("MenuReports", "Help"))
    $menu["Reports"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:reports','Help');"
    );


  /* Policy */
  if (Session::menu_perms("MenuPolicy", "PolicyPolicy"))
    $menu["Policy"][] = array (
      "name" => gettext("Policy"),
      "id" => "Policy",
      "url" => "policy/policy.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyHosts"))
    $menu["Policy"][] = array (
      "name" => gettext("Hosts"),
      "id" => "Hosts",
      "url" => "host/host.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyHosts"))
    $menu["Policy"][] = array (
      "name" => gettext("Host groups"),
      "id" => "Host groups",
      "url" => "host/hostgroup.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyNetworks"))
    $menu["Policy"][] = array (
      "name" => gettext("Networks"),
      "id" => "Networks",
      "url" => "net/net.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyNetworks"))
    $menu["Policy"][] = array (
      "name" => gettext("Network groups"),
      "id" => "Network groups",
      "url" => "net/netgroup.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicySensors"))
    $menu["Policy"][] = array (
      "name" => gettext("Sensors"),
      "id" => "Sensors",
      "url" => "sensor/sensor.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyServers"))
    $menu["Policy"][] = array (
      "name" => gettext("Servers"),
      "id" => "Servers",
      "url" => "server/server.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyPorts"))
    $menu["Policy"][] = array (
      "name" => gettext("Ports"),
      "id" => "Ports",
      "url" => "port/port.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyActions"))
    $menu["Policy"][] = array (
      "name" => gettext("Actions"),
      "id" => "Actions",
      "url" => "action/action.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyResponses"))
    $menu["Policy"][] = array (
      "name" => gettext("Responses"),
      "id" => "Responses",
      "url" => "response/response.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyPluginGroups"))
    $menu["Policy"][] = array (
      "name" => gettext("Plugin Groups"),
      "id" => "Plugin Groups",
      "url" => "policy/plugingroups.php"
    );
  if (Session::menu_perms("MenuReports", "Help"))
    $menu["Policy"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:policy','Help');"
    );

  /* Correlation */
  if (Session::menu_perms("MenuCorrelation", "CorrelationDirectives"))
    $menu["Correlation"][] = array(
      "name" => gettext("Directives"),
      "id" => "Directives",
      "url" => "directives/index.php"
    );
  if (Session::menu_perms("MenuCorrelation", "CorrelationCrossCorrelation"))
    $menu["Correlation"][] = array (
      "name" => gettext("Cross Correlation"),
      "id" => "Cross Correlation",
      "url" => "conf/pluginref.php"
    );
  if (Session::menu_perms("MenuCorrelation", "CorrelationBacklog"))
    $menu["Correlation"][] = array(
      "name" => gettext("Backlog"),
      "id" => "Backlog",
      "url" => "control_panel/backlog.php"
    );
  if (Session::menu_perms("MenuReports", "Help"))
    $menu["Correlation"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:correlation','Help');"
    );

  /* Configuration */
  if (Session::menu_perms("MenuConfiguration", "ConfigurationMain"))
    $menu["Configuration"][] = array (
      "name" => gettext("Main"),
      "id" => "Main",
      "url" => "conf/index.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationUsers"))
    $menu["Configuration"][] = array (
      "name" => gettext("Users"),
      "id" => "Users",
      "url" => "session/users.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationPlugins"))
    $menu["Configuration"][] = array (
      "name" => gettext("Plugins"),
      "id" => "Plugins",
      "url" => "conf/plugin.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationRRDConfig"))
    $menu["Configuration"][] = array (
      "name" => gettext("RRD Config"),
      "id" => "RRD Config",
      "url" => "rrd_conf/rrd_conf.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationHostScan"))
    $menu["Configuration"][] = array(
      "name" => gettext("Host Scan"),
      "id" => "Host Scan",
      "url" => "scan/hostscan.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationUserActionLog"))
    $menu["Configuration"][] = array(
      "name" => gettext("User action logs"),
      "id" => "User action logs",
      "url" => "conf/userlog.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationEmailTemplate"))
    $menu["Configuration"][] = array(
      "name" => gettext("Incidents Email Template"),
      "id" => "Incidents Email Template",
      "url" => "conf/emailtemplate.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationUpgrade"))
    $menu["Configuration"][] = array(
      "name" => gettext("Upgrade"),
      "id" => "Upgrade",
      "url" => "upgrade/"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationMaps"))
    $menu["Configuration"][] = array(
      "name" => gettext("Maps"),
      "id" => "Maps",
      "url" => "maps/"
    );
  if (Session::menu_perms("MenuConfiguration", "Help"))
    $menu["Configuration"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:configuration','Help');"
    );

  /* Tools */
  if (Session::menu_perms("MenuTools", "ToolsScan"))
    $menu["Tools"][] = array (
      "name" => gettext("Net Scan"),
      "id" => "Net Scan",
      "url" => "netscan/index.php"
    );
  if (Session::menu_perms("MenuTools", "ToolsRuleViewer"))
    $menu["Tools"][] = array (
      "name" => gettext("Rule Viewer"),
      "id" => "Rule Viewer",
      "url" => "editor/editor.php"
    );
  if (Session::menu_perms("MenuTools", "ToolsBackup"))
    $menu["Tools"][] = array (
      "name" => gettext("Backup"),
      "id" => "Backup",
      "url" => "backup/index.php"
    );
  if (Session::menu_perms("MenuTools", "ToolsUserLogViewer"))
    $menu["Tools"][] = array (
      "name" => gettext("User log"),
      "id" => "User log",
      "url" => "userlog/user_action_log.php"
    );
  if (Session::menu_perms("MenuTools", "ToolsDownloads"))
    $menu["Tools"][] = array (
      "name" => gettext("Downloads"),
      "id" => "Downloads",
      "url" => "downloads/index.php"
    );
  if (Session::menu_perms("MenuTools", "Help"))
    $menu["Tools"][] = array (
      "name" => gettext("Help"),
      "id" => "Help",
      "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:tools','Help');"
    );

  /* Logout */
  $menu["Logout"] = "session/login.php?action=logout"; // Plain url if no array entry

  $option = GET('option');
  $soption = GET('soption');
  $url = GET('url');

  if (empty($option)) $option = 0;
  if (!isset($soption)) {
  	if (isset($_SESSION["_TopMenu_" . $option]))
        $soption = $_SESSION["_TopMenu_" . $option];
    else
        $soption = 0;
  } else {
  	$_SESSION["_TopMenu_" . $option] = $soption; 
  }

  $keys=array_keys($menu);
?>

<!-- MENU -->
<tr><td height=23 background="pixmaps/top/naranja.gif" style="padding-left:30px; padding-top:1px">

  <table border=0 cellpadding=0 cellspacing=0><tr>
<? $i=0; foreach ($menu as $name => $opc) { ?>
  <td style="padding-right:10px">
     <table border=0 cellpadding=0 cellspacing=0><tr>
       <? if ($option==$i) { ?>
       <td style="padding-right:3px"><img src="pixmaps/top/abajo.gif" border=0></td>
       <td class=blue NOWRAP>
	   <? if(in_array($language, $uc_languages)) echo htmlentities(strtoupper(html_entity_decode(gettext($name)))); else echo gettext($name); ?>
	   </td>
       <? } else { ?>
       <td style="padding-right:3px"><img src="pixmaps/top/dcha.gif" border=0></td>
       <td class=white NOWRAP>
        <a class=white href="top.php?option=<?=$i ?>"><?=gettext($name);?></a>
<?php
if($name == "Logout"){
echo "<b> [<font color=\"black\">" .  $_SESSION["_user"] . "</font>] </b>";
}
?>&nbsp;</td>
       <? }
if($name == "Policy"){
    echo "<td id=\"ReloadPolicy\" style=\"display:none\"><img src=\"pixmaps/top/reload-policy.gif\" title='Policy reload needed' /></td>";
}
?>
     </tr></table>
   </td>
<? $i++; } ?>
  </tr></table>

</td></tr>

<!-- SUBMENU -->
<tr><td height=24 background="pixmaps/top/azul.gif" valign=bottom style="padding-left:30px">

  <table border=0 cellpadding=0 cellspacing=0 width="100%"><tr>
	<table align="left" border=0 cellpadding=0 cellspacing=0><tr>
<? if (!is_array($menu[$keys[$option]])) {
    // jump to url if not array
    $url = $menu[$keys[$option]];
    }
  else {
    foreach ($menu[$keys[$option]] as $i => $op) {
      if ($soption==$i && !(GET('soption'))) $url = $op["url"];

	if($op["id"]=="Help")
	{
		?></tr></table><table border=0 cellpadding=0 cellspacing=0 align="right"><tr><td width="*"></td>
		<?
	}?>

    <td <? if ($op["id"]=="Help") echo ' align="right" '; ?> >
      <table <? if ($op["id"]=="Help")echo ' align="right"'; ?> border=0 cellpadding=0 cellspacing=0><tr>
        <? if ($soption==$i) { ?>
        <td ><img src="pixmaps/top/li.gif" border=0></td>
        <td class=blue bgcolor=white style="padding-left:8px;padding-right:8px"><a class=blue
	<?
		if($op["id"]!="Help")
		{
			?>href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<?  echo urlencode($op["url"]) ?>"><?
		}
		else
		{
			?>href="<? echo $op["url"] ?>"><?
		}
		if(in_array($language, $uc_languages)) echo htmlentities(strtoupper(html_entity_decode($op["name"]))); else echo $op["name"]; ?></a></td>
        <? } else { ?>
        <td class=blue style="padding-left:8px;padding-right:8px"><a class=blue  
<?
                if($op["id"]!="Help")
                {
                        ?>href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<? echo urlencode($op["url"]) ?>"><?
		}
		else
		{
			?>href="<? echo $op["url"] ?>"><?
		}
 echo $op["name"]; #echo strtoupper($op["name"]) ?></a></td>
        <? }
$menu1 = $keys[$option];
$menu2 = $op["id"];
if($menu1 == "Policy" && $menu2 == "Policy") {
    echo "<td "; if ($soption==$i) echo "bgcolor=white ";
    echo "id=\"Reload_policies\"><img src='pixmaps/top/reload-policy.gif' title='Policies reload needed' /></td>";
}
if($menu1 == "Policy" && $menu2 == "Hosts") {
    echo "<td "; if ($soption==$i) echo "bgcolor=white ";
    echo "id=\"Reload_hosts\"><img src='pixmaps/top/reload-policy.gif' title='Hosts reload needed' /></td>";
}
if($menu1 == "Policy" && $menu2 == "Networks") {
    echo "<td "; if ($soption==$i) echo "bgcolor=white ";
    echo "id=\"Reload_nets\"><img src='pixmaps/top/reload-policy.gif' title='Nets reload needed' /></td>";
}
if($menu1 == "Policy" && $menu2 == "Sensors") {
    echo "<td "; if ($soption==$i) echo "bgcolor=white ";
    echo "id=\"Reload_sensors\"><img src='pixmaps/top/reload-policy.gif' title='Sensors reload needed' /></td>";
}
if($menu1 == "Policy" && $menu2 == "Servers") {
    echo "<td "; if ($soption==$i) echo "bgcolor=white ";
    echo "id=\"Reload_servers\"><img src='pixmaps/top/reload-policy.gif' title='Servers reload needed' /></td>";
}
?>
        <? if ($soption==$i) { ?>
        <td><img src="pixmaps/top/ld.gif" border=0></td>
        <? } else { ?>
        <td>&nbsp;</td>
        <? } ?>
      </tr></table>
    </td>
    <? if ($i+1!=$soption&&$op["id"]!="Help") { ?> <td><img src="pixmaps/top/sep.gif" border=0></td> <? } ?>
<?  }
   }
?> </tr></table>

</td></tr>
</table>

<? if ($url!="") { ?>
<script> parent.frames["main"].document.location.href = '<? echo $url ?>' </script>
<? }
$OssimWebIndicator->update_display();
?>
</body>
</html>


