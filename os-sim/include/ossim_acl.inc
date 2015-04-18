<?php

define ("ACL_DEFAULT_USER_SECTION",     "users");
define ("ACL_DEFAULT_USER_GROUP",       "users");

define ("ACL_DEFAULT_OSSIM_ADMIN",      "admin");

define ("ACL_DEFAULT_DOMAIN_SECTION",   "DomainAccess");
define ("ACL_DEFAULT_DOMAIN_ALL",       "All");
define ("ACL_DEFAULT_DOMAIN_LOGIN",     "Login");
define ("ACL_DEFAULT_DOMAIN_NETS",      "Nets");


$ACL_MAIN_MENU = array (

    "MenuControlPanel"  => array
        (
            "ControlPanelMetrics" => array
                (
                    "name"          => "Control Panel -> Metrics",
                    "default_perm"  => True
                ),
            "ControlPanelAlarms" => array
                (
                    "name"          => "Control Panel -> Alarms",
                    "default_perm"  => True
                ),
            "ControlPanelAlerts" => array
                (
                    "name"          => "Control Panel -> Alerts",
                    "default_perm"  => True
                ),
            "ControlPanelVulnerabilities" => array
                (
                    "name"          => "Control Panel -> Vulnerabilities",
                    "default_perm"  => True
                ),
            "ControlPanelAnomalies" => array
                (
                    "name"          => "Control Panel -> Anomalies",
                    "default_perm"  => True
                )
        ),

    "MenuPolicy"    => array
        (
            "PolicyPolicy" => array
                (
                    "name"          => "Policy -> Policy",
                    "default_perm"  => False
                ),
            "PolicyHosts" => array
                (
                    "name"          => "Policy -> Hosts",
                    "default_perm"  => False
                ),
            "PolicyNetworks" => array
                (
                    "name"          => "Policy -> Networks",
                    "default_perm"  => False
                ),
            "PolicySensors" => array
                (
                    "name"          => "Policy -> Sensors",
                    "default_perm"  => False
                ),
            "PolicySignatures" => array
                (
                    "name"          => "Policy -> PolicySignatures",
                    "default_perm"  => False
                ),
            "PolicyPriorityReliability" => array
                (
                    "name"          => "Policy -> Priority & Reliability",
                    "default_perm"  => False
                ),
            "PolicyPorts" => array
                (
                    "name"          => "Policy -> Ports",
                    "default_perm"  => False
                )
        ),
    "MenuReports"       => array
        (
            "ReportsHostReport" => array
                (
                    "name"          => "Reports -> HostReport",
                    "default_perm"  => True
                ),
            "ReportsSecurityReport" => array
                (
                    "name"          => "Reports -> SecurityReport",
                    "default_perm"  => True
                ),
            "ReportsIncidents" => array
                (
                    "name"          => "Reports -> Incidents",
                    "default_perm"  => True
                )
        ),
    "MenuMonitors"      => array
        (
            "MonitorsSession" => array
                (
                    "name"          => "Monitors -> Session",
                    "default_perm"  => True
                ),
            "MonitorsNetwork" => array
                (
                    "name"          => "Monitors -> Network",
                    "default_perm"  => True
                ),
            "MonitorsAvailability" => array
                (
                    "name"          => "Monitors -> Availability",
                    "default_perm"  => True
                ),
            "MonitorsRiskmeter" => array
                (
                    "name"          => "Monitors -> Riskmeter",
                    "default_perm"  => True
                )
        ),
    "MenuConfiguration" => array
        (
            "ConfigurationMain" => array
                (
                    "name"          => "Configuration -> Main",
                    "default_perm"  => False
                ),
            "ConfigurationUsers" => array
                (
                    "name"          => "Configuration -> Users",
                    "default_perm"  => False
                ),
            "ConfigurationDirectives" => array
                (
                    "name"          => "Configuration -> Directives",
                    "default_perm"  => False
                ),
            "ConfigurationCorrelation" => array
                (
                    "name"          => "Configuration -> Correlation",
                    "default_perm"  => False
                ),
            "ConfigurationRRDConfig" => array
                (
                    "name"          => "Configuration -> RRD Config",
                    "default_perm"  => False
                ),
            "ConfigurationHostScan" => array
                (
                    "name"          => "Configuration -> Host Scan",
                    "default_perm"  => False
                ),
            "ConfigurationRiskmeter" => array
                (
                    "name"          => "Configuration -> Riskmeter",
                    "default_perm"  => False
                )
        ),
    "MenuTools"         => array
        (
            "ToolsScan" => array
                (
                    "name"          => "Tools -> Scan",
                    "default_perm"  => True
                ),
            "ToolsBacklog" => array
                (
                    "name"          => "Tools -> Backlog",
                    "default_perm"  => True
                ),
            "ToolsRuleViewer" => array
                (
                    "name"          => "Tools -> RuleViewer",
                    "default_perm"  => True
                ),
            "ToolsBackup" => array
                (
                    "name"          => "Tools -> Backup",
                    "default_perm"  => True
                )
        )
);


?>
