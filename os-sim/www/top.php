<html>
<head>
  <title>OSSIM</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="style/style.css"/>
</head>
<body>


<?php
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
?>

  <table align="center" border="0">
    <tr>
      <th align="center">

<?php
    if ($_GET["menu"] == "control_panel") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=control_panel" 
           title="Control Panel"><font color="#991e1e">Control
           Panel</font>&nbsp;<img border="0" src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=control_panel" 
           title="Control Panel">Control Panel&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
    
<?php
    } 

    if ($_GET["menu"] == "report") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=report" 
           title="Report"><font color="#991e1e">Reports</font>&nbsp;<img
           border="0" src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=report" 
           title="Report">Reports&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
<?php
    }
    
    if ($_GET["menu"] == "monitors") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=monitors" 
           title="Monitors"><font color="#991e1e">Monitors</font>&nbsp;<img
           border="0" src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=monitors" 
           title="Monitors">Monitors&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
<?php
    }

    if ($_GET["menu"] == "policy") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=policy" 
           title="Policy"><font color="#991e1e">Policy</font>&nbsp;<img
           border="0" src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=policy" 
           title="Policy">Policy&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
<?php
    }
    
    if ($_GET["menu"] == "correlation") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=correlation" 
           title="Correlation"><font
           color="#991e1e">Correlation</font>&nbsp;<img border="0"
           src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=correlation" 
           title="Correlation">Correlation&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
<?php
    }

    if ($_GET["menu"] == "config") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=config" 
           title="Configuration"><font
           color="#991e1e">Configuration</font>&nbsp;<img border="0"
           src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=config" 
           title="Configuration">Configuration&nbsp;<img border="0"
           src="pixmaps/arrow2.gif"/></a>]
<?php
    }
?>


<?php
    if ($_GET["menu"] == "tools") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=tools" 
           title="Tools"><font color="#991e1e">Tools</font>&nbsp;<img
           border="0" src="pixmaps/arrow.gif"/></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=tools" 
           title="Tools">Tools&nbsp;<img border="0" src="pixmaps/arrow2.gif"/></a>]
<?php
    }
?>

        &nbsp;&nbsp;[<a href="session/login.php?action=logout"
           title="Logout" target="main"><font color="black">Logout</font></a>]



<!--
    submenu 
-->
    </th>
  </tr>
<?php
    if ($_GET["menu"] == "control_panel") {
?>
  <tr>
    <th>
        [<a href="control_panel/global_score.php" 
           title="OSSIM Control Panel - Metrics"
           target="main">Metrics</a>]
        [<a href="control_panel/alarm_console.php" 
           title="OSSIM Control Panel - Alarm Console"
           target="main">Alarms</a>]
        [<a href="<?php 
           echo $conf->get_conf("acid_link") . "/acid_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"; ?>" 
           title="(ACID)" 
           target="main">Alerts</a>]
        [<a href="vulnmeter/index.php" title="OSSIM vulnmeter" 
           target="main">Vulnerabilities</a>]
      </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "policy") {
?>
  <tr>
    <th>
        [<a href="policy/policy.php" title="policy management" 
           target="main">Policy</a>]
        [<a href="host/host.php" title="host management" 
           target="main">Hosts</a>]
        [<a href="net/net.php" title="network management" 
           target="main">Networks</a>]
        [<a href="sensor/sensor.php" title="sensor management" 
           target="main">Sensors</a>]
        [<a href="signature/signature.php" title="sensor management" 
           target="main">Signatures</a>]
        [<a href="port/port.php" title="port management" 
           target="main">Ports</a>]
      </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "report") {
?>
  <tr>
    <th>
        [<a href="report/report.php" title="host report" 
           target="main">Host Report</a>]
        [<a href="sec_report/sec_report.php?section=all" 
           title="security report" 
           target="main">Security Report</a>]
        [<a href="report/pdfreportform.php" title="PDF report" 
           target="main">PDF Report</a>]
        [<a href="control_panel/anomalies.php"
           title="(Anomalies)" 
           target="main">Anomalies</a>]
        [<a href="incidents/index.php" title="incidents" 
           target="main">Incidents</a>]
      </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "monitors") {

        $ntop_link = $conf->get_conf("ntop_link");
        $sensor = parse_url($ntop_link);
 
?>
  <tr>
    <th>
        [<a href="ntop/session.php?sensor=<?php echo $sensor["host"] ?>"
           title="(NTOP - Active TCP Sessions)" 
           target="main">Session</a>]
        [<a href="ntop/index.php?sensor=<?php echo $sensor["host"] ?>" 
           title="(NTOP)" 
           target="main">Network</a>]
        [<a href="<?php 
           echo $conf->get_conf("opennms_link"); ?>"
           title="(OpenNMS)" 
           target="main">Availability</a>]
        [<a href="sensor/sensor_plugins.php" title="OSSIM status" 
           target="main">Sensors</a>]
        [<a href="riskmeter/index.php" title="OSSIM riskmeter" 
           target="main">Riskmeter</a>]
      </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "correlation") {
?>
  <tr>
    <th>
        [<a href="directives/index.php" title="directive viewer" 
           target="main">Directives</a>]
        [<a href="conf/pluginref.php" title="correlation reference viewer" 
           target="main">Cross Correlation</a>]
        [<a href="control_panel/backlog.php" 
           title="OSSIM Control Panel - Backlog"
           target="main">Backlog</a>]
    </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "config") {
?>
  <tr>
    <th>
        [<a href="conf/main.php" title="main configuration" 
           target="main">Main</a>]
        [<a href="session/users.php" title="users" 
           target="main">Users</a>]
        [<a href="conf/plugin.php" title="Plugin Config" 
           target="main">Plugins</a>]
        [<a href="rrd_conf/rrd_conf.php" title="RRD Conf Management" 
           target="main">RRD Config</a>]
        [<a href="scan/hostscan.php" title="Host Scan configuration" 
           target="main">Host Scan</a>]
      </th>
    </tr>
<?php
    } elseif ($_GET["menu"] == "tools") {
?>
  <tr>
    <th>
        [<a href="scan/scan.php" title="host scanning" 
           target="main">Scan</a>]
        [<a href="editor/editor.php" title="rule viewer" 
           target="main">Rule Viewer</a>]
        [<a href="backup/index.php" title="Backup Manager" 
           target="main">Backup</a>]
      </th>
    </tr>
<?php
    }
?>

  </table>

</body>
</html>
