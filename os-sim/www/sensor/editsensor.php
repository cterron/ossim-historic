<?php
    require_once ('ossim_conf.inc');
    $ossim_conf = new ossim_conf();
    $base_dir = $ossim_conf->get_conf("base_dir");
    
    $REMOTE_PATH = "/etc";      # where to get and insert config files
    $LOCAL_PATH = "/tmp";       # where to download config files to modify
    $DEFAULT_PATH = "$base_dir/etc/"; # where to get default config files
    
    $SNORT_FILE = "$LOCAL_PATH/snort.conf";
    $OSSIM_FILE = "$LOCAL_PATH/ossim.conf";
    $SPADE_FILE = "$LOCAL_PATH/spade.conf";

    $SNORT_FILE_DEFAULT = "$DEFAULT_PATH/snort.conf.sample";
    $OSSIM_FILE_DEFAULT = "$DEFAULT_PATH/ossim.conf.sample";
    $SPADE_FILE_DEFAULT = "$DEFAULT_PATH/spade.conf.sample";
?>

<html>
<head>
  <title> ossim </title>
  <meta http-equiv="pragma" content="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Ossim Framework</h1>
  <h2>Edit sensor <?php echo $_REQUEST["ip"] ?> properties</h2>

<?php

    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';

    $db = new ossim_db();
    $conn = $db->connect();

    if (!($_REQUEST["snort"] || $_REQUEST["spade"] ||
          $_REQUEST["ossim"] || $_REQUEST["ntop"] ||
          $_REQUEST["snortwrite"] || $_REQUEST["spadewrite"] ||
          $_REQUEST["ossimwrite"] || $_REQUEST["ntopwrite"] )) {

        if (!$ip = $_REQUEST["ip"]) {
        
        
            echo "<p>What sensor do you want to edit?</p>\n";
                                                                                
            if ($sensor_list = Sensor::get_list($conn, "")) {
                foreach($sensor_list as $sensor) {
                    $ip = $sensor->get_ip();
                    $name = $sensor->get_name();
?>
  <p><a href="<?php echo $_SERVER["PHP_SELF"] ?>?ip=<?php 
        echo $ip ?>"><? echo $name ?></a></p>
<?php
                }
            }

            exit;
        }

        /* 
         * Config files transfer 
         */
        @unlink($SNORT_FILE);
        @unlink($SPADE_FILE);
        @unlink($OSSIM_FILE);
        system("scp root@$ip:$REMOTE_PATH/snort.conf $SNORT_FILE");
        system("scp root@$ip:$REMOTE_PATH/spade.conf $SPADE_FILE");
        system("scp root@$ip:$REMOTE_PATH/ossim.conf $OSSIM_FILE");

    }
    
    $db->close($conn);

?>


  <!-- menu -->
  <p>
  <a href="?snort=1&ip=<?php echo $ip ?>" 
     title="edit snort properties">snort</a>&nbsp;·&nbsp;
  <a href="?spade=1&ip=<?php echo $ip ?>" 
     title="edit snort properties">spade</a>&nbsp;·&nbsp;
  <a href="?ntop=1&ip=<?php echo $ip ?>" 
     title="edit snort properties">ntop</a>&nbsp;·&nbsp;
  <a href="?ossim=1&ip=<?php echo $ip ?>" 
     title="edit snort properties">ossim</a>
  </p>
  <!-- end menu -->

<?php

    /* 
     * S N O R T 
     */
    if ($_REQUEST["snort"]) {

        if (!$fd = @fopen($SNORT_FILE, 'r+')) {
            echo "Error opening $SNORT_FILE file\n";
            exit;
        }
        while (!feof($fd))
        {
            $line = fgets($fd, 4096);

            /* 
             * network variables 
             */
            if (preg_match("/^var HOME_NET\s*(.*)/", $line, $regs)) {
                $home_net = $regs[1];
            }
            if (preg_match("/^var EXTERNAL_NET\s*(.*)/", $line, $regs)) {
                $external_net = $regs[1];
            }

            /* 
             * Path to the rules files
             */
            if (preg_match("/^var RULE_PATH\s*(.*)/", $line, $regs)) {
                $rule_path = $regs[1];
            }
            
            /* 
             * output database
             */
            if (preg_match("/^output database:/", $line, $regs)) {
                if (preg_match("/user=([^\s]+)/", $line, $regs))
                    $snort_user = $regs[1];
                if (preg_match("/dbname=([^\s]+)/", $line, $regs))
                    $snort_dbname = $regs[1];
                if (preg_match("/host=([^\s]+)/", $line, $regs))
                    $snort_host = $regs[1];
                if (preg_match("/password=([^\s]+)/", $line, $regs))
                    $snort_password = $regs[1];
            }
        }
        fclose($fd);
?>
    <table align="center">
      <form action="editsensor.php" method="post">
      <input type="hidden" name="snortload" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr>
        <td>Load default values for snort config file <br>
          <i>(use this option only if you are sure)</i></td>
      </tr>
      <tr>
        <td align="center" colspan="2">
          <input type="submit" value="LOAD DEFAULT">
        </td>
      </tr>
      </form>
    </table>

    <br><br>

    
    <table align="center">
    <form action="editsensor.php" method="post">
      <input type="hidden" name="snortwrite" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr><th colspan="2">Snort configuration</th></tr>
      <tr>
        <th>HOME_NET</th>
        <td><input type="text" name="home_net" 
                   value="<?php echo $home_net; ?>"></td>
      </tr>
      <tr>
        <th>EXTERNAL_NET</th>
        <td><input type="text" name="external_net" 
                   value="<?php echo $external_net; ?>"></td>
      </tr>
      <tr>
        <th>RULE_PATH</th>
        <td><input type="text" name="rule_path" 
                   value="<?php echo $rule_path; ?>"></td>
      </tr>
      <tr>
        <th>SNORT_USER</th>
        <td><input type="text" name="snort_user" 
                   value="<?php echo $snort_user; ?>"></td>
      </tr>
      <tr>
        <th>SNORT_DBNAME</th>
        <td><input type="text" name="snort_dbname" 
                   value="<?php echo $snort_dbname; ?>"></td>
      </tr>
      <tr>
        <th>SNORT_HOST</th>
        <td><input type="text" name="snort_host" 
                   value="<?php echo $snort_host; ?>"></td>
      </tr>
      <tr>
        <th>SNORT_PASSWORD*</th>
        <td><input type="password" name="snort_password" 
                   value="<?php //echo $snort_password; ?>"></td>
      </tr>
      <tr>
        <td colspan="2">* <i>Password unchanged if field left blank.</i></td>
      </tr>
      <tr>
        <td align="center" colspan="2">
          <input type="submit" value="WRITE">
        </td>
      </tr>
    </form>
    </table>

   
   

<?php

    } elseif($_POST["snortwrite"]) {
    
        $buff = file_get_contents($SNORT_FILE);
        $location = "$SNORT_FILE";
        if (file_exists($location)) {
            unlink($location);
        }

        /* 
         * network variables 
         */
        $buff = ereg_replace("\nvar HOME_NET\s*[^\n]*",
                             "\nvar HOME_NET $home_net",
                             $buff);
        $buff = ereg_replace("\nvar EXTERNAL_NET\s*[^\n]*",
                             "\nvar EXTERNAL_NET $external_net",
                             $buff);

        /* 
         * Path to the rules files
         */
        $buff = ereg_replace("\nvar RULE_PATH\s*[^\n]*",
                             "\nvar RULE_PATH $rule_path",
                             $buff);

        /* 
         * output database
         */
        if ($snort_password)
            $buff = ereg_replace("\noutput database: log, ([^,]+)\s*[^\n]*",
                                 "\noutput database: log, \\1, user=$snort_user password=$snort_password dbname=$snort_dbname host=$snort_host",
                                $buff);
        else
            $buff = ereg_replace("\noutput database: log,([^,]+)\s*(user=([^\s*])\s*|password=([^\s*])\s*|dbname=([^\s*])\s*|host=([^\s*])\s*){1,4}",
                                "\noutput database: log, \\1, user=$snort_user password=\\4 dbname=$snort_dbname host=$snort_host",
                                $buff);


        if (!$fd = fopen ($location, "w")) echo "Error opening file\n";
        fwrite ($fd, $buff);
        fclose ($fd);

        echo "<p>Sensor edit completed</p>\n";
        system("scp $SNORT_FILE root@$ip:$REMOTE_PATH/snort.conf");
    }

    elseif($_POST["snortload"]) {
        system("scp $SNORT_FILE_DEFAULT root@$ip:$REMOTE_PATH/snort.conf");
        system("cp $SNORT_FILE_DEFAULT $SNORT_FILE");
        echo "<p>Default values loaded</p>\n";
    }


    /*
     * O S S I M
     */
    elseif($_REQUEST["ossim"]) {
    
        if (!$fd = fopen($OSSIM_FILE, 'r+')) {
            echo "Error opening $OSSIM_FILE file\n";
            exit;
        }
        
        while (!feof($fd))
        {
            $line = fgets($fd, 4096);

            /*
             * base configuration
             */
            if (preg_match("/^base_dir=([^\n]*)/", $line, $regs))
                $base_dir = $regs[1];
            if (preg_match("/^ossim_log=([^\n]*)/", $line, $regs))
                $ossim_log = $regs[1];

            /* 
             * database configuration
             */
            if (preg_match("/^ossim_base=([^\n]*)/", $line, $regs))
                $ossim_base = $regs[1];
            if (preg_match("/^ossim_user=([^\n]*)/", $line, $regs))
                $ossim_user = $regs[1];
            if (preg_match("/^ossim_pass=([^\n]*)/", $line, $regs))
                $ossim_pass = $regs[1];
            if (preg_match("/^ossim_host=([^\n]*)/", $line, $regs))
                $ossim_host = $regs[1];

            /*
             * snort configuration
             */
            if (preg_match("/^snort_path=([^\n]*)/", $line, $regs))
                $snort_path = $regs[1];
            if (preg_match("/^snort_rules_path=([^\n]*)/", $line, $regs))
                $snort_rules_path = $regs[1];
            if (preg_match("/^snort_base=([^\n]*)/", $line, $regs))
                $snort_base = $regs[1];
            if (preg_match("/^snort_user=([^\n]*)/", $line, $regs))
                $snort_user = $regs[1];
            if (preg_match("/^snort_pass=([^\n]*)/", $line, $regs))
                $snort_pass = $regs[1];
            if (preg_match("/^snort_host=([^\n]*)/", $line, $regs))
                $snort_host = $regs[1];

            /*
             * paths
             */
            if (preg_match("/^adodb_path=([^\n]*)/", $line, $regs))
                $adodb_path = $regs[1];
            if (preg_match("/^rrdtool_path=([^\n]*)/", $line, $regs))
                $rrdtool_path = $regs[1];
            if (preg_match("/^rrdtool_lib_path=([^\n]*)/", $line, $regs))
                $rrdtool_lib_path = $regs[1];
            if (preg_match("/^mrtg_path=([^\n]*)/", $line, $regs))
                $mrtg_path = $regs[1];
            if (preg_match("/^mrtg_rrd_files_path=([^\n]*)/", $line, $regs))
                $mrtg_rrd_files_path = $regs[1];

            /*
             * applications
             */
            if (preg_match("/^nmap_path=([^\n]*)/", $line, $regs))
                $nmap_path = $regs[1];
            if (preg_match("/^p0f_path=([^\n]*)/", $line, $regs))
                $p0f_path = $regs[1];
            if (preg_match("/^arpwatch_path=([^\n]*)/", $line, $regs))
                $arpwatch_path = $regs[1];
            
            /*
             * links
             */
            if (preg_match("/^acid_link=([^\n]*)/", $line, $regs))
                $acid_link = $regs[1];
            if (preg_match("/^ntop_link=([^\n]*)/", $line, $regs))
                $ntop_link = $regs[1];
            if (preg_match("/^opennms_link=([^\n]*)/", $line, $regs))
                $opennms_link = $regs[1];
            if (preg_match("/^mrtg_link=([^\n]*)/", $line, $regs))
                $mrtg_link = $regs[1];
            if (preg_match("/^graph_link=([^\n]*)/", $line, $regs))
                $graph_link = $regs[1];
        }
        
        fclose($fd);
?>
    <table align="center">
      <form action="editsensor.php" method="post">
      <input type="hidden" name="ossimload" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr>
        <td>Load default values for ossim config file <br>
          <i>(use this option only if you are sure)</i></td>
      </tr>
      <tr>
        <td align="center" colspan="2">
          <input type="submit" value="LOAD DEFAULT">
        </td>
      </tr>
      </form>
    </table>

    <br><br>

    <table align="center">
    <form action="editsensor.php" method="post">
      <input type="hidden" name="ossimwrite" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr><th colspan="2">OSSIM configuration</th></tr>
      <tr><th colspan="2"></th></tr>
      <tr><th colspan="2">Base configuration</th></tr>
      <tr>
        <td>Base directory</td>
        <td><input type="text" name="base_dir" 
                   value="<?php echo $base_dir; ?>">
        </td>
      </tr>
      <tr>
        <td>Log file</td>
        <td><input type="text" name="ossim_log" 
                   value="<?php echo $ossim_log; ?>">
        </td>
      </tr>
      <tr><th colspan="2">Database</th></tr>
      <tr>
        <td>hostname of the mysql database server</td>
        <td><input type="text" name="ossim_host" 
                   value="<?php echo $ossim_host; ?>">
        </td>
      </tr>
      <tr>
        <td>name of the database</td>
        <td><input type="text" name="ossim_base" 
                   value="<?php echo $ossim_base; ?>">
        </td>
      </tr>
      <tr>
        <td>name of the database user</td>
        <td><input type="text" name="ossim_user" 
                   value="<?php echo $ossim_user; ?>">
        </td>
      </tr>
      <tr>
        <td>password for the database connection *</td>
        <td><input type="text" name="ossim_pass" 
                   value="<?php //echo $ossim_pass; ?>">
        </td>
      </tr>
      <tr>
        <td colspan="2">* <i>Leave field blank for password unchanged</i></td>
      </tr>
      <tr><th colspan="2">Snort</th></tr>
      <tr>
        <td>path to snort</td>
        <td><input type="text" name="snort_path" 
                   value="<?php echo $snort_path; ?>">
        </td>
      </tr>
      <tr>
        <td>path to snort rules directory</td>
        <td><input type="text" name="snort_rules_path" 
                   value="<?php echo $snort_rules_path; ?>">
        </td>
      </tr>
        <td>hostname of the snort database server</td>
        <td><input type="text" name="snort_host" 
                   value="<?php echo $snort_host; ?>">
        </td>
      </tr>
      <tr>
        <td>name of the snort database</td>
        <td><input type="text" name="snort_base" 
                   value="<?php echo $snort_base; ?>">
        </td>
      </tr>
      <tr>
        <td>name of the snort database user</td>
        <td><input type="text" name="snort_user" 
                   value="<?php echo $snort_user; ?>">
        </td>
      </tr>
      <tr>
        <td>password for the snort database connection **</td>
        <td><input type="text" name="snort_pass" 
                   value="<?php echo $snort_pass; ?>">
        </td>
      </tr>
      <tr>
        <td colspan="2">** <i>Leave field blank for password unchanged</i></td>
      </tr>
      <tr><th colspan="2">Paths</th></tr>
      <tr>
        <td>adodb</td>
        <td><input type="text" name="adodb_path" 
                   value="<?php echo $adodb_path; ?>">
        </td>
      </tr>
      <tr>
        <td>rrdtool</td>
        <td><input type="text" name="rrdtool_path" 
                   value="<?php echo $rrdtool_path; ?>">
        </td>
      </tr>
      <tr>
        <td>rrdtool lib directory</td>
        <td><input type="text" name="rrdtool_lib_path" 
                   value="<?php echo $rrdtool_lib_path; ?>">
        </td>
      </tr>
      <tr>
        <td>mrtg</td>
        <td><input type="text" name="mrtg_path" 
                   value="<?php echo $mrtg_path; ?>">
        </td>
      </tr>
      <tr>
        <td>mrtg rrd files</td>
        <td><input type="text" name="mrtg_rrd_files_path" 
                   value="<?php echo $mrtg_rrd_files_path; ?>">
        </td>
      </tr>
      <tr><th colspan="2">Applications</th></tr>
      <tr>
        <td>nmap</td>
        <td><input type="text" name="nmap_path" 
                   value="<?php echo $nmap_path; ?>">
        </td>
      </tr>
      <tr>
        <td>p0f</td>
        <td><input type="text" name="p0f_path" 
                   value="<?php echo $p0f_path; ?>">
        </td>
      </tr>
      <tr>
        <td>arpwatch</td>
        <td><input type="text" name="arpwatch_path" 
                   value="<?php echo $arpwatch_path; ?>">
        </td>
      </tr>
      <tr><th colspan="2">Links</th></tr>
      <tr>
        <td>acid</td>
        <td><input type="text" name="acid_link" 
                   value="<?php echo $acid_link; ?>">
        </td>
      </tr>
      <tr>
        <td>ntop</td>
        <td><input type="text" name="ntop_link" 
                   value="<?php echo $ntop_link; ?>">
        </td>
      </tr>
      <tr>
        <td>opennms</td>
        <td><input type="text" name="opennms_link" 
                   value="<?php echo $opennms_link; ?>">
        </td>
      </tr>
      <tr>
        <td>mrtg</td>
        <td><input type="text" name="mrtg_link" 
                   value="<?php echo $mrtg_link; ?>">
        </td>
      </tr>
      <tr>
        <td>graph</td>
        <td><input type="text" name="graph_link" 
                   value="<?php echo $graph_link; ?>">
        </td>
      </tr>
      <tr>
        <td align="center" colspan="2">
          <input type="submit" value="WRITE">
        </td>
      </tr>
    </form>
    </table>
<?php
    } elseif($_POST["ossimwrite"]) {
    
        $buff = file_get_contents($OSSIM_FILE);
        $location = "$OSSIM_FILE";
        if (file_exists($location)) {
            unlink($location);
        }

        /*
         * Base configuration
         */
        $buff = ereg_replace("base_dir=([^\n]*)", 
                             "base_dir=$base_dir",
                             $buff);
        $buff = ereg_replace("ossim_log=([^\n]*)", 
                             "ossim_log=$ossim_log",
                             $buff);


        /* 
         * database configuration
         */
        $buff = ereg_replace("ossim_base=([^\n]*)", 
                             "ossim_base=$ossim_base",
                             $buff);
        $buff = ereg_replace("ossim_user=([^\n]*)", 
                             "ossim_user=$ossim_user",
                             $buff);

        if ($ossim_pass) {
            $buff = ereg_replace("ossim_pass=([^\n]*)", 
                                 "ossim_pass=$ossim_pass",
                                 $buff);
        }
        $buff = ereg_replace("ossim_host=([^\n]*)", 
                             "ossim_host=$ossim_host",
                             $buff);

        /*
         * snort configuration
         */
        $buff = ereg_replace("snort_path=([^\n]*)", 
                             "snort_path=$snort_path",
                             $buff);
        $buff = ereg_replace("snort_rules_path=([^\n]*)", 
                             "snort_rules_path=$snort_rules_path",
                             $buff);
        $buff = ereg_replace("snort_base=([^\n]*)", 
                             "snort_base=$snort_base",
                             $buff);
        $buff = ereg_replace("snort_user=([^\n]*)", 
                             "snort_user=$snort_user",
                             $buff);
        $buff = ereg_replace("snort_pass=([^\n]*)", 
                             "snort_pass=$snort_pass",
                             $buff);
        $buff = ereg_replace("snort_host=([^\n]*)", 
                             "snort_host=$snort_host",
                             $buff);
        
        /*
         * paths
         */
        $buff = ereg_replace("adodb_path=([^\n]*)", 
                             "adodb_path=$adodb_path",
                             $buff);
        $buff = ereg_replace("rrdtool_path=([^\n]*)", 
                             "rrdtool_path=$rrdtool_path",
                             $buff);
        $buff = ereg_replace("rrdtool_lib_path=([^\n]*)", 
                             "rrdtool_lib_path=$rrdtool_lib_path",
                             $buff);
        $buff = ereg_replace("mrtg_path=([^\n]*)", 
                             "mrtg_path=$mrtg_path",
                             $buff);
        $buff = ereg_replace("mrtg_rrd_files_path=([^\n]*)", 
                             "mrtg_rrd_files_path=$mrtg_rrd_files_path",
                             $buff);

        /*
         * applications
         */
        $buff = ereg_replace("nmap_path=([^\n]*)", 
                             "nmap_path=$nmap_path",
                             $buff);
        $buff = ereg_replace("p0f_path=([^\n]*)", 
                             "p0f_path=$p0f_path",
                             $buff);
        $buff = ereg_replace("arpwatch_path=([^\n]*)", 
                             "arpwatch_path=$arpwatch_path",
                             $buff);
        
        /*
         * links
         */
        $buff = ereg_replace("acid_link=([^\n]*)", 
                             "acid_link=$acid_link",
                             $buff);
        $buff = ereg_replace("ntop_link=([^\n]*)", 
                             "ntop_link=$ntop_link",
                             $buff);
        $buff = ereg_replace("opennms_link=([^\n]*)", 
                             "opennms_link=$opennms_link",
                             $buff);
        $buff = ereg_replace("mrtg_link=([^\n]*)", 
                             "mrtg_link=$mrtg_link",
                             $buff);
        $buff = ereg_replace("graph_link=([^\n]*)", 
                             "graph_link=$graph_link",
                             $buff);

        
        if (!$fd = fopen ($location, "w")) echo "Error opening file\n";
        fwrite ($fd, $buff);
        fclose ($fd);
                                                                                                                                                
        system("scp $OSSIM_FILE root@$ip:$REMOTE_PATH/ossim.conf");
        echo "<p>Sensor edit completed</p>\n";        
    }


    elseif($_POST["ossimload"]) {
        system("scp $OSSIM_FILE_DEFAULT root@$ip:$REMOTE_PATH/ossim.conf");
        system("cp $OSSIM_FILE_DEFAULT $OSSIM_FILE");
        echo "<p>Default values loaded</p>\n";
    }
    
    /*
     * S P A D E
     */
    elseif($_REQUEST["spade"]) {
    
        if (!$fd = fopen($SPADE_FILE, 'r+')) {
            echo "Error opening $SPADE_FILE file\n";
            exit;
        }
        while (!feof($fd)) {
            $line = fgets($fd, 4096);

            if (preg_match("/^var SPADEDIR ([^\n]*)/", $line, $regs))
                $spadedir = $regs[1];
            if (preg_match("/preprocessor spade:/", $line, $regs)) {
                if (preg_match("/dest=([^\s]+)/", $line, $regs))
                    $spade_dest = $regs[1];
                if (preg_match("/logfile=([^\s]+)/", $line, $regs))
                    $spade_logfile = $regs[1];
                if (preg_match("/statefile=([^\s]+)/", $line, $regs))
                    $spade_statefile = $regs[1];
            }
            if (preg_match("/preprocessor spade-homenet:\s*(.*)/", 
                           $line, $regs)) {
                    $spade_homenet = $regs[1];
            }
            
        }
        fclose($fd);
?>

    <table align="center">
      <form action="editsensor.php" method="post">
      <input type="hidden" name="spadeload" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr>
        <td>Load default values for spade config file <br>
          <i>(use this option only if you are sure)</i></td>
      </tr>
      <tr>
        <td align="center" colspan="2">
          <input type="submit" value="LOAD DEFAULT">
        </td>
      </tr>
      </form>
    </table>

    <br><br>

    <table align="center">
    <form action="editsensor.php" method="post">
      <input type="hidden" name="spadewrite" value="1">
      <input type="hidden" name="ip" value="<?php echo $ip ?>">
      <tr><th colspan="2">Spade configuration</th></tr>
      <tr>
        <th>SPADEDIR</th>
        <td><input type="text" name="spadedir"
                   value="<?php echo $spadedir; ?>"></td>
      </tr>
      <tr>
        <th>dest</th>
        <td><input type="text" name="spade_dest"
                   value="<?php echo $spade_dest; ?>"></td>
      </tr>
      <tr>
        <th>logfile</th>
        <td><input type="text" name="spade_logfile"
                   value="<?php echo $spade_logfile; ?>"></td>
      </tr>
      <tr>
        <th>statefile</th>
        <td><input type="text" name="spade_statefile"
                   value="<?php echo $spade_statefile; ?>"></td>
      </tr>
      <tr>
        <th>homenet</th>
        <td><input type="text" name="spade_homenet"
                   value="<?php echo $spade_homenet; ?>"></td>
      </tr>
      <tr>
        <td align="center" colspan="2">           
            <input type="submit" value="WRITE">
        </td>
      </tr>
    </form>
    </table>

<?php
    } elseif($_POST["spadewrite"]) {

        $buff = file_get_contents($SPADE_FILE);
        $location = "$SPADE_FILE";
        if (file_exists($location)) {
            unlink($location);
        }

        $buff = ereg_replace("var SPADEDIR ([^\n]*)",
                             "var SPADEDIR $spadedir",
                             $buff);
        $buff = ereg_replace("\npreprocessor spade:[^\n]*",
                             "\npreprocessor spade: dest=$spade_dest logfile=$spade_logfile statefile=$spade_statefile",
                             $buff);

        $buff = ereg_replace("\npreprocessor spade-homenet:[^\n]*",
                             "\npreprocessor spade-homenet: $spade_homenet",
                             $buff);

        if (!$fd = fopen ($location, "w")) echo "Error opening file\n";
        fwrite ($fd, $buff);
        fclose ($fd);
                                                                                
                                                                                
        echo "<p>Sensor edit completed</p>\n";
        system("scp $SPADE_FILE root@$ip:$REMOTE_PATH/spade.conf");

    }
    
    elseif($_POST["spadeload"]) {
        system("scp $SPADE_FILE_DEFAULT root@$ip:$REMOTE_PATH/spade.conf");
        system("cp $SPADE_FILE_DEFAULT $SPADE_FILE");
        echo "<p>Default values loaded</p>\n";
    }
?>

</body>
</html>

