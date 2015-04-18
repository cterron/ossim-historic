<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Delete host scan configuration</h1>

<?php 
    if (!$host_ip = $_GET["host_ip"]) { 
        echo "<p align=\"center\">Wrong ip</p>";
        exit;
    }

    if (!$plugin_id = $_GET["plugin_id"]) {
        echo "<p align=\"center\">Plugin id missing</p>";
        exit;
    }

    if (!$_GET["confirm"]) {
        echo "<p>Are you sure?</p>";
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
    <p>Host scan configuration deleted</p>
    <p><a href="hostscan.php">Back</a></p>
    <?php exit(); ?>

</body>
</html>
