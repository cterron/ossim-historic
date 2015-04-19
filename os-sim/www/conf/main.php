<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationMain");

require_once ('ossim_conf.inc');
$ossim_conf = new ossim_conf();

$CONFIG = array (

    "snort" => array 
    (
        "title" => "Snort",
        "desc"  => "Snort database and paths configuration",
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

    "server" => array 
    (
        "title" => "Server",
        "desc"  => "Configure where the server is listening to",
        "conf"  => array 
        (
            "server_address"    => "text",
            "server_port"       => "text"
        )
    ),

    "metrics" => array
    (
        "title" => "Metrics",
        "desc"  => "Configure metric settings",
        "conf"  => array 
        (
            "recovery"  => "text",
            "threshold" => "text"
        )
    ),

    "php" => array 
    (
        "title" => "PHP",
        "desc"  => "PHP Configuration (graphs, acls, database api)",
        "conf"  => array 
        (
            "phpgacl_path"  => "text",
            "adodb_path"    => "text",
            "jpgraph_path"  => "text",
            "fpdf_path"	    => "text",
            "use_resolv"    => "select"
        )
    ),

    "rrd" => array
    (
        "title" => "RRD",
        "desc"  => "RRD Configuration (graphing)",
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
            "rrdpath_ntop"          => "text",
            "font_path"             => "text"
        )
    ),

    "links" => array
    (
        "title" => "Links",
        "desc"  => "Links to other applications",
        "conf"  => array 
        (
            "ntop_link"     => "text",
            "opennms_link"  => "text"
        )
    ),

    "backup" => array 
    (
        "title" => "Backup",
        "desc"  => "Backup configuration: backup database, directory, interval",
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
        "title" => "Nessus",
        "desc"  => "Nessus client configuration",
        "conf"  => array 
        (
            "nessus_user"       => "text", 
            "nessus_pass"       => "password",
            "nessus_host"       => "text",
            "nessus_port"       => "text",
            "nessus_path"       => "text",
            "nessus_rpt_path"   => "text"
        )
    ),

    "acid" => array
    (
        "title" => "ACID",
        "desc"  => "Acid cache configuration",
        "conf"  => array 
        (
            "acid_link"         => "text",
            "acid_user"         => "text",
            "acid_pass"         => "password",
            "acid_path"         => "text",
            "ossim_web_user"    => "text",
            "ossim_web_pass"    => "password"
        )
    ),

    "apps" => array
    (
        "title" => "External applications",
        "desc"  => "Path to other applications",
        "conf"  => array 
        (
            "nmap_path"     => "text",
            "p0f_path"      => "text",
            "arpwatch_path" => "text",
            "mail_path"     => "text",
            "touch_path"    => "text",
            "wget_path"     => "text"
        )
    )
);


function valid_value ($key, $value) 
{
    $numeric_values = array ("recovery", "threshold", "use_resolv");

    if (in_array($key, $numeric_values)) {
        if (!is_numeric($value)) {
            echo "Error: <b>".$_POST["conf"]."</b> must be numeric";
            return False;
        }
    }

    return True;
}


if ($_POST["update"]) 
{
    require_once ('classes/Config.inc');
    $config = new Config();

    if (valid_value($_POST["conf"], $_POST["value"])) {
        $value_update = mysql_real_escape_string($_POST["value"]);
        $config->update($_POST["conf"], $value_update);
    }
}

if ($_POST["reset"]) {
    require_once ('classes/Config.inc');
    $config = new Config();
    $config->reset();
}

?>

<html>
<head>
  <title> Main Configuration </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Main Configuration</h1>
  
  <table align="center">
<?php
    foreach ($CONFIG as $key => $val)
    {
        print "<tr><th colspan=\"3\">" . $val["title"] . "</th></tr>";
        print "<tr><td colspan=\"3\">" . $val["desc"] . "</td></tr>";
        foreach ($val["conf"] as $conf => $type)
        {
            $conf_value = $ossim_conf->get_conf($conf);
?>
        <form method="POST" action="<?php echo $_SERVER["PHP_SELF"] ?>" \>
        <tr>
          <td>
            <input type="hidden" name="conf" value="<?php echo $conf ?>" />
            <?php echo "<b>$conf</b>"; ?>
          </td>
          <td class="left">
<?php 
    if ($type == "select") { 
?>
            <select name="value">
              <option <?php if (!$conf_value) echo " selected " ?>
                value="0">No</option>
              <option <?php if ($conf_value) echo " selected " ?> 
                value="1">Yes</option>
            </select>
<?php 
    } else { 
?>
            <input type="<?php echo $type ?>" 
                size="30" name="value" 
                value="<?php echo $conf_value ?>" />
<?php 
    } 
?>
          </td>
          <td><input 
<?php if ($ossim_conf->is_in_file($conf)) echo " class=\"disabled\" "; ?>
                style="" type="submit" name="update" value="update" 
<?php if ($ossim_conf->is_in_file($conf)) echo " DISABLED "; ?>
          />
          </td>
        </tr>
        </form>
<?php
        }
        print "<tr><td colspan=\"3\"><br/></td></tr>";
    }
?>

    <tr>
    <form method="POST" action="<?php echo $_SERVER["PHP_SELF"] ?>" \>
      <td colspan="3">
        <input type="submit" name="reset" value="Reset default values" />
      </td>
    </form>
    </tr>



<!--
    <tr><td colspan="3"><br/></td></tr>
    <tr><th colspan="3">Reload server structures</th></tr>
    <tr><td colspan="3"></td></tr>
    <tr>
      <td colspan="3">
        <a href="reload.php">RELOAD ALL</a>
      </td>
    </tr>
    <tr>
      <td colspan="3">
        <a href="reload.php?what=policies">Reload policies</a>
      </td>
    </tr>
    <tr>
      <td colspan="3">
        <a href="reload.php?what=hosts">Reload hosts</a>
      </td>
    </tr>
    <tr>
      <td colspan="3">
        <a href="reload.php?what=nets">Reload nets</a>
      </td>
    </tr>
    <tr>
      <td colspan="3">
        <a href="reload.php?what=sensors">Reload sensors</a>
      </td>
    </tr>
    <tr>
      <td colspan="3">
        <a href="reload.php?what=directives">Reload directives</a>
      </td>
    </tr>
    <tr><td colspan="3"><hr noshade></td></tr>
    <tr><th colspan="3"><a href="../setup/ossim_acl.php">Reload ACLS</a></th></tr>
    <tr><td colspan="3"></td></tr>
-->


  </table>
    
  
</body>
</html>
