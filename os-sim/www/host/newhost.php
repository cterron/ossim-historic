<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("New Host"); ?> </h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["hostname"] || !$_POST["ip"] ||
         !$_POST["threshold_c"] || !$_POST["threshold_a"] || 
         // !$_POST["persistence"] || 
         !$_POST["nsens"])) 
    {
?>

  <p align="center">
  <?php echo gettext("Please, complete all the fields"); ?> </p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $hostname    = mysql_escape_string($_POST["hostname"]);
    $ip          = mysql_escape_string($_POST["ip"]);
    $asset       = mysql_escape_string($_POST["asset"]);
    $threshold_c = mysql_escape_string($_POST["threshold_c"]);
    $threshold_a = mysql_escape_string($_POST["threshold_a"]);
    $rrd_profile = mysql_escape_string($_POST["rrd_profile"]);
    $alert       = mysql_escape_string($_POST["alert"]);
    $persistence = mysql_escape_string($_POST["persistence"]);
    $nat         = mysql_escape_string($_POST["nat"]);
    $descr       = mysql_escape_string($_POST["descr"]);
    $os          = mysql_escape_string($_POST["os"]);
    $mac         = mysql_escape_string($_POST["mac"]);
    $num_sens    = 0;

    for ($i = 1; $i <= mysql_escape_string($_POST["nsens"]); $i++) {
        $name = "mboxs" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $num_sens ++;
            $sensors[] = mysql_escape_string($_POST[$name]);
        }
    }
    if($num_sens == 0){
    ?>
      <p align="center">
      <?php echo gettext("Please, complete all the fields"); ?> </p>
<?php
exit();

    }

    require_once 'ossim_db.inc';
    require_once 'ossim_conf.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    $conf = new ossim_conf();

    if ($hostname == "__scan") {

        $nmap = $conf->get_conf("nmap_path");
        $ips = shell_exec("$nmap -sP -v -n $ip");
        $ip_list = explode("\n", $ips);
        
        foreach ($ip_list as $line) {
        
            $pattern = "/Host ([^\s]+)/";
            if (preg_match_all($pattern, $line, $regs)) {
                $ip = $regs[1][0];
            }

            if (! Host::in_host($conn, $ip)) {
            
                $pattern = "/appears to be up/";
                if (preg_match_all($pattern, $line, $regs)) {
                    
                    Host::insert ($conn, $ip, gethostbyaddr($ip), 
                                  $asset, $threshold_c, $threshold_a, 
                                  $rrd_profile, $alert, $persistence, $nat, 
                                  $sensors, $descr);
                    if($_POST["nessus"]) {
                        Host_scan::insert ($conn, $ip, 3001, 0);
                    }
                }
            }
        }
    } else {
        if (! Host::in_host($conn, $ip)) {
            Host::insert ($conn, $ip, $hostname, $asset, 
                          $threshold_c, $threshold_a, $rrd_profile, $alert, 
                          $persistence, $nat, $sensors, $descr, $os, $mac);
            if($_POST["nessus"]) {
                Host_scan::insert ($conn, $ip, 3001, 0);
            }
        }
    }


    $db->close($conn);
}
?>
    <p> <?php echo gettext("Host succesfully inserted"); ?> </p>
    <p><a href="host.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

