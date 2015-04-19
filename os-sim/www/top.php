<?php
require_once ('classes/Session.inc');
Session::logcheck("MainMenu", "Index", "session/login.php");
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
  $conf = new ossim_conf();

  $ntop_link = $conf->get_conf("ntop_link");
  $sensor = parse_url($ntop_link);

  $menu = array();
  $placeholder = gettext("Control Panel");
  $placeholder = gettext("Reports");
  $placeholder = gettext("Monitors");
  $placeholder = gettext("Policy");
  $placeholder = gettext("Correlation");
  $placeholder = gettext("Configuration");
  $placeholder = gettext("Tools");
  $placeholder = gettext("Logout");

  $menu["Control Panel"][] = array("name" => gettext("Metrics") , "url" => "control_panel/global_score.php");
  $menu["Control Panel"][] = array("name" => gettext("Alarms") , "url" => "control_panel/alarm_console.php");
  $menu["Control Panel"][] = array("name" => gettext("Alerts") , "url" => $conf->get_conf("acid_link") . "/acid_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d");
  $menu["Control Panel"][] = array("name" => gettext("Vulnerabilities") , "url" => "vulnmeter/index.php");
  //
  $menu["Reports"][] = array("name" => gettext("Host Report") , "url" => "report/report.php");
  $menu["Reports"][] = array("name" => gettext("Security Report") , "url" => "report/sec_report.php?section=all");
  $menu["Reports"][] = array("name" => gettext("PDF Report") , "url" => "report/pdfreportform.php");
  $menu["Reports"][] = array("name" => gettext("Anomalies") , "url" => "control_panel/anomalies.php");
  $menu["Reports"][] = array("name" => gettext("Incidents") , "url" => "incidents/index.php");
  //
  $menu["Monitors"][] = array("name" => gettext("Riskmeter") , "url" => "riskmeter/index.php");
  $menu["Monitors"][] = array("name" => gettext("Session") , "url" => "ntop/session.php?sensor=" . $sensor["host"]);
  $menu["Monitors"][] = array("name" => gettext("Network") , "url" => "ntop/index.php?sensor=" . $sensor["host"]);
  $menu["Monitors"][] = array("name" => gettext("Availability") , "url" => $conf->get_conf("opennms_link"));
  $menu["Monitors"][] = array("name" => gettext("Sensors") , "url" => "sensor/sensor_plugins.php");
  //
  $menu["Policy"][] = array("name" => gettext("Policy") , "url" => "policy/policy.php");
  $menu["Policy"][] = array("name" => gettext("Hosts") , "url" => "host/host.php");
  $menu["Policy"][] = array("name" => gettext("Networks") , "url" => "net/net.php");
  $menu["Policy"][] = array("name" => gettext("Network groups") , "url" => "net/netgroup.php");
  $menu["Policy"][] = array("name" => gettext("Sensors") , "url" => "sensor/sensor.php");
  $menu["Policy"][] = array("name" => gettext("Signatures") , "url" => "signature/signature.php");
  $menu["Policy"][] = array("name" => gettext("Ports") , "url" => "port/port.php");
  //
  $menu["Correlation"][] = array("name" => gettext("Directives") , "url" => "directives/index.php");
  $menu["Correlation"][] = array("name" => gettext("Cross Correlation") , "url" => "conf/pluginref.php");
  $menu["Correlation"][] = array("name" => gettext("Backlog") , "url" => "control_panel/backlog.php");
  //
  $menu["Configuration"][] = array("name" => gettext("Main") , "url" => "conf/main.php");
  $menu["Configuration"][] = array("name" => gettext("Users") , "url" => "session/users.php");
  $menu["Configuration"][] = array("name" => gettext("Plugins") , "url" => "conf/plugin.php");
  $menu["Configuration"][] = array("name" => gettext("RRD Config") , "url" => "rrd_conf/rrd_conf.php");
  $menu["Configuration"][] = array("name" => gettext("Host Scan") , "url" => "scan/hostscan.php");
  //
  $menu["Tools"][] = array("name" => gettext("Net Scan") , "url" => "netscan/index.php");
  $menu["Tools"][] = array("name" => gettext("Rule Viewer") , "url" => "editor/editor.php");
  $menu["Tools"][] = array("name" => gettext("Backup") , "url" => "backup/index.php");
  //
  $menu["Logout"] = "session/login.php?action=logout"; // Plain url if no array entry

  $option=0;
  $soption=0;
  $url = "";
  if (isset($_GET["option"])) $option=$_GET["option"];
  if (isset($_GET["soption"])) $soption=$_GET["soption"];
  if (isset($_GET["url"])) $url=$_GET["url"];
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
       <td class=blue><? echo strtoupper(gettext($name)) ?></td>
       <? } else { ?>
       <td style="padding-right:3px"><img src="pixmaps/top/dcha.gif" border=0></td>
       <td class=white><a class=white href="top.php?option=<? echo $i ?>">
<?  echo strtoupper(gettext($name)) ?></a></td>
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
      if ($soption==$i && !isset($_GET["soption"])) $url = $op["url"];
    ?>
    <td>
      <table border=0 cellpadding=0 cellspacing=0><tr>
        <? if ($soption==$i) { ?>
        <td><img src="pixmaps/top/li.gif" border=0></td>
        <td class=blue bgcolor=white style="padding-left:8px;padding-right:8px"><a class=blue href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<? echo urlencode($op["url"]) ?>"><? echo strtoupper($op["name"]) ?></a></td>
        <td><img src="pixmaps/top/ld.gif" border=0></td>
        <? } else { ?>
        <td class=blue style="padding-left:8px;padding-right:8px"><a class=blue href="top.php?option=<? echo $option ?>&soption=<? echo $i ?>&url=<? echo urlencode($op["url"]) ?>"><? echo strtoupper($op["name"]) ?></a></td>
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


