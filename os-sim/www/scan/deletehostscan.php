<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete host scan configuration"); ?> </h1>

<?php 
    require_once 'classes/Security.inc';
    
    $host_ip = GET('host_ip');
    $plugin_id = GET('plugin_id'); 
    
    ossim_valid($host_ip, OSS_IP_ADDR, 'illegal:'._("IP Address"));
    ossim_valid($plugin_id, OSS_DIGIT, 'illegal:'._("Plugin id"));

    if (ossim_error()) {
        die(ossim_error());
    }
    
    if (GET('confirm')) {
        echo "<p> " . gettext("Are you sure") . " ?</p>";
        echo "<p><a href=\"" . $_SERVER["PHP_SELF"].
        "?host_ip=$host_ip&plugin_id=$plugin_id&confirm=yes\">Yes</a>" . 
        "&nbsp;&nbsp;&nbsp;<a href=\"hostscan.php\">No</a></p>";
        exit();
    }

    
    require_once 'ossim_db.inc';
    require_once 'classes/Host_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Host_scan::delete($conn, $host_ip, $plugin_id);
    $db->close($conn);
?>
    <p> <?php echo gettext("Host scan configuration deleted"); ?> </p>
    <p><a href="hostscan.php"> <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>
