<?php
    $SNORT_FILE = "snort.conf";
    $OSSIM_FILE = "ossim.conf";
?>

<html>
<head>
  <title> ossim </title>
  <meta http-equiv="pragma" content="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Ossim Framework</h1>
  <h2>Edit sensor properties</h2>

  <!-- menu -->
  <p>
  <a href="?snort=1" 
     title="edit snort properties">snort</a>&nbsp;·&nbsp;
  <a href="?spade=1" 
     title="edit snort properties">spade</a>&nbsp;·&nbsp;
  <a href="?ntop=1" 
     title="edit snort properties">ntop</a>&nbsp;·&nbsp;
  <a href="?ossim=1" 
     title="edit snort properties">ossim</a>
  </p>
  <!-- end menu -->

<?php

    /* 
     * S N O R T 
     */
    if ($_REQUEST["snort"]) {

        if (!$fd = fopen($SNORT_FILE, 'r+')) {
            echo "Error opening file\n";
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
    <form action="sensoredit.php" method="post">
      <input type="hidden" name="snortwrite" value="1">
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
        <th>SNORT_PASSWORD</th>
        <td><input type="password" name="snort_password" 
                   value="<?php echo $snort_password; ?>"></td>
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
        $buff = ereg_replace("\noutput database: log, ([^,]+)\s*[^\n]*",
                             "\noutput database: log, \\1, user=$snort_user password=$snort_password dbname=$snort_dbname host=$snort_host",
                             $buff);

        if (!$fd = fopen ($location, "w")) echo "Error opening file\n";
        fwrite ($fd, $buff);
        fclose ($fd);

        echo "<p>Sensor edit completed</p>\n";
    }



    /*
     * O S S I M
     */
    elseif($_REQUEST["ossim"]) {
    
        if (!$fd = fopen($OSSIM_FILE, 'r+')) {
            echo "Error opening file\n";
            exit;
        }
        
        while (!feof($fd))
        {
            $line = fgets($fd, 4096);

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



            
        }
        
        fclose($fd);
?>
    <table align="center">
    <form action="sensoredit.php" method="post">
      <input type="hidden" name="ossimwrite" value="1">
      <tr><th colspan="2">OSSIM configuration</th></tr>
      <tr><th colspan="2"></th></tr>
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
        <td>password for the database connection</td>
        <td><input type="text" name="ossim_pass" 
                   value="<?php echo $ossim_pass; ?>">
        </td>
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
        <td>password for the snort database connection</td>
        <td><input type="text" name="snort_pass" 
                   value="<?php echo $snort_pass; ?>">
        </td>
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
         * database configuration
         */
        $buff = ereg_replace("ossim_base=([^\n]*)", 
                             "ossim_base=$ossim_base",
                             $buff);
        $buff = ereg_replace("ossim_user=([^\n]*)", 
                             "ossim_user=$ossim_user",
                             $buff);
        $buff = ereg_replace("ossim_pass=([^\n]*)", 
                             "ossim_pass=$ossim_pass",
                             $buff);
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
        
        
        if (!$fd = fopen ($location, "w")) echo "Error opening file\n";
        fwrite ($fd, $buff);
        fclose ($fd);
                                                                                                                                                
        echo "<p>Sensor edit completed</p>\n";        
    }
?>

</body>
</html>

