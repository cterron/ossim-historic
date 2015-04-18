<?php

# TODO: Read data from include/ossim_acl.inc!

include ('/var/www/phpgacl/gacl.class.php');
include ('/var/www/phpgacl/gacl_api.class.php');

$gacl_api = new gacl_api();

$gacl_api->add_object_section ('Domain Access','DomainAccess', 1, 0, 'ACO');
$gacl_api->add_object ('DomainAccess', 'All', 'All', 1, 0, 'ACO');
$gacl_api->add_object ('DomainAccess', 'Login', 'Login', 2, 0, 'ACO');
$gacl_api->add_object ('DomainAccess', 'Nets', 'Nets', 3, 0, 'ACO');

$gacl_api->add_object_section ('Menu Control Panel', 'MenuControlPanel', 10, 0, 'ACO');
$gacl_api->add_object ('MenuControlPanel', 'Control Panel Metrics', 'ControlPanelMetrics', 1, 0, 'ACO');
$gacl_api->add_object ('MenuControlPanel', 'Control Panel Alarms', 'ControlPanelAlarms', 2, 0, 'ACO');
$gacl_api->add_object ('MenuControlPanel', 'Control Panel Alerts', 'ControlPanelAlerts', 3, 0, 'ACO');
$gacl_api->add_object ('MenuControlPanel', 'Control Panel Vulnerabilities', 'ControlPanelVulnerabilities', 4, 0, 'ACO');
$gacl_api->add_object ('MenuControlPanel', 'Control Panel Anomalies', 'ControlPanelAnomalies', 5, 0, 'ACO');

$gacl_api->add_object_section ('Menu Policy', 'MenuPolicy', 11, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Policy', 'PolicyPolicy', 1, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Hosts', 'PolicyHosts', 2, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Networks', 'PolicyNetworks', 3, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Sensors', 'PolicySensors', 4, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Signatures', 'PolicySignatures', 5, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Priority and Reliability', 'PolicyPriorityReliability', 6, 0, 'ACO');
$gacl_api->add_object ('MenuPolicy', 'Policy Ports', 'PolicyPorts', 7, 0, 'ACO');

$gacl_api->add_object_section ('Menu Reports', 'MenuReports', 12, 0, 'ACO');
$gacl_api->add_object ('MenuReports', 'Reports Host Report', 'ReportsHostReport', 1, 0, 'ACO');
$gacl_api->add_object ('MenuReports', 'Reports Security Report', 'ReportsSecurityReport', 2, 0, 'ACO');

$gacl_api->add_object_section ('Menu Monitors', 'MenuMonitors', 13, 0, 'ACO');
$gacl_api->add_object ('MenuMonitors', 'Monitors Session', 'MonitorsSession', 1, 0, 'ACO');
$gacl_api->add_object ('MenuMonitors', 'Monitors Network', 'MonitorsNetwork', 2, 0, 'ACO');
$gacl_api->add_object ('MenuMonitors', 'Monitors Availability', 'MonitorsAvailability', 3, 0, 'ACO');
$gacl_api->add_object ('MenuMonitors', 'Monitors Riskmeter', 'MonitorsRiskmeter', 4, 0, 'ACO');

$gacl_api->add_object_section ('Menu Configuration', 'MenuConfiguration', 14, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Main', 'ConfigurationMain', 1, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Users', 'ConfigurationUsers', 2, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Directives', 'ConfigurationDirectives', 3, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Correlation', 'ConfigurationCorrelation', 4, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration RRD Config', 'ConfigurationRRDConfig', 5, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Host Scan', 'ConfigurationHostScan', 6, 0, 'ACO');
$gacl_api->add_object ('MenuConfiguration', 'Configuration Riskmeter', 'ConfigurationRiskmeter', 7, 0, 'ACO');

$gacl_api->add_object_section ('MenuTools', 'MenuTools', 15, 0, 'ACO');
$gacl_api->add_object ('MenuTools', 'Tools Scan', 'ToolsScan', 1, 0, 'ACO');
$gacl_api->add_object ('MenuTools', 'Tools Backlog', 'ToolsBacklog', 2, 0, 'ACO');
$gacl_api->add_object ('MenuTools', 'Tools Rule Viewer', 'ToolsRuleViewer', 3, 0, 'ACO');

/* Groups */
$groups['ossim'] = $gacl_api->add_group('ossim', 'OSSIM', 0, 'ARO');
$groups['users'] = $gacl_api->add_group('users', 'Users', $groups['ossim'], 'ARO');

/* Default User */
$gacl_api->add_object_section ('Users','users', 1, 0, 'ARO');
$gacl_api->add_object ('users','Admin','admin',1,0,'ARO');

$gacl_api->add_acl(array('DomainAccess'=>array('All')), array('users'=>array('admin')));

/*
$gacl_api->add_acl(
    array (

        "MenuControlPanel"  => array (
            "ControlPanelMetrics",
            "ControlPanelAlarms",
            "ControlPanelAlerts",
            "ControlPanelVulnerabilities",
            "ControlPanelAnomalies"
        ),

        "MenuPolicy"    => array (
            "PolicyPolicy",
            "PolicyHosts",
            "PolicyNetworks",
            "PolicySensors",
            "PolicySignatures",
            "PolicyPriorityReliability",
            "PolicyPorts"
        ),

        "MenuReports"   => array (
            "ReportsHostReport",
            "ReportsSecurityReport"
        ),

        "MenuMonitors"      => array (
            "MonitorsSession",
            "MonitorsNetwork",
            "MonitorsAvailability",
            "MonitorsRiskmeter"
        ),

        "MenuConfiguration" => array (
            "ConfigurationMain",
            "ConfigurationUsers",
            "ConfigurationDirectives",
            "ConfigurationCorrelation",
            "ConfigurationRRDConfig",
            "ConfigurationHostScan",
            "ConfigurationRiskmeter"
        ),

        "MenuTools"         => array (
            "ToolsScan",
            "ToolsBacklog",
            "ToolsRuleViewer"
        )
    ),

    array ('users' => array('admin'))
);
*/

?>
