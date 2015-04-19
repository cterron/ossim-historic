<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMain");

require_once 'ossim_conf.inc';
$ossim_conf = $GLOBALS["CONF"];

require_once 'classes/Xajax.inc';
$xajax = new xajax();
$xajax->registerFunction("showConfig");
$xajax->registerFunction("showHelp");
$xajax->registerFunction("cleanHelp");
$xajax->registerFunction("updateMenu");
$xajax->registerFunction("processForm");

require_once 'classes/Security.inc';

$CONFIG = array (

    "Language" => array
    (
        "title" => gettext("Language"),
        "desc"  => gettext("Configure Internationalization"),
        "conf"  => array 
        (
            "language" => array
            (
                "type" => array
                (
                    "de_DE" => gettext("German"),
                    "en_GB" => gettext("English"),
                    "es_ES" => gettext("Spanish"),
                    "fr_FR" => gettext("French"),
                    "ja_JP" => gettext("Japanese"),
                    "pt_BR" => gettext("Brazilian Portuguese"),
                    "zh_CN" => gettext("Simplified Chinese"),
                    "zh_TW" => gettext("Traditional Chinese"),
                    "ru_RU.UTF-8" => gettext("Russian")
                ),
                "help" => gettext("")
            ),
            "locale_dir" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Ossim Server" => array 
    (
        "title" => gettext("Ossim Server"),
        "desc"  => gettext("Configure the server's listening address"),
        "conf"  => array 
        (
            "server_address"    => array (
                "type"  =>  "text",
                "help"  =>  gettext("")
            ),
            "server_port"       => array (
                "type"  =>  "text",
                "help"  =>  gettext("")
            )
        )
    ),

    "Ossim Framework" => array 
    (
        "title" => gettext("Ossim Framework"),
        "desc"  => gettext("PHP Configuration (graphs, acls, database api) and links to other applications"),
        "conf"  => array 
        (
            "ossim_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "adodb_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "jpgraph_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "fpdf_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "xajax_php_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "xajax_js_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "report_graph_type" => array
            (
                "type" => array
                (
                    "images"  => gettext("Images (php jpgraph)"),
                    "applets" => gettext("Applets (jfreechart)")
                ),
                "help" => gettext("")
            ),
            "use_svg_graphics" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes (Need SVG plugin)")
                ),
                "help" => gettext("")
            ),
            "use_resolv" => array
            (
                "type" => array 
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            ),
            "ntop_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nagios_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "use_ntop_rewrite" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            ),
             "use_munin" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            ),
            "munin_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            )

        )
    ),

    "Ossim FrameworkD" => array 
    (
        "title" => gettext("Ossim Framework Daemon"),
        "desc"  => gettext("Configure the frameworkd's listening address"),
        "conf"  => array 
        (
            "frameworkd_address" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "frameworkd_port" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "frameworkd_dir" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "frameworkd_controlpanelrrd" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_acidcache" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_optimizedb" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_listener" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_scheduler" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_soc" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_businessprocesses" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            ),
            "frameworkd_backup" => array
            (
                "type" => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
                "help" => gettext("")
            )
        )
    ),

    "Snort" => array 
    (
        "title" => gettext("Snort"),
        "desc"  => gettext("Snort database and path configuration"),
        "conf"  => array 
        (
            "snort_path" => array 
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_rules_path" => array 
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_type" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_base" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            ),
            "snort_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "snort_port" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Osvdb" => array 
    (
        "title" => gettext("Osvdb"),
        "desc"  => gettext("Open source vulnerability database configuration") ,
        "conf"  => array 
        (
            "osvdb_type" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "osvdb_base" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "osvdb_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "osvdb_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            ),
            "osvdb_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Metrics" => array
    (
        "title" => gettext("Metrics"),
        "desc"  => gettext("Configure metric settings"),
        "conf"  => array 
        (
            "recovery" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "threshold" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),
    
    "Executive Panel" => array
    (
        "title" => gettext("Executive Panel"),
        "desc"  => gettext("Configure panel settings"),
        "conf"  => array 
        (
            "panel_plugins_dir" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "panel_configs_dir" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),
    
    "ACLs" => array 
    (
        "title" => gettext("phpGACL configuration"),
        "desc"  => gettext("Access control list database configuration") ,
        "conf"  => array 
        (
            "phpgacl_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "phpgacl_type" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "phpgacl_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "phpgacl_base" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "phpgacl_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "phpgacl_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            )
        )
    ),

    "RRD" => array
    (
        "title" => gettext("RRD"),
        "desc"  => gettext("RRD Configuration (graphing)"),
        "conf"  => array 
        (
            "graph_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdtool_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdtool_lib_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "mrtg_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "mrtg_rrd_files_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_net" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_global" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_level" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_incidents" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "rrdpath_bps" => array
            (
                "type" => "text",
                "help" => gettext("business processes rrd directory")
            ),
            "rrdpath_ntop" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "font_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Backup" => array 
    (
        "title" => gettext("Backup"),
        "desc"  => gettext("Backup configuration: backup database, directory, interval"),
        "conf"  => array 
        (
            "backup_type" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_base" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            ),
            "backup_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_port" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_dir" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "backup_day" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Nessus" => array
    (
        "title" => gettext("Nessus"),
        "desc"  => gettext("Nessus client configuration"),
        "conf"  => array 
        (
            "nessus_user" => array
            (
                "type" => "text", 
                "help" => gettext("")
            ),
            "nessus_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            ),
            "nessus_host" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nessus_port" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nessus_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nessus_rpt_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nessusrc_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "nessus_distributed" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            ),
            "vulnerability_incident_threshold" => array
            (
                "type" => array
                (
                    "0" => "0",
                    "1" => "1",
                    "2" => "2",
                    "3" => "3",
                    "4" => "4",
                    "5" => "5",
                    "6" => "6",
                    "7" => "7",
                    "8" => "8",
                    "9" => "9",
                ),
                "help" => gettext("")
            )
        )
    ),

    "Acid/Base" => array
    (
        "title" => gettext("ACID/BASE"),
        "desc"  => gettext("Acid and/or Base configuration"),
        "conf"  => array 
        (
            "event_viewer" => array
            (
                "type" => array
                (
                    "acid" => gettext("Acid"),
                    "base" => gettext("Base")
                ),
                "help" => gettext("")
            ),
            "acid_link" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "acid_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "acid_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "acid_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            ),
            "ossim_web_user" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "ossim_web_pass" => array
            (
                "type" => "password",
                "help" => gettext("")
            )
        )
    ),

    "External Apps" => array
    (
        "title" => gettext("External applications"),
        "desc"  => gettext("Path to other applications"),
        "conf"  => array 
        (
            "nmap_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "p0f_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "arpwatch_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "mail_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "touch_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "wget_path" => array
            (
                "type" => "text",
                "help" => gettext("")
            ),
            "have_scanmap3d" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            )
        )
    ),

   "User Log" => array
    (
        "title" => gettext("User action logging"),
        "desc"  => gettext("User action logging"),
        "conf"  => array
        (
            "user_action_log" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            ),
            "log_syslog" => array
            (
                "type" => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
                "help" => gettext("")
            )
        )
    ),

    "Event Viewer" => array
    (
        "title" => gettext("Real time event viewer"),
        "desc" => gettext("Real time event viewer"),
        "conf" => array
        (
            "max_event_tmp" => array
            (
                "type" => "text",
                "help" => gettext("")
            )
        )
    ),

    "Login" => array
    (
        "title" => gettext("Login methods"),
        "desc" => gettext("Setup main login methods"),
        "conf" => array
        (
            "login_enforce_existing_user" => array
            (
                "type" => array
                (
                    "yes" => _("Yes"),
                    "no" => _("No")
                ),
                "help" => _("")
            ),
            "login_enable_ldap" => array
            (
                "type" => array
                (
                    "yes" => _("Yes"),
                    "no" => _("No")
                ),
                "help" => _("")
            ),
            "login_ldap_server" => array
            (
                "type" => "text",
                "help" => _("")
            ),
            "login_ldap_o" => array
            (
                "type" => "text",
                "help" => _("")
            ),
            "login_ldap_ou" => array
            (
                "type" => "text",
                "help" => _("")
            )
        )
    )
);


function valid_value ($key, $value) 
{
    $numeric_values = array (
            "server_port",
            "recovery",
            "threshold", 
            "use_resolv",
            "have_scanmap3d",
            "max_event_tmp"
        );

    if (in_array($key, $numeric_values)) {
        if (!is_numeric($value)) {
            return false;
        }
    }

    return true;
}


function showConfig($section)
{
    global $CONFIG, $ossim_conf;

    $form = "
        <form id=\"form_$section\">
        
        <table align='center' width='100%'>
          <tr><th colspan='3'>".$CONFIG[$section]['title']."</th></tr>
          <tr><td colspan='3'><b>".$CONFIG[$section]['desc']."</b></td></tr>";
    foreach ($CONFIG[$section]['conf'] as $conf => $value) {

        $conf_value = $ossim_conf->get_conf($conf, $debug=False);
        $type = $value["type"];
        $help = "<b>$conf</b>: " . $value["help"];

        $form .= "
          <tr>
            <td>$conf";
        if ($conf_value === NULL)
            $form .= " <i>(".gettext("Warning: variable is not set").")</i>";
        $form .= "
            </td>
            <td><img onMouseOver=\"xajax_showHelp('$help')\" 
                     onMouseOut=\"xajax_cleanHelp()\" 
                     src='../pixmaps/help.png' width='16'/></td>
            <td class='left'>";

            /* select */
            if (is_array($type)) {
                $form .= "<select name=\"$conf\">";
                foreach ($type as $option_value => $option_text)
                {
                    $form .= "<option ";
                    if ($conf_value == $option_value) $form .= " SELECTED ";
                    $form .= "value=\"$option_value\">$option_text</option>";
                }
                $form .= "</select>";
            }

            /* input */
            else {
                $form .= "<input name=\"$conf\"";
                if ($ossim_conf->is_in_file($conf)) {
                    $form .= " class=\"disabled\" ";
                    $form .= " DISABLED ";
                }
                $form .= "type=\"$type\" size=\"35\" value=\"$conf_value\" />";
            }
        $form .= "
            </td>
          </tr>";
    }
    $form .= "
          <tr>
            <td colspan='3'>
              <a onClick=\"xajax_processForm(xajax.getFormValues('form_$section'), '$section')\">Update</a>
            </td>
          </tr>
        </table>
        </form>";

    $objResponse = new xajaxResponse();
    $objResponse->addAssign("SectionTable","innerHTML", $form);
    return $objResponse;
}

function showHelp($help) {

    global $CONFIG;

    $xml = "
      <table class='noborder' width='100%'><tr><td class='help'>
          $help
      </td></tr></table>";

    $objResponse = new xajaxResponse();
    $objResponse->addAssign("HelpTable","innerHTML", $xml);
    return $objResponse;
}

function cleanHelp() {
    $objResponse = new xajaxResponse();
    $objResponse->addAssign("HelpTable","innerHTML", "");
    return $objResponse;
}

/* Markup the current section and clean others */
function updateMenu($section)
{
    global $CONFIG;
    $objResponse = new xajaxResponse();
 
    /* clean menu */
    foreach ($CONFIG as $key => $val)
    {
        if ($key != $section)
            $objResponse->addAssign("section_$key","innerHTML", "$key");
    }

    /* mark selected section */
    $currentSection = "<div class='selected'><b><a>$section</a></b></div>";
    $objResponse->addAssign("section_$section","innerHTML", $currentSection);

    /* clean help table */
    $objResponse->addAssign("HelpTable","innerHTML", "");

    return $objResponse;
}

function tabs()
{
    global $CONFIG;

    $html = "<table width='100%'>";
    foreach ($CONFIG as $key => $val)
    {
        $html .= "
          <tr><td>
              <a href='#' id=\"section_$key\" 
                 onClick=\"xajax_showConfig('$key');
                           xajax_updateMenu('$key');\">$key</a>
          </td></tr>";
    }
    $html .= "</table>";
    print $html;
}

function processForm($form_data, $section)
{

    require_once 'classes/Config.inc';
    $config = new Config();

    $help_msg = "Section <b>$section</b> updated<br/>";

    foreach ($form_data as $conf => $value) {
        if (valid_value($conf, $value)) {

            /* variable is not set, insert new one */
            if (!$config->has_conf($conf)) {
                $help_msg .= "<br/><b>$conf</b> ".
                gettext("has a new value: ") . " [".$value."]";
                $config->insert($conf, $value);
            }

            /* variable has changed its value, update it */
            elseif ($config->get_conf($conf) != $value) {
                $help_msg .= "
                    <br/><b>$conf</b> ".
                    gettext("has changed its value from") .
                    " [". $config->get_conf($conf)."] " .
                    gettext("to") . " [".$value."]";
                $config->update($conf, $value);
            }

            /* no variable changes */
            else {
            }
        } else {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $help_msg .= $error->get("NOT_NUMERIC", array($conf));
        }
    }

    return showHelp($help_msg);
}

$xajax->processRequests();

?>

<html>
<head>
  <title> <?php echo gettext("Main Configuration"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <?= $xajax->printJavascript('', XAJAX_JS); ?>
</head>
<body>

  <table align="center" class="noborder">
    <tr>
      <td valign='top' width='150'><?php tabs() ?></td>
      <td valign='top' width='600'>
        <div id="SectionTable"><img src="../pixmaps/logo5.jpg"/></div>
        <div id="HelpTable"></div>
      </td>
    </tr>
  </table>

</body>
</html>

