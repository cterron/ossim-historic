<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Update Scan</h1>

<?php
        
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $data_dir = $conf->get_conf("data_dir");

    function start_shell ($cmd) {
     exec('nohup "'.$cmd.'" > /dev/null &');
    }
    
    start_shell("$data_dir/scripts/do_nessus.pl");

?>
<center> * Nessus scan started, depending on number of hosts to be scanned this may take a while.</center>
<center> * Notice: httpd user needs write permission to the vulnmeter dir and subdirs.</center>
<center><a href="index.php"> Back </a></center>
 
</body>
</html>

