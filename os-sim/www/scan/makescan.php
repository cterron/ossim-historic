<?php

    $DEFAULT_TARGET = "192.168.1.1-254";

    if ($_POST["scan"]) {

        require_once('classes/Scan.inc');
        require_once 'ossim_db.inc';
        require_once 'ossim_conf.inc';

        $db = new ossim_db();
        $conn = $db->connect();

       $conf = new ossim_conf();
    
        $target = $_POST["target"];
        $target = escapeshellcmd($target);
       	$nmap = $conf->get_conf("nmap_path");
        $ips = shell_exec("$nmap -sP -v -n $target");
        $ip_list = explode("\n", $ips);
?>

        <a href="scan.php">Back</a><br><br>

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

    <form method="post" action="<?php echo $_SERVER["PHP_SELF"]?>">
      <input type="hidden" name="scan" value="scan">
      Range: 
      <input type="text" name="target" value="<?php echo $DEFAULT_TARGET ?>">
      <br/><input type="submit" value="Scan">
    </form>

