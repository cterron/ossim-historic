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
require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
/* Generate reporting server url */
switch ($conf->get_conf("bi_type", FALSE)) {
    case "jasperserver":
    default:
        if ($conf->get_conf("bi_host", FALSE) == "localhost") {
            $bi_host = $_SERVER["SERVER_ADDR"];
        } else {
            $bi_host = $conf->get_conf("bi_host", FALSE);
        }
        if (!strstr($bi_host, "http")) {
            $reporting_link = "http://";
        }
        $bi_link = $conf->get_conf("bi_link", FALSE);
        $bi_link = str_replace("USER", $conf->get_conf("bi_user", FALSE) , $bi_link);
        $bi_link = str_replace("PASSWORD", $conf->get_conf("bi_pass", FALSE) , $bi_link);
        $reporting_link.= $bi_host;
        $reporting_link.= ":";
        $reporting_link.= $conf->get_conf("bi_port", FALSE);
        $reporting_link.= $bi_link;
}
/* Dashboards */
if (Session::menu_perms("MenuControlPanel", "ControlPanelExecutive")) $menu["Dashboards"][] = array(
    "name" => gettext("Main") ,
    "id" => "Executive Panel",
    "url" => "panel/"
);
if (Session::menu_perms("MenuControlPanel", "ControlPanelMetrics")) {
    $menu["Dashboards"][] = array(
        "name" => gettext("Aggregated Risk") ,
        "id" => "Metrics",
        "url" => "control_panel/global_score.php"
    );
    $hmenu["Metrics"][] = array(
        "name" => gettext("Last day") ,
        "id" => "Metrics",
        "url" => "control_panel/global_score.php?range=day"
    );
    $hmenu["Metrics"][] = array(
        "name" => gettext("Last week") ,
        "id" => "MetricsW",
        "url" => "control_panel/global_score.php?range=week"
    );
    $hmenu["Metrics"][] = array(
        "name" => gettext("Last month") ,
        "id" => "MetricsM",
        "url" => "control_panel/global_score.php?range=month"
    );
    $hmenu["Metrics"][] = array(
        "name" => gettext("Last year") ,
        "id" => "MetricsY",
        "url" => "control_panel/global_score.php?range=year"
    );
}
if (Session::menu_perms("MenuControlPanel", "BusinessProcesses")) if (!file_exists($version_file)) {
    $menu["Dashboards"][] = array(
        "name" => gettext("Business Processes") ,
        "id" => "Business Processes",
        "url" => "business_processes/index.php"
    );
}
if (Session::menu_perms("MenuControlPanel", "BusinessProcesses")) {
    $menu["Dashboards"][] = array(
      "name" => gettext("Risk Maps") ,
      "id" => "Risk Maps",
      "url" => "risk_maps/riskmaps.php?view=1"
    );
    $hmenu["Risk Maps"][] = array(
      "name" => gettext("View"),
      "id" => "Risk Maps",
      "target" => "main",
      "url" => "risk_maps/riskmaps.php?view=1"
    );
    $hmenu["Risk Maps"][] = array(
      "name" => gettext("Edit"),
      "id" => "Edit Risk Maps",
      "target" => "main",
      "url" => "risk_maps/riskmaps.php"
    );
}
if (Session::menu_perms("MenuControlPanel", "Help")) $menu["Dashboards"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:control_panel','Dashboard Help');"
);
/* Incidents */
if (Session::menu_perms("MenuIncidents", "IncidentsIncidents")) {
    $menu["Incidents"][] = array(
        "name" => gettext("Tickets") ,
        "id" => "Tickets",
        "url" => "incidents/index.php?status=$status"
    );
    $hmenu["Tickets"][] = array(
        "name" => gettext("Tickets") ,
        "id" => "Tickets",
        "url" => "incidents/index.php?status=$status"
    );
}
if (Session::menu_perms("MenuIncidents", "IncidentsTypes")) $hmenu["Tickets"][] = array(
    "name" => gettext("Types") ,
    "id" => "Types",
    "url" => "incidents/incidenttype.php"
);
if (Session::menu_perms("MenuIncidents", "IncidentsTags")) $hmenu["Tickets"][] = array(
    "name" => gettext("Tags") ,
    "id" => "Tags",
    "url" => "incidents/incidenttag.php"
);
if (Session::menu_perms("MenuIncidents", "IncidentsReport")) $hmenu["Tickets"][] = array(
    "name" => gettext("Report") ,
    "id" => "Report",
    "url" => "report/incidentreport.php"
);
if (Session::menu_perms("MenuConfiguration", "ConfigurationEmailTemplate")) {
    $hmenu["Tickets"][] = array(
        "name" => gettext("Incidents Email Template") ,
        "id" => "Incidents Email Template",
        "url" => "conf/emailtemplate.php"
    );
}
if (Session::menu_perms("MenuControlPanel", "ControlPanelAlarms")) {
    $menu["Incidents"][] = array(
        "name" => gettext("Alarms") ,
        "id" => "Alarms",
        "url" => "control_panel/alarm_console.php?&hide_closed=1"
        //"url" => "control_panel/alarm_group_console.php"
        
    );
    $hmenu["Alarms"][] = array(
        "name" => gettext("Alarms") ,
        "id" => "Alarms",
        "url" => "control_panel/alarm_console.php?hide_closed=1"
    );
    if (Session::menu_perms("MenuReports", "ReportsAlarmReport")) $hmenu["Alarms"][] = array(
        "name" => gettext("Report") ,
        "id" => "Report",
        "url" => "report/sec_report.php?section=all&type=alarm"
    );
}
if (Session::menu_perms("MenuTools", "Repository")) if (file_exists($version_file)) {
    $menu["Incidents"][] = array(
        "name" => gettext("Knowledge DB") ,
        "id" => "Repository",
        "url" => "repository/index.php"
    );
    $hmenu["Repository"][] = array(
        "name" => gettext("Knowledge DB") ,
        "id" => "Repository",
        "url" => "repository/index.php"
    );
}
if (Session::menu_perms("MenuIncidents", "Help")) $menu["Incidents"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:incidents','Help');"
);
/* Events */
if (Session::menu_perms("MenuEvents", "EventsForensics")) {
    $menu["Events"][] = array(
        "name" => gettext("SIM Events") ,
        "id" => "Forensics",
        //"url" => $conf->get_conf("acid_link", FALSE) . "/" . $conf->get_conf("event_viewer", FALSE) . "_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
        "url" => "forensics/base_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
    );
    $hmenu["Forensics"][] = array(
        "name" => gettext("SIM Events") ,
        "id" => "Forensics",
        //"url" => $conf->get_conf("acid_link", FALSE) . "/" . $conf->get_conf("event_viewer", FALSE) . "_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
        "url" => "forensics/base_qry_main.php?&num_result_rows=-1&submit=Query+DB&current_view=-1&sort_order=time_d"
    );
}
if (Session::menu_perms("MenuEvents", "EventsRT")) {
/*    $menu["Events"][] = array(
    "name" => gettext("Real Time") ,
    "id" => "RT Events",
    "url" => "control_panel/event_panel.php"
    );
*/
    $hmenu["Forensics"][] = array(
        "name" => gettext("Real Time") ,
        "id" => "RT Events",
        "url" => "control_panel/event_panel.php"
    );
}
if (Session::menu_perms("MenuEvents", "EventsViewer")) $hmenu["Forensics"][] = array(
    "name" => gettext("Customized View") ,
    "id" => "Events Viewer",
    "url" => "event_viewer/index.php"
);
if (Session::menu_perms("MenuEvents", "EventsForensics")) $hmenu["Forensics"][] = array(
    "name" => gettext("Statistics") ,
    "id" => "Events Stats",
    "url" => "report/event_stats.php"
);
if (is_dir("/var/ossim/")) {
    // Only show SEM menu if SEM is available
    if (Session::menu_perms("MenuControlPanel", "ControlPanelSEM")) {
        $menu["Events"][] = array(
            "name" => gettext("SEM Events") ,
            "id" => "SEM",
            "url" => "sem/index.php"
        );
        $hmenu["SEM"][] = array(
            "name" => gettext("SEM Events") ,
            "id" => "SEM",
            "url" => "sem/index.php"
        );
    }
}
if (Session::menu_perms("MenuEvents", "EventsVulnerabilities")) {
    $menu["Events"][] = array(
        "name" => gettext("Vulnerabilities") ,
        "id" => "Vulnerabilities",
        "url" => "vulnmeter/index.php"
    );
    $hmenu["Vulnerabilities"][] = array(
        "name" => gettext("Vulnerabilities") ,
        "id" => "Vulnerabilities",
        "url" => "vulnmeter/index.php"
    );
}
if (Session::menu_perms("MenuEvents", "EventsAnomalies")) {
    $menu["Events"][] = array(
        "name" => gettext("Anomalies") ,
        "id" => "Anomalies",
        "url" => "control_panel/anomalies.php"
    );
    $hmenu["Anomalies"][] = array(
        "name" => gettext("Anomalies") ,
        "id" => "Anomalies",
        "url" => "control_panel/anomalies.php"
    );
}
if (Session::menu_perms("MenuEvents", "Help Events")) $menu["Events"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:events','Event Help');"
);
/* Monitors */
if (Session::menu_perms("MenuMonitors", "MonitorsNetwork")) {
    $menu["Monitors"][] = array(
        "name" => gettext("Usage & Profiles") ,
        "id" => "Network",
        "url" => "ntop/index.php?sensor=" . $sensor_ntop["host"]
    );
    $hmenu["Network"][] = array(
        "name" => gettext("Global") ,
        "id" => "Network",
        "target" => "main",
        "url" => "ntop/index.php?sensor=" . $sensor_ntop["host"]
    );
    $hmenu["Network"][] = array(
        "name" => gettext("Services") ,
        "id" => "Services",
        "target" => "main",
        "url" => "ntop/index.php?opc=services&sensor=" . $sensor_ntop["host"]
    );
    $hmenu["Network"][] = array(
        "name" => gettext("Throughput") ,
        "id" => "Throughput",
        "target" => "main",
        "url" => "ntop/index.php?opc=throughput&sensor=" . $sensor_ntop["host"]
    );
    $hmenu["Network"][] = array(
        "name" => gettext("Matrix") ,
        "id" => "Matrix",
        "target" => "main",
        "url" => "ntop/index.php?opc=matrix&sensor=" . $sensor_ntop["host"]
    );
}
/*
if (Session::menu_perms("MenuMonitors", "MonitorsSession")) $menu["Usage & Profiles"][] = array(
"name" => gettext("Session") ,
"id" => "Session",
"url" => "ntop/session.php?sensor=" . $sensor_ntop["host"]
);*/
if (Session::menu_perms("MenuMonitors", "MonitorsAvailability")) {
    $menu["Monitors"][] = array(
        "name" => gettext("Availability") ,
        "id" => "Availability",
        "url" => "nagios/index.php?sensor=" . $sensor_nagios["host"]
    );
    $hmenu["Availability"][] = array(
        "name" => gettext("Monitoring") ,
        "id" => "Availability",
        "target" => "main",
        "url" => "nagios/index.php?sensor=" . $sensor_nagios["host"]
    );
    $hmenu["Availability"][] = array(
        "name" => gettext("Reporting") ,
        "id" => "Reporting",
        "target" => "main",
        "url" => "nagios/index.php?opc=reporting&sensor=" . $sensor_nagios["host"]
    );
}
/*
if (Session::menu_perms("MenuMonitors", "MonitorsVServers") && $conf->get_conf("ovcp_link", FALSE) != "") $menu["Usage & Profiles"][] = array(
"name" => gettext("Virtual Servers") ,
"id" => "Virtual Servers",
"url" => "$ovcp_link"
);*/
if (Session::menu_perms("MenuMonitors", "MonitorsSensors")) {
    $menu["Monitors"][] = array(
        "name" => gettext("System") ,
        "id" => "Sensors",
        "url" => "sensor/sensor_plugins.php"
    );
    $hmenu["Sensors"][] = array(
        "name" => gettext("System") ,
        "id" => "Sensors",
        "url" => "sensor/sensor_plugins.php"
    );
}
/*
if (Session::menu_perms("MenuMonitors", "MonitorsRiskmeter")) $menu["Usage & Profiles"][] = array(
"name" => gettext("Riskmeter") ,
"id" => "Riskmeter",
"url" => "riskmeter/index.php"
);*/
if (Session::menu_perms("MenuMonitors", "Help")) $menu["Monitors"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:monitors','Help');"
);
/* Reports */
if (Session::menu_perms("MenuReports", "ReportsHostReport")) {
    $menu["Reports"][] = array(
        "name" => gettext("Host Report") ,
        "id" => "Host Report",
        "url" => "report/report.php"
    );
    $hmenu["Host Report"][] = array(
        "name" => gettext("Host Report") ,
        "id" => "Host Report",
        "url" => "report/report.php"
    );
}
if (Session::menu_perms("MenuReports", "ReportsSecurityReport")) {
    $menu["Reports"][] = array(
        "name" => gettext("Security Report") ,
        "id" => "Security Report",
        "url" => "report/sec_report.php?section=all"
    );
    $hmenu["Security Report"][] = array(
        "name" => gettext("Security Report") ,
        "id" => "Security Report",
        "url" => "report/sec_report.php?section=all"
    );
}
if (Session::menu_perms("MenuReports", "ReportsGLPI") && $conf->get_conf("glpi_link", FALSE) != "") $menu["Reports"][] = array(
    "name" => gettext("GLPI") ,
    "id" => "GLPI",
    "url" => "$glpi_link"
);
if (Session::menu_perms("MenuReports", "ReportsOCSInventory") && $conf->get_conf("ocs_link", FALSE) != "") $menu["Reports"][] = array(
    "name" => gettext("OCS Inventory") ,
    "id" => "OCS Inventory",
    "url" => "$ocs_link"
);
if (Session::menu_perms("MenuReports", "ReportsPDFReport")) {
    $menu["Reports"][] = array(
        "name" => gettext("PDF Report") ,
        "id" => "PDF Report",
        "url" => "report/pdfreportform.php"
        //      "url" => "ocs/index.php"
        
    );
    $hmenu["PDF Report"][] = array(
        "name" => gettext("PDF Report") ,
        "id" => "PDF Report",
        "url" => "report/pdfreportform.php"
        //      "url" => "ocs/index.php"
        
    );
}
if (Session::menu_perms("MenuTools", "ToolsUserLogViewer")) {
    $menu["Reports"][] = array(
        "name" => gettext("User log") ,
        "id" => "User log",
        "url" => "userlog/user_action_log.php"
    );
    $hmenu["User log"][] = array(
        "name" => gettext("User log") ,
        "id" => "User log",
        "url" => "userlog/user_action_log.php"
    );
}
if (Session::menu_perms("MenuReports", "ReportsReportServer")) $menu["Reports"][] = array(
    "name" => gettext("Report Manager") ,
    "id" => "Reporting Server",
    "url" => "$reporting_link"
);
if (Session::menu_perms("MenuReports", "Help")) $menu["Reports"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:reports','Help');"
);
/* Policy */
if (Session::menu_perms("MenuPolicy", "PolicyPolicy")) {
    $menu["Policy"][] = array(
        "name" => gettext("Policy") ,
        "id" => "Policy",
        "url" => "policy/policy.php"
    );
    $hmenu["Policy"][] = array(
        "name" => gettext("Policy") ,
        "id" => "Policy",
        "url" => "policy/policy.php"
    );
}
if (Session::menu_perms("MenuPolicy", "PolicyPolicy")) $hmenu["Policy"][] = array(
    "name" => gettext("Policy groups") ,
    "id" => "Policy Group",
    "url" => "policy/policygroup.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyHosts")) $hmenu["Policy"][] = array(
    "name" => gettext("Hosts") ,
    "id" => "Hosts",
    "url" => "host/host.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyHosts")) $hmenu["Policy"][] = array(
    "name" => gettext("Host groups") ,
    "id" => "Host groups",
    "url" => "host/hostgroup.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyNetworks")) $hmenu["Policy"][] = array(
    "name" => gettext("Networks") ,
    "id" => "Networks",
    "url" => "net/net.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyNetworks")) $hmenu["Policy"][] = array(
    "name" => gettext("Network groups") ,
    "id" => "Network groups",
    "url" => "net/netgroup.php"
);
if (Session::menu_perms("MenuPolicy", "PolicySensors")) $hmenu["Policy"][] = array(
    "name" => gettext("Sensors") ,
    "id" => "Sensors",
    "url" => "sensor/sensor.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyServers")) $hmenu["Policy"][] = array(
    "name" => gettext("Servers") ,
    "id" => "Servers",
    "url" => "server/server.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyPorts")) $hmenu["Policy"][] = array(
    "name" => gettext("Ports") ,
    "id" => "Ports",
    "url" => "port/port.php"
);
if (Session::menu_perms("MenuPolicy", "PolicyActions")) {
    $menu["Policy"][] = array(
        "name" => gettext("Actions") ,
        "id" => "Actions",
        "url" => "action/action.php"
    );
    $hmenu["Actions"][] = array(
        "name" => gettext("Actions") ,
        "id" => "Actions",
        "url" => "action/action.php"
    );
}
/*if (Session::menu_perms("MenuPolicy", "PolicyResponses"))
$menu["Policy"][] = array (
"name" => gettext("Responses"),
"id" => "Responses",
"url" => "response/response.php"
);*/
if (Session::menu_perms("MenuPolicy", "PolicyPluginGroups")) $hmenu["Policy"][] = array(
    "name" => gettext("Plugin Groups") ,
    "id" => "Plugin Groups",
    "url" => "policy/plugingroups.php"
);
if (Session::menu_perms("MenuReports", "Help")) $menu["Policy"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:policy','Help');"
);
/* Correlation */
if (Session::menu_perms("MenuCorrelation", "CorrelationDirectives")) {
    $menu["Correlation"][] = array(
        "name" => gettext("Directives") ,
        "id" => "Directives",
        "url" => "directive_editor/index.php"
    );
    $hmenu["Directives"][] = array(
        "name" => gettext("Directives") ,
        "id" => "Directives",
        "url" => "directive_editor/index.php"
    );
}
if (Session::menu_perms("MenuCorrelation", "CorrelationCrossCorrelation")) {
    $menu["Correlation"][] = array(
        "name" => gettext("Cross Correlation") ,
        "id" => "Cross Correlation",
        "url" => "conf/pluginref.php"
    );
    $hmenu["Cross Correlation"][] = array(
        "name" => gettext("Plugin reference") ,
        "id" => "Cross Correlation",
        "url" => "conf/pluginref.php"
    );
}
if (Session::menu_perms("MenuCorrelation", "CorrelationBacklog")) {
    $menu["Correlation"][] = array(
        "name" => gettext("Backlog") ,
        "id" => "Backlog",
        "url" => "control_panel/backlog.php"
    );
    $hmenu["Backlog"][] = array(
        "name" => gettext("Backlog") ,
        "id" => "Backlog",
        "url" => "control_panel/backlog.php"
    );
}
if (Session::menu_perms("MenuReports", "Help")) $menu["Correlation"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:correlation','Help');"
);
/* Configuration */
if (Session::menu_perms("MenuConfiguration", "ConfigurationMain")) if (file_exists($version_file)) {
    $menu["Configuration"][] = array(
        "name" => gettext("Main") ,
        "id" => "Main",
        "url" => "conf/main.php"
    );
    $hmenu["Main"][] = array(
        "name" => gettext("Simple") ,
        "id" => "Main",
        "url" => "conf/main.php"
    );
    $hmenu["Main"][] = array(
        "name" => gettext("Advanced") ,
        "id" => "Advanced",
        "url" => "conf/main.php?adv=1"
    );
} else {
    $menu["Configuration"][] = array(
        "name" => gettext("Main") ,
        "id" => "Main",
        "url" => "conf/index.php"
    );
    $hmenu["Main"][] = array(
        "name" => gettext("Simple") ,
        "id" => "Main",
        "url" => "conf/index.php"
    );
    $hmenu["Main"][] = array(
        "name" => gettext("Advanced") ,
        "id" => "Advanced",
        "url" => "conf/index.php?adv=1"
    );
}
if (Session::menu_perms("MenuConfiguration", "ConfigurationUsers")) {
    $menu["Configuration"][] = array(
        "name" => gettext("Users") ,
        "id" => "Users",
        "url" => "session/users.php"
    );
    $hmenu["Users"][] = array(
        "name" => gettext("Configuration") ,
        "id" => "Users",
        "url" => "session/users.php"
    );
}
if (Session::menu_perms("MenuConfiguration", "ConfigurationUserActionLog")) $hmenu["Users"][] = array(
    "name" => gettext("User action logs") ,
    "id" => "User action logs",
    "url" => "conf/userlog.php"
);
if (Session::menu_perms("MenuConfiguration", "ConfigurationPlugins")) {
    $menu["Configuration"][] = array(
        "name" => gettext("Plugins") ,
        "id" => "Plugins",
        "url" => "conf/plugin.php"
    );
    $hmenu["Plugins"][] = array(
        "name" => gettext("Priority and Reliability configuration") ,
        "id" => "Plugins",
        "url" => "conf/plugin.php"
    );
}
/*
if (Session::menu_perms("MenuConfiguration", "ConfigurationRRDConfig")) {
$menu["Configuration"][] = array(
"name" => gettext("RRD Config") ,
"id" => "RRD Config",
"url" => "rrd_conf/rrd_conf.php"
);
$hmenu["RRD Config"][] = array(
"name" => gettext("RRD Config") ,
"id" => "RRD Config",
"url" => "rrd_conf/rrd_conf.php"
);
}
if (Session::menu_perms("MenuConfiguration", "ConfigurationHostScan")) $menu["Configuration"][] = array(
"name" => gettext("Host Scan") ,
"id" => "Host Scan",
"url" => "scan/hostscan.php"
);
if (Session::menu_perms("MenuConfiguration", "ConfigurationEmailTemplate")) {
$menu["Configuration"][] = array(
"name" => gettext("Incidents Email Template") ,
"id" => "Incidents Email Template",
"url" => "conf/emailtemplate.php"
);
$hmenu["Incidents Email Template"][] = array(
"name" => gettext("Incidents Email Template") ,
"id" => "Incidents Email Template",
"url" => "conf/emailtemplate.php"
);
}*/
if (Session::menu_perms("MenuConfiguration", "ConfigurationUpgrade")) {
    $menu["Configuration"][] = array(
        "name" => gettext("Software Upgrade") ,
        "id" => "Upgrade",
        "url" => "upgrade/"
    );
    $hmenu["Upgrade"][] = array(
        "name" => gettext("Software Upgrade") ,
        "id" => "Upgrade",
        "url" => "upgrade/"
    );
    $hmenu["Upgrade"][] = array(
        "name" => gettext("Update Notification") ,
        "id" => "Updates",
        "url" => "updates/index.php"
    );
}
/*
if (Session::menu_perms("MenuConfiguration", "ConfigurationMaps")) $menu["Configuration"][] = array(
"name" => gettext("Maps") ,
"id" => "Maps",
"url" => "maps/"
);*/
if (Session::menu_perms("MenuConfiguration", "Help")) $menu["Configuration"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:configuration','Help');"
);
/* Tools */
if (Session::menu_perms("MenuTools", "ToolsBackup")) {
    $menu["Tools"][] = array(
        "name" => gettext("Backup") ,
        "id" => "Backup",
        "url" => "backup/index.php"
    );
    $hmenu["Backup"][] = array(
        "name" => gettext("Backup") ,
        "id" => "Backup",
        "url" => "backup/index.php"
    );
}
if (Session::menu_perms("MenuTools", "ToolsDownloads")) if (file_exists($version_file)) {
    $menu["Tools"][] = array(
        "name" => gettext("Downloads") ,
        "id" => "Downloads",
        "url" => "downloads/index.php"
    );
    $hmenu["Downloads"][] = array(
        "name" => gettext("Tool Downloads") ,
        "id" => "Downloads",
        "url" => "downloads/index.php"
    );
}
if (Session::menu_perms("MenuTools", "ToolsScan")) {
    $menu["Tools"][] = array(
        "name" => gettext("Net Scan") ,
        "id" => "Net Scan",
        "url" => "netscan/index.php"
    );
    $hmenu["Net Scan"][] = array(
        "name" => gettext("Net Scan") ,
        "id" => "Net Scan",
        "url" => "netscan/index.php"
    );
}
/*
if (Session::menu_perms("MenuTools", "ToolsRuleViewer")) $menu["Tools"][] = array(
"name" => gettext("Rule Viewer") ,
"id" => "Rule Viewer",
"url" => "editor/editor.php"
);*/
// Right now only the installer uses this so it makes no sense in mainstream
/*
if (Session::menu_perms("MenuTools", "Updates")) $menu["Tools"][] = array(
"name" => gettext("Update Information") ,
"id" => "Updates",
"url" => "updates/index.php"
);*/
if (Session::menu_perms("MenuTools", "Help")) $menu["Tools"][] = array(
    "name" => gettext("Help") ,
    "id" => "Help",
    "url" => "javascript:new_wind('http://ossim.net/dokuwiki/doku.php?id=user_manual:tools','Help');"
);
/* Logout */
$menu["Logout"] = "session/login.php?action=logout"; // Plain url if no array entry

?>
