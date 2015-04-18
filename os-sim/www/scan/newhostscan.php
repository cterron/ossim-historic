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
                                                                                
  <h1>New host scan configuration</h1>

<?php
    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["host_ip"] || !$_POST["plugin_id"])) {
        echo "<p align=\"center\">Please, complete all the fields</p>";
        exit();
    }
    
    /* check OK, insert into BD */
    elseif($_POST["insert"]) {

        $host_ip    = mysql_escape_string($_POST["host_ip"]);
        $plugin_id  = mysql_escape_string($_POST["plugin_id"]);
    }

    require_once ('ossim_db.inc');
    require_once ('classes/Host_scan.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    Host_scan::insert($conn, $host_ip, $plugin_id, 0);

    $db->close($conn);
?>
    <p>Host scan configuration succesfully inserted</p>
    <p><a href="hostscan.php">Back</a></p>

</body>
</html>


