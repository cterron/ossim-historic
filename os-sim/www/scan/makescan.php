<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<?php

    /* TODO: define internal net */
    $DEFAULT_TARGET = "192.168.0.0/24";
    
    require_once 'classes/Security.inc';
    
    $scan = POST('scan');

    if (POST('scan')) {

        require_once('classes/Scan.inc');
        require_once 'ossim_db.inc';
        require_once 'ossim_conf.inc';

        $db = new ossim_db();
        $conn = $db->connect();

        $conf = $GLOBALS["CONF"];
    
        $target = POST('target');
        
        ossim_valid($confirm, OSS_ALPHA, OSS_PUNC , 'illegal:'._("Scan target"));

        if (ossim_error()) { die(ossim_error()); }
        
        $target = escapeshellcmd($target);
       	$nmap = $conf->get_conf("nmap_path");
        $ips = shell_exec("$nmap -sP -v -n $target");
        $ip_list = explode("\n", $ips);
?>

        <a href="scan.php"> <?php echo gettext("Back"); ?> </a><br><br>

<?php
        
        foreach ($ip_list as $line) {
        
            $pattern = "/Host ([^\s]+)/";
            if (preg_match_all($pattern, $line, $regs)) {
                $ip = $regs[1][0];
            }
            
            $pattern = "/appears to be up/";
            if (preg_match_all($pattern, $line, $regs)) {
                echo "Host $ip appears to be up<br/>";
                if (Scan::in_scan($conn, $ip)) {
                    if (!Scan::is_active($conn, $ip)) {
                        Scan::active($conn, $ip);
                    }
                } else {
                    Scan::insert($conn, $ip, 1);
                }
            }
            
            $pattern = "/appears to be down/";
            if (preg_match_all($pattern, $line, $regs)) {
                echo "Host $ip appears to be down<br/>";
                if (Scan::in_scan($conn, $ip)) {
                    if (Scan::is_active($conn, $ip)) {
                        Scan::disactive($conn, $ip);
                    }
                } else {
                    Scan::insert($conn, $ip, 0);
                }
            }
        }

        $db->close($conn);
        exit;
    }
?>

    <table>
    <form method="post" action="<?php echo $_SERVER["PHP_SELF"]?>">
      <tr>
        <td>
          Range: 
          <input type="text" name="target" 
            value="<?php echo $DEFAULT_TARGET ?>">
        </td>
        <td>
            <input type="submit" name="scan" value="Ping Scan">
        </td>
      </tr>
    </form>

    <!-- use host insert form -->
    <form method="post" action="../host/newhostform.php">
      <tr>
        <td>
          Range: 
          <input type="text" name="target" 
            value="<?php echo $DEFAULT_TARGET ?>">
        </td>
        <td>
            <input type="submit" name="scan" value="Scan & Update DB">
        </td>
      </tr>
    </form>
    </table>

