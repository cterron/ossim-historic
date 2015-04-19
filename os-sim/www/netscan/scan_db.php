<?php
    session_start();
    $scan = $_SESSION["_scan"];
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

    /*
     * scan_db.php
     *
     * Update ossim database with scan structure
     */

    update_db($_POST, $scan);
    echo "<br/><a href=\"../host/host.php\">" . 
        gettext ("Return to host's policy") . "</a>";


    function update_db($global_info, $scan)
    {
        require_once ('ossim_db.inc');
        require_once ('classes/Host.inc');
        require_once ('classes/Host_scan.inc');
        require_once ('classes/Host_services.inc');

        $db = new ossim_db();
        $conn = $db->connect();


        $ips = $global_info["ips"];

        for ($i = 0; $i < $ips; $i++)
        {
            if ($ip = $global_info["ip_$i"])
            {

                /* sensor info */
                $sensors = array();
                for ($j = 1; $j <= $global_info["nsens"]; $j++) {
                    $name = "mboxs" . $j;
                    if (validateVar($global_info[$name])) {
                        $sensors[] = validateVar($global_info[$name]);
                    }
                }


                if (Host::in_host($conn, $ip)) {
                    echo "* " . gettext("Updating ") . "$ip..<br/>";

                    Host::update ($conn,
                                  $ip,
                                  gethostbyname($ip),
                                  $global_info["asset"],
                                  $global_info["threshold_c"],
                                  $global_info["threshold_a"], 
                                  $global_info["rrd_profile"],
                                  0,
                                  0, 
                                  $global_info["nat"],
                                  $sensors,
                                  $global_info["descr"],
                                  $scan["$ip"]["os"], 
                                  $scan["$ip"]["mac"],
                                  $scan["$ip"]["mac_vendor"]);
                                  
                    Host_scan::delete ($conn, $ip, 3001);
                    if ($global_info["nessus"]) {
                        Host_scan::insert ($conn, $ip, 3001);
                    }

                } else {
                    echo "<font color=\"blue\">\n";
                    echo "* " . gettext("Inserting ") . " $ip..<br/>\n";
                    echo "</font>\n";

                    Host::insert ($conn,
                                  $ip,
                                  gethostbyname($ip),
                                  $global_info["asset"],
                                  $global_info["threshold_c"],
                                  $global_info["threshold_a"], 
                                  $global_info["rrd_profile"],
                                  0,
                                  0, 
                                  $global_info["nat"],
                                  $sensors,
                                  $global_info["descr"],
                                  $scan[$ip]["os"],
                                  $scan[$ip]["mac"],
                                  $scan[$ip]["mac_vendor"]);

                    if($global_info["nessus"]) {
                        Host_scan::insert ($conn, $ip, 3001, 0);
                    }                
                }
            
                /* services */
                Host_services::delete($conn, $ip);
                foreach ($scan[$ip]["services"] as $port_proto => $service)
                {
                    Host_services::insert($conn,
                                          $ip,
                                          $service["port"],
                                          $service["proto"],
                                          $service["service"],
                                          $service["service"],
                                          $service["version"],
                                          strftime("%Y-%m-%d %H:%M:%S"),
                                          1);
                }
            
                flush();
            }
        }

        $db->close($conn);
    }

?>

</body>
</html>

