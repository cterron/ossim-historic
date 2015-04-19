<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/Upgrade.inc';
Session::logcheck("MainMenu", "Index", "session/login.php");

$upgrade = new Upgrade();
?>
<html>
<head>
<title>OSSIM (Open Source Security Information Management)</title>
<link rel="stylesheet" type="TEXT/CSS" href="style/top.css">
</head>

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
        "url"  => "upgrade/index.php"
    );
  }
  $placeholder = gettext("Control Panel");
  $placeholder = gettext("Reports");
  $placeholder = gettext("Incidents");
  $placeholder = gettext("Monitors");
  $placeholder = gettext("Policy");
  $placeholder = gettext("Correlation");
  $placeholder = gettext("Configuration");
  $placeholder = gettext("Tools");
  $placeholder = gettext("Logout");


  /* Control Panel */
  if (Session::menu_perms("MenuControlPanel", "ControlPanelExecutive"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Executive Panel") , 
      "url" => "panel/"
    );
  
  if (Session::menu_perms("MenuControlPanel", "ControlPanelMetrics"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Metrics") , 
      "url" => "control_panel/global_score.php"
    );

  if (Session::menu_perms("MenuControlPanel", "ControlPanelAlarms"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Alarms"),
      "url" => "control_panel/alarm_console.php?&hide_closed=1"
    );

  if (Session::menu_perms("MenuControlPanel", "ControlPanelEvents"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Events"),
      "url" => $conf->get_conf("acid_link") . "/".$conf->get_conf("event_viewer")."_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
    );
    
  if (Session::menu_perms("MenuControlPanel", "ControlPanelVulnerabilities"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Vulnerabilities"),
      "url" => "vulnmeter/index.php"
    );

  if (Session::menu_perms("MenuControlPanel", "ControlPanelAnomalies"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Anomalies"),
      "url" => "control_panel/anomalies.php"
    );

  if (Session::menu_perms("MenuControlPanel", "ControlPanelHids"))
    $menu["Control Panel"][] = array (
      "name" => gettext("Hids"),
      "url" => "hids/index.php"
    );



  /* Reports */
  if (Session::menu_perms("MenuReports", "ReportsHostReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Host Report"),
      "url" => "report/report.php"
    );

  if (Session::menu_perms("MenuReports", "ReportsAlarmReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Alarm Report"),
      "url" => "report/sec_report.php?section=all&type=alarm"
    );

  if (Session::menu_perms("MenuReports", "ReportsSecurityReport"))
    $menu["Reports"][] = array (
      "name" => gettext("Security Report"),
      "url" => "report/sec_report.php?section=all"
    );

  if (Session::menu_perms("MenuReports", "ReportsPDFReport"))
    $menu["Reports"][] = array (
      "name" => gettext("PDF Report"),
      "url" => "report/pdfreportform.php"
    );

  /* Incidents */
  if (Session::menu_perms("MenuIncidents", "IncidentsIncidents"))
    $menu["Incidents"][] = array (
      "name" => gettext("Incidents"),
      "url" => "incidents/index.php?&status=Open"
    );

  if (Session::menu_perms("MenuIncidents", "IncidentsTypes"))
    $menu["Incidents"][] = array (
      "name" => gettext("Types"),
      "url" => "incidents/incidenttype.php"
    );

  if (Session::menu_perms("MenuIncidents", "IncidentsTags"))
    $menu["Incidents"][] = array (
      "name" => gettext("Tags"),
      "url" => "incidents/incidenttag.php"
    );

  if (Session::menu_perms("MenuIncidents", "IncidentsReport"))
    $menu["Incidents"][] = array (
      "name" => gettext("Report"),
      "url" => "report/incidentreport.php"
    );

  /* Riskmeter */
  if (Session::menu_perms("MenuMonitors", "MonitorsRiskmeter"))
    $menu["Monitors"][] = array (
      "name" => gettext("Riskmeter"),
      "url" => "riskmeter/index.php"
    );

  if (Session::menu_perms("MenuMonitors", "MonitorsSession"))
    $menu["Monitors"][] = array (
      "name" => gettext("Session"),
      "url" => "ntop/session.php?sensor=" . $sensor_ntop["host"]
    );

  if (Session::menu_perms("MenuMonitors", "MonitorsNetwork"))
    $menu["Monitors"][] = array (
      "name" => gettext("Network"), 
      "url" => "ntop/index.php?sensor=" . $sensor_ntop["host"]
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsAvailability"))
    $menu["Monitors"][] = array (
      "name" => gettext("Availability"),
      "url" => "nagios/index.php?sensor=" . $sensor_nagios["host"]
    );
  if (Session::menu_perms("MenuMonitors", "MonitorsSensors"))
    $menu["Monitors"][] = array (
      "name" => gettext("Sensors"),
      "url" => "sensor/sensor_plugins.php"
    );

  /* Policy */

  if (Session::menu_perms("MenuPolicy", "PolicyPolicy"))
    $menu["Policy"][] = array (
      "name" => gettext("Policy"),
      "url" => "policy/policy.php"
    );


  if (Session::menu_perms("MenuPolicy", "PolicyHosts"))
    $menu["Policy"][] = array (
      "name" => gettext("Hosts"), 
      "url" => "host/host.php"
    );

  if (Session::menu_perms("MenuPolicy", "PolicyNetworks"))
    $menu["Policy"][] = array (
      "name" => gettext("Networks"),
      "url" => "net/net.php"
    );

  if (Session::menu_perms("MenuPolicy", "PolicyNetworks"))
    $menu["Policy"][] = array (
      "name" => gettext("Network groups"),
      "url" => "net/netgroup.php"
    );

  if (Session::menu_perms("MenuPolicy", "PolicySensors"))
    $menu["Policy"][] = array (
      "name" => gettext("Sensors"),
      "url" => "sensor/sensor.php"
    );

/*
  if (Session::menu_perms("MenuPolicy", "PolicySignatures"))
    $menu["Policy"][] = array (
      "name" => gettext("Signatures"),
      "url" => "signature/signature.php"
    );
*/

  if (Session::menu_perms("MenuPolicy", "PolicyPorts"))
    $menu["Policy"][] = array (
      "name" => gettext("Ports"),
      "url" => "port/port.php"
    );

  if (Session::menu_perms("MenuPolicy", "PolicyActions"))
    $menu["Policy"][] = array (
      "name" => gettext("Actions"),
      "url" => "action/action.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyResponses"))
    $menu["Policy"][] = array (
      "name" => gettext("Responses"),
      "url" => "response/response.php"
    );
  if (Session::menu_perms("MenuPolicy", "PolicyPluginGroups"))
    $menu["Policy"][] = array (
      "name" => gettext("Plugin Groups"),
      "url" => "policy/plugingroups.php"
    );
  
  /* Correlation */
  if (Session::menu_perms("MenuCorrelation", "CorrelationDirectives"))
    $menu["Correlation"][] = array(
      "name" => gettext("Directives"),
      "url" => "directives/index.php"
    );

  if (Session::menu_perms("MenuCorrelation", "CorrelationCrossCorrelation"))
    $menu["Correlation"][] = array (
      "name" => gettext("Cross Correlation"),
      "url" => "conf/pluginref.php"
    );

  if (Session::menu_perms("MenuCorrelation", "CorrelationBacklog"))
    $menu["Correlation"][] = array(
      "name" => gettext("Backlog"),
      "url" => "control_panel/backlog.php"
    );

  /* Configuration */
  if (Session::menu_perms("MenuConfiguration", "ConfigurationMain"))
    $menu["Configuration"][] = array (
      "name" => gettext("Main"),
      "url" => "conf/main.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationUsers"))
    $menu["Configuration"][] = array (
      "name" => gettext("Users"),
      "url" => "session/users.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationPlugins"))
    $menu["Configuration"][] = array (
      "name" => gettext("Plugins"),
      "url" => "conf/plugin.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationRRDConfig"))
    $menu["Configuration"][] = array (
      "name" => gettext("RRD Config"),
      "url" => "rrd_conf/rrd_conf.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationHostScan"))
    $menu["Configuration"][] = array(
      "name" => gettext("Host Scan"),
      "url" => "scan/hostscan.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationUserActionLog"))
    $menu["Configuration"][] = array(
      "name" => gettext("User action logs"),
      "url" => "conf/userlog.php"
    );

  if (Session::menu_perms("MenuConfiguration", "ConfigurationEmailTemplate"))
    $menu["Configuration"][] = array(
      "name" => gettext("Incidents Email Template"),
      "url" => "conf/emailtemplate.php"
    );
  if (Session::menu_perms("MenuConfiguration", "ConfigurationUpgrade"))
    $menu["Configuration"][] = array(
      "name" => gettext("Upgrade"),
      "url" => "upgrade/"
    );

  /* Tools */
  if (Session::menu_perms("MenuTools", "ToolsScan"))
    $menu["Tools"][] = array (
      "name" => gettext("Net Scan"),
      "url" => "netscan/index.php"
    );

  if (Session::menu_perms("MenuTools", "ToolsRuleViewer"))
    $menu["Tools"][] = array (
      "name" => gettext("Rule Viewer"),
      "url" => "editor/editor.php"
    );

  if (Session::menu_perms("MenuTools", "ToolsBackup"))
    $menu["Tools"][] = array (
      "name" => gettext("Backup"),
      "url" => "backup/index.php"
    );

  if (Session::menu_perms("MenuTools", "ToolsUserLogViewer"))
    $menu["Tools"][] = array (
      "name" => gettext("User log"),
      "url" => "userlog/user_action_log.php"
    );

  /* Logout */
  $menu["Logout"] = "session/login.php?action=logout"; // Plain url if no array entry

  $option = GET('option');
  $soption = GET('soption');
  $url = GET('url');
    
    if (empty($option)) $option = 0;
    if (empty($soption)) $soption = 0;
    
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
	   <? if($language != "ru_RU.UTF-8") echo strtoupper(gettext($name)); else echo gettext($name); ?>
	   </td>
       <? } else { ?>
       <td style="padding-right:3px"><img src="pixmaps/top/dcha.gif" border=0></td>
       <td class=white NOWRAP>
        <a class=white href="top.php?option=<?=$i ?>"><?=gettext($name);?></a>
<?php
if($name == "Logout"){
echo "<b> [<font color=\"black\">" .  $_SESSION["_user"] . "</font>] </b>"; 
}
?></td>
       <? } ?>
     </tr></table>
   </td>
<? $i++; } ?>
  </tr></table>

</td></tr>

<!-- SUBMENU -->
<tr><td height=24 background="pixmaps/top/azul.gif" valign=bottom style="padding-left:30px">

  <table border=0 cellpadding=0 cellspacing=0><tr>
<? if (!is_array($menu[$keys[$option]])) { 
    // jump to url if not array
    $url = $menu[$keys[$option]];
    }
  else {     
    foreach ($menu[$keys[$option]] as $i => $op) { 
      if ($soption==$i && !(GET('soption'))) $url = $op["url"];
    ?>
    <td>
      <table border=0 cellpadding=0 cellspacing=0><tr>
        <? if ($soption==$i) { ?>
        <td><img src="pixmaps/top/li.gif" border=0></td>
        <td class=blue bgcolor=white style="padding-left:8px;padding-right:8px"><a class=blue
		href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<?  echo urlencode($op["url"]) ?>"><? if($language != "ru_RU.UTF-8") echo strtoupper($op["name"]); else echo $op["name"]; ?></a></td>
        <td><img src="pixmaps/top/ld.gif" border=0></td>
        <? } else { ?>
        <td class=blue style="padding-left:8px;padding-right:8px"><a class=blue href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<? echo urlencode($op["url"]) ?>"><? echo $op["name"]; #echo strtoupper($op["name"]) ?></a></td>
        <? } ?>
      </tr></table>
    </td>
    <? if ($i+1!=$soption) { ?> <td><img src="pixmaps/top/sep.gif" border=0></td> <? } ?>
<?  } 
   } 
?> </tr></table>

</td></tr>
</table>

<? if ($url!="") { ?>
<script> parent.frames["main"].document.location.href = '<? echo $url ?>' </script>
<? } ?>
</body>
</html>


