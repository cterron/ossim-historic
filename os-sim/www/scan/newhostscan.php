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
                                                                                
  <h1> <?php echo gettext("New host scan configuration"); ?> </h1>

<?php
    require_once ('classes/Security.inc');

    $insert = POST('insert');
    $host_ip = POST('host_ip'); 
    $plugin_id = POST('plugin_id');

    ossim_valid($insert, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("insert"));
    ossim_valid($host_ip, OSS_IP_ADDR, 'illegal:'._("host IP"));
    ossim_valid($plugin_id, OSS_DIGIT, 'illegal:'._("Plugin Id"));

    if (ossim_error()) {
        die(ossim_error());
    }

    require_once ('ossim_db.inc');
    require_once ('classes/Host_scan.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    Host_scan::insert($conn, $host_ip, $plugin_id, 0);

    $db->close($conn);
?>
    <p> <?php echo gettext("Host scan configuration succesfully inserted"); ?> </p>
    <p><a href="hostscan.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>


