<?php

define ("ACL_DEFAULT_USER_SECTION",     "users");
define ("ACL_DEFAULT_USER_GROUP",       "users");

define ("ACL_DEFAULT_OSSIM_ADMIN",      "admin");

define ("ACL_DEFAULT_DOMAIN_SECTION",   "DomainAccess");
define ("ACL_DEFAULT_DOMAIN_ALL",       "All");
define ("ACL_DEFAULT_DOMAIN_LOGIN",     "Login");
define ("ACL_DEFAULT_DOMAIN_NETS",      "Nets");
define ("ACL_DEFAULT_DOMAIN_SENSORS",   "Sensors");

/* get phpgacl config */
require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];

/* include default $gacl_options */
//require_once ("$phpgacl_path/admin/gacl_admin.inc.php");

$ACL_OPTIONS = array (
/*
    'debug'                     => $gacl_options['debug'],
    'items_per_page'            => $gacl_options['items_per_page'],
    'max_select_box_items'      => $gacl_options['max_select_box_items'],
    'max_search_return_items'   => $gacl_options['max_search_return_items'],
*/    
    'db_type'                   => $conf->get_conf("phpgacl_type"),
    'db_host'                   => $conf->get_conf("phpgacl_host"),
    'db_user'                   => $conf->get_conf("phpgacl_user"),
    'db_password'               => $conf->get_conf("phpgacl_pass"),
    'db_name'                   => $conf->get_conf("phpgacl_base"),
/*    
    'db_table_prefix'           => $gacl_options['db_table_prefix'],
    'caching'                   => $gacl_options['caching'],
    'force_cache_expire'        => $gacl_options['force_cache_expire'],
    'cache_dir'                 => $gacl_options['cache_dir'],
    'cache_expire_time'         => $gacl_options['cache_expire_time']
*/
);

$phpgacl_path = $conf->get_conf("phpgacl_path");
/*
 * phpgacl on "new gacl()" does a lot of heavy operations like:
 * parsing files, including code and opens a new persistant
 * connection to the BD. So better make $gacl global
 */
$phpgacl = $GLOBALS["CONF"]->get_conf("phpgacl_path");
require_once $phpgacl . DIRECTORY_SEPARATOR . 'gacl.class.php';
require_once $phpgacl . DIRECTORY_SEPARATOR . 'gacl_api.class.php';

$GLOBALS['ACL'] = new gacl($ACL_OPTIONS);

$ACL_MAIN_MENU = array (
    "MainMenu" => array 
        (
            "Index" => array
                (
                    "name"          => "Main Menu & Top frame",
                    "default_perm"  => true
                )
        ),

    "MenuControlPanel"  => array
        (
            "ControlPanelExecutive" => array
                (
                    "name"          => "Control Panel -> Executive Panel",
                    "default_perm"  => true
                ),
            "ControlPanelExecutiveEdit" => array
                (
                    "name"          => "Control Panel -> Executive Panel Edit",
                    "default_perm"  => false
                ),
            "BusinessProcesses" => array
                (
                    "name"          => "Control Panel -> Business Processes",
                    "default_perm"  => true 
                ),
            "BusinessProcessesEdit" => array
                (
                    "name"          => "Control Panel -> Business Processes Edit",
                    "default_perm"  => false
                ),
            "ControlPanelMetrics" => array
                (
                    "name"          => "Control Panel -> Metrics",
                    "default_perm"  => true
                ),
            "ControlPanelAlarms" => array
                (
                    "name"          => "Control Panel -> Alarms",
                    "default_perm"  => true
                ),
            "ControlPanelEvents" => array
                (
                    "name"          => "Control Panel -> Events",
                    "default_perm"  => true
                ),
            "ControlPanelVulnerabilities" => array
                (
                    "name"          => "Control Panel -> Vulnerabilities",
                    "default_perm"  => true
                ),
            "ControlPanelAnomalies" => array
                (
                    "name"          => "Control Panel -> Anomalies",
                    "default_perm"  => true
                ),
            "ControlPanelHids" => array
                (
                    "name"          => "Control Panel -> Hids",
                    "default_perm"  => true
                )
        ),

    "MenuPolicy"    => array
        (
            "PolicyPolicy" => array
                (
                    "name"          => "Policy -> Policy",
                    "default_perm"  => false
                ),
            "PolicyHosts" => array
                (
                    "name"          => "Policy -> Hosts",
                    "default_perm"  => false
                ),
            "PolicyNetworks" => array
                (
                    "name"          => "Policy -> Networks",
                    "default_perm"  => false
                ),
            "PolicySensors" => array
                (
                    "name"          => "Policy -> Sensors",
                    "default_perm"  => false
                ),
            "PolicyServers" => array
                (
                    "name"          => "Policy -> PolicyServers",
                    "default_perm"  => false
                ),
            "PolicyPorts" => array
                (
                    "name"          => "Policy -> Ports",
                    "default_perm"  => false
                ),
            "PolicyActions" => array
                (
                    "name"          => "Policy -> Actions",
                    "default_perm"  => false
                ),
            "PolicyResponses" => array
                (
                    "name"          => "Policy -> Responses",
                    "default_perm"  => false
                ),
            "PolicyPluginGroups" => array
                (
                    "name"          => "Policy -> Plugin Groups",
                    "default_perm"  => false
                )
        ),
    "MenuReports"       => array
        (
            "ReportsHostReport" => array
                (
                    "name"          => "Reports -> HostReport",
                    "default_perm"  => true
                ),
            "ReportsAlarmReport" => array
                (
                    "name"          => "Reports -> AlarmReport",
                    "default_perm"  => true
                ),
            "ReportsSecurityReport" => array
                (
                    "name"          => "Reports -> SecurityReport",
                    "default_perm"  => true
                ),
            "ReportsPDFReport" => array
                (
                    "name"          => "Reports -> PDFReport",
                    "default_perm"  => true
                ),
        ),
    "MenuIncidents"     => array
        (
            "IncidentsIncidents" => array
                (
                    "name"          => "Incidents -> Incidents",
                    "default_perm"  => true
                ),
            "IncidentsTypes" => array
                (
                    "name"          => "Incidents -> Types",
                    "default_perm"  => false
                ),
            "IncidentsReport" => array
                (
                    "name"          => "Incidents -> Report",
                    "default_perm"  => true
                ),
            "IncidentsTags"  => array
                (
                    "name"          => "Incidents -> Tags",
                    "default_perm"  => false
                ),
            "Osvdb"  => array
                (
                    "name"          => "Incidents -> Osvdb",
                    "default_perm"  => true
                ),
        ),
    "MenuMonitors"      => array
        (
            "MonitorsSession" => array
                (
                    "name"          => "Monitors -> Session",
                    "default_perm"  => true
                ),
            "MonitorsNetwork" => array
                (
                    "name"          => "Monitors -> Network",
                    "default_perm"  => true
                ),
            "MonitorsAvailability" => array
                (
                    "name"          => "Monitors -> Availability",
                    "default_perm"  => true
                ),
            "MonitorsSensors" => array
                (
                    "name"          => "Monitors -> Sensors",
                    "default_perm"  => false 
                ),
            "MonitorsRiskmeter" => array
                (
                    "name"          => "Monitors -> Riskmeter",
                    "default_perm"  => true
                )
        ),
    "MenuCorrelation" => array
        (
            "CorrelationDirectives" => array
                (
                    "name"          => "Correlation -> Directives",
                    "default_perm"  => false
                ),
            "CorrelationCrossCorrelation" => array
                (
                    "name"          => "Correlation -> Cross Correlation",
                    "default_perm"  => false
                ),
            "CorrelationBacklog" => array
                (
                    "name"          => "Correlation -> Backlog",
                    "default_perm"  => true
                )
        ),
    "MenuConfiguration" => array
        (
            "ConfigurationMain" => array
                (
                    "name"          => "Configuration -> Main",
                    "default_perm"  => false
                ),
            "ConfigurationUsers" => array
                (
                    "name"          => "Configuration -> Users",
                    "default_perm"  => false
                ),
            "ConfigurationPlugins" => array
                (
                    "name"          => "Configuration -> Plugins",
                    "default_perm"  => false
                ),
            "ConfigurationRRDConfig" => array
                (
                    "name"          => "Configuration -> RRD Config",
                    "default_perm"  => false
                ),
            "ConfigurationHostScan" => array
                (
                    "name"          => "Configuration -> Host Scan",
                    "default_perm"  => false
                ),
            "ConfigurationUserActionLog" => array
                (
                    "name"          => "Configuration -> User action log",
                    "default_perm"  => false
                ),
            "ConfigurationEmailTemplate" => array
                (
                    "name"          => "Configuration -> Incidents Email Template",
                    "default_perm"  => false
                ),
            "ConfigurationUpgrade" => array
                (
                    "name"          => "Configuration -> Upgrades",
                    "default_perm"  => false
                ),
            "ConfigurationMaps" => array
                (
                    "name"         => "Configuration -> Maps",
                    "default_perm" => true
                ),
        ),
    "MenuTools"         => array
        (
            "ToolsScan" => array
                (
                    "name"          => "Tools -> Scan",
                    "default_perm"  => true
                ),
            "ToolsRuleViewer" => array
                (
                    "name"          => "Tools -> RuleViewer",
                    "default_perm"  => false
                ),
            "ToolsBackup" => array
                (
                    "name"          => "Tools -> Backup",
                    "default_perm"  => false
                ),
            "ToolsUserLog" => array
                (
                    "name"          => "Tools -> User log",
                    "default_perm"  => true
                )
        )
);


?>
