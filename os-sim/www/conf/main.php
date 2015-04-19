<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMain");
require_once 'ossim_conf.inc';
require_once 'classes/Security.inc';

$ossim_conf = $GLOBALS["CONF"];

$CONFIG = array (

    "generic" => array
    (
        "title" => gettext("Language"),
        "desc"  => gettext("Configure Internationalization"),
        "conf"  => array 
        (
            "language"          => array
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
            "locale_dir"        => "text"
        )
    ),

    "server" => array 
    (
        "title" => gettext("Server"),
        "desc"  => gettext("Configure the server's listening address"),
        "conf"  => array 
        (
            "server_address"    => "text",
            "server_port"       => "text"
        )
    ),

    "frameworkd" => array 
    (
        "title" => gettext("FrameworkD"),
        "desc"  => gettext("Configure the frameworkd's listening address"),
        "conf"  => array 
        (
            "frameworkd_address"        => "text",
            "frameworkd_port"           => "text",
            "frameworkd_dir"            => "text",

            "frameworkd_controlpanelrrd"    => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_acidcache"          => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_listener"           => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_scheduler"          => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_soc"                => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_businessprocesses"  => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
            "frameworkd_backup"             => array
                (
                    "0" => gettext("Disabled"),
                    "1" => gettext("Enabled")
                ),
        )
    ),



    "snort" => array 
    (
        "title" => gettext("Snort"),
        "desc"  => gettext("Snort database and path configuration") ,
        "conf"  => array 
        (
            "snort_path"            => "text",
            "snort_rules_path"      => "text",
            "snort_type"            => "text",
            "snort_base"            => "text",
            "snort_user"            => "text",
            "snort_pass"            => "password",
            "snort_host"            => "text",
            "snort_port"            => "text"
        )
    ),


    "osvdb" => array 
    (
        "title" => gettext("Osvdb"),
        "desc"  => gettext("Open source vulnerability database configuration") ,
        "conf"  => array 
        (
            "osvdb_type"            => "text",
            "osvdb_base"            => "text",
            "osvdb_user"            => "text",
            "osvdb_pass"            => "password",
            "osvdb_host"            => "text",
        )
    ),

    "metrics" => array
    (
        "title" => gettext("Metrics"),
        "desc"  => gettext("Configure metric settings"),
        "conf"  => array 
        (
            "recovery"  => "text",
            "threshold" => "text"
        )
    ),
    
    "panel" => array
    (
        "title" => gettext("Executive Panel"),
        "desc"  => gettext("Configure panel settings"),
        "conf"  => array 
        (
            "panel_plugins_dir"  => "text",
            "panel_configs_dir"  => "text"
        )
    ),
    
    "phpgacl" => array 
    (
        "title" => gettext("phpGACL"),
        "desc"  => gettext("Access control list database configuration") ,
        "conf"  => array 
        (
            "phpgacl_path"  => "text",
            "phpgacl_type"  => "text",
            "phpgacl_host"  => "text",
            "phpgacl_base"  => "text",
            "phpgacl_user"  => "text",
            "phpgacl_pass"  => "password",
        )
    ),

    "php" => array 
    (
        "title" => gettext("PHP"),
        "desc"  => gettext("PHP Configuration (graphs, acls, database api)"),
        "conf"  => array 
        (
            "adodb_path"        => "text",
            "jpgraph_path"      => "text",
            "fpdf_path"	        => "text",
            "xajax_php_path"    => "text",
            "xajax_js_path"     => "text",
            "report_graph_type" => array
                (
                    "images"  => gettext("Images (php jpgraph)"),
                    "applets" => gettext("Applets (jfreechart)")
                ),
            "use_svg_graphics"  => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes (Need SVG plugin)")
                ),
            "use_resolv"        => array 
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                )
        )
    ),

    "rrd" => array
    (
        "title" => gettext("RRD"),
        "desc"  => gettext("RRD Configuration (graphing)"),
        "conf"  => array 
        (
            "graph_link"            => "text",
            "rrdtool_path"          => "text",
            "rrdtool_lib_path"      => "text",
            "mrtg_path"             => "text",
            "mrtg_rrd_files_path"   => "text",
            "rrdpath_host"          => "text",
            "rrdpath_net"           => "text",
            "rrdpath_global"        => "text",
            "rrdpath_level"         => "text",
            "rrdpath_incidents"     => "text",
            "rrdpath_ntop"          => "text",
            "font_path"             => "text"
        )
    ),

    "links" => array
    (
        "title" => gettext("Links"),
        "desc"  => gettext("Links to other applications"),
        "conf"  => array 
        (
            "ossim_link"    => "text",
            "ntop_link"     => "text",
            "nagios_link"   => "text"
        )
    ),

    "backup" => array 
    (
        "title" => gettext("Backup"),
        "desc"  => gettext("Backup configuration: backup database, directory, interval"),
        "conf"  => array 
        (
            "backup_type"   => "text",
            "backup_base"   => "text",
            "backup_user"   => "text",
            "backup_pass"   => "password",
            "backup_host"   => "text",
            "backup_port"   => "text",
            "backup_dir"    => "text",
            "backup_day"    => "text"
        )
    ),

    "nessus" => array
    (
        "title" => gettext("Nessus"),
        "desc"  => gettext("Nessus client configuration"),
        "conf"  => array 
        (
            "nessus_user"           => "text", 
            "nessus_pass"           => "password",
            "nessus_host"           => "text",
            "nessus_port"           => "text",
            "nessus_path"           => "text",
            "nessus_rpt_path"       => "text",
            "nessusrc_path"         => "text",
            "nessus_distributed"    => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                ),
            "vulnerability_incident_threshold" => array
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
        )
    ),

    "acid" => array
    (
        "title" => gettext("ACID"),
        "desc"  => gettext("Acid configuration"),
        "conf"  => array 
        (
            "event_viewer" => array
                (
                    "acid" => gettext("Acid"),
                    "base" => gettext("Base")
                ),
            "acid_link"         => "text",
            "acid_path"         => "text",
            "acid_user"         => "text",
            "acid_pass"         => "password",
            "ossim_web_user"    => "text",
            "ossim_web_pass"    => "password"
        )
    ),

    "apps" => array
    (
        "title" => gettext("External applications"),
        "desc"  => gettext("Path to other applications"),
        "conf"  => array 
        (
            "nmap_path"         => "text",
            "p0f_path"          => "text",
            "arpwatch_path"     => "text",
            "mail_path"         => "text",
            "touch_path"        => "text",
            "wget_path"         => "text",
            "have_scanmap3d"    => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                )
        )
    ),

   "userlog" => array
   (
       "title" => gettext("User action logging"),
       "desc"  => gettext("User action logging"),
       "conf"  => array
       (
        "user_action_log"       => array
                (
                    "0" => gettext("No"),
                    "1" => gettext("Yes")
                )     
       )
   ),

   "event_viewer" => array
   (
        "title" => gettext("Real time event viewer"),
        "desc" => gettext("Real time event viewer"),
        "conf" => array
        (
         "max_event_tmp" => "text"
        )
   ),

   "login_methods" => array
   (
        "title" => gettext("OSSIM login methods"),
        "desc" => gettext("Setup main login methods"),
        "conf" => array
        (
         "login_enforce_existing_user" => array
             (
                 "yes" => _("Yes"),
                 "no" => _("No")
             ),
         "login_enable_ldap" => array
             (
                 "yes" => _("Yes"),
                 "no" => _("No")
             )
        )
   ),

   "login_options" => array
   (
        "title" => gettext("OSSIM login options"),
        "desc" => gettext("Configure various login options"),
        "conf" => array
        (
         "login_ldap_server" => "text",
         "login_ldap_o" => "text",
         "login_ldap_ou" => "text"
        )
   )

);


function valid_value ($key, $value) 
{
    $numeric_values = array (
            "recovery",
            "threshold", 
            "use_resolv",
            "have_scanmap3d",
            "max_event_tmp"
        );

    if (in_array($key, $numeric_values)) {
        if (!is_numeric($value)) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("NOT_NUMERIC", array($key));
        }
    }

    return true;
}

function submit ()
{
?>
    <!-- submit -->
    <tr>
      <td colspan="2">
        <input type="submit" name="update" 
            value=" <?php echo gettext("Update configuration"); ?> " />
        <input type="submit" name="reset" 
            value=" <?php echo gettext("Reset default values"); ?> " />
      </td>
    </tr>
    <!-- end sumbit -->
<?php
}


if (POST('update')) 
{
    require_once 'classes/Config.inc';
    $config = new Config();

    for ($i = 0; $i < POST('nconfs'); $i++)
    {
        if (valid_value(POST("conf_$i"), POST("value_$i"))) {
            $config->update(POST("conf_$i"), POST("value_$i"));
        }
    }
    header("Location: ".$_SERVER['PHP_SELF']); exit;
}

if (REQUEST("reset")) {

    if (!(GET('confirm'))) {
?>
        <p align="center">
          <b><?php echo gettext("Are you sure ?") ?></b>
          <br/>
          <a href="?reset=1&confirm=1"><?php echo gettext("Yes") ?></a>&nbsp;|&nbsp;
          <a href="main.php"><?php echo gettext("No") ?></a>
        </p>
<?php
        exit;
    }

    require_once 'classes/Config.inc';
    $config = new Config();
    $config->reset();
    header("Location: ".$_SERVER['PHP_SELF']); exit;
}

?>

<html>
<head>
  <title> <?php echo gettext("Main Configuration"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Main Configuration"); ?> </h1>
  
  <form method="POST" action="<?php echo $_SERVER["PHP_SELF"] ?>" />
  <table align="center">


<?php
    submit();

    $count = 0;
    foreach ($CONFIG as $key => $val)
    {
        print "<tr><th colspan=\"2\">" . $val["title"] . "</th></tr>";
        print "<tr><td colspan=\"2\">" . $val["desc"] . "</td></tr>";

        foreach ($val["conf"] as $conf => $type)
        {
            $conf_value = $ossim_conf->get_conf($conf);
?>
    <tr>

      <input type="hidden" name="conf_<?php echo $count ?>"
             value="<?php echo $conf ?>" />

      <td><b><?php echo $conf ?></b></td>
      <td class="left">
<?php
            $input = "";

            /* select */
            if (is_array($type)) {
                $input .= "<select name=\"value_$count\">";
                foreach ($type as $option_value => $option_text)
                {
                    $input .= "<option ";
                    if ($conf_value == $option_value) $input .= " SELECTED ";
                    $input .= "value=\"$option_value\">$option_text</option>";
                }
                $input .= "</select>";
            }

            /* input */
            else {
                $input .= "<input ";
                if ($ossim_conf->is_in_file($conf)) {
                    $input .= " class=\"disabled\" ";
                    $input .= " DISABLED ";
                }
                $input .= "type=\"$type\" size=\"30\" 
                    name=\"value_$count\" value=\"$conf_value\" />";
            }

            echo $input;
?>
      </td>
    </tr>
<?php
            $count += 1;
        }
    }
    submit();
?>

    <input type="hidden" name="nconfs" value="<?php echo $count ?>" />

  </table>
  </form>

</body>
</html>

