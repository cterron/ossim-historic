<?php

require_once ("ossim_db.inc");

class Host_os{

    /*
     * Return list of the all records (anomalies or not) for each host
     * An inferior and superior limit of record is given 
     * (used by secondary specific anomalie screen)
     */
    function get_list($conn, $inf, $sup)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
        
        $nanoms = $sup - $inf;

        $sql = "SELECT distinct ip, sensor FROM host_os LIMIT ? OFFSET ?";
        $params = array((int)$nanoms, (int)$inf);

        $list = array();

        if (!$ips = &$conn->Execute($sql, $params)){
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$ips->EOF){
                
                $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr 
                        from host_os where  ip= ? and sensor = ? order by date desc";
                $params = array($ips->fields["ip"], $ips->fields["sensor"]);

                
                if (!$aux = &$conn->Execute($sql, $params)) {
                    print $conn->ErrorMsg();
                } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) 
                            and (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
                {
                    if (($aux->fields["anom"] == "0") || ($aux->RecordCount() == "1")) {
                        $list[] =  array("ip" => $aux->fields["ip_addr"], "os" => $aux->fields["os"],
                                         "date" => $aux->fields["date"], "old_os" => $aux->fields["os"],
                                         "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                         "interface" => $aux->fields["interface"]);
                    } else {
                        $ip = $aux->fields["ip_addr"];
                        $os = $aux->fields["os"];
                        $date = $aux->fields["date"];
                        $sensor = $aux->fields["ip_sensor"];
                        $interface = $aux->fields["interface"];
                        $aux->MoveNext();
                        while ((!$aux->EOF) && ($aux->fields["anom"] != "0")){
                            $old_os  = $aux->fields["os"];
                            $old_date = $aux->fields["date"];
                            $aux->MoveNext();
                        }
                        if ($aux->fields["anom"] == "0"){
                            $list[] =  array("ip" => $ip, "os" => $os, "date" => $date,
                                             "old_os" => $aux->fields["os"], "old_date" => $aux->fields["date"],
                                             "sensor" => $sensor, "interface" => $interface);
                        } else {
                            $list[] =  array("ip" => $ip, "os" => $os, "date" => $date,
                                             "old_os" => $old_os, "old_date" => $old_date,
                                             "sensor" => $sensor, "interface" => $interface);
                        }
                
                    }
            }
            $ips->MoveNext();
            }
        } 
        return $list;
    } 



    /*
     * Return list of all records (anomalies or not) of a specific host
     * (used by secondary specific anomalie screen)
     */
    function get_ip_list($conn, $host, $sensor)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $list = array();
        if ( !(Session::hostAllowed($conn, $host)) or !(Session::hostAllowed($conn, $sensor)) )
            return $list;

        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as  ip_addr from host_os where 
                ip = inet_aton(?) and sensor = inet_aton(?)  order by date desc";
        $params = array($host, $sensor);
        
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
            return $list;
        }

        if ($aux->RecordCount() == "1") {
            $list[] =  array("ip" => $aux->fields["ip_addr"], "os" => $aux->fields["os"],
                             "date" => $aux->fields["date"], "old_os" => $aux->fields["os"],
                             "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                             "interface" => $aux->fields["interface"]);
            return $list;
        }

        $ip = $aux->fields["ip_addr"];
        $sensor = $aux->fields["ip_sensor"];
        $interface = $aux->fields["interface"];

        $os = $aux->fields["os"];
        $date = $aux->fields["date"];
        $anom = $aux->fields["anom"];
        $aux->MoveNext();
        do {
            $old_os  = $aux->fields["os"];
            $old_date = $aux->fields["date"];
            if ($anom == "0") {
                $list[] =  array("ip" => $ip, "os" => $os, "date" => $date, "old_os" => $os,
                                 "old_date" => $date, "sensor" => $sensor, "interface" => $interface);
            } else {
                $list[] =  array("ip" => $ip, "os" => $os, "date" => $date, "old_os" => $old_os,
                                 "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);
            }
            $os = $old_os;
            $date = $old_date;
            $anom = $aux->fields["anom"];
            $aux->MoveNext();
        }
        while (!$aux->EOF);
        $list[] =  array("ip" => $ip, "os" => $os, "date" => $date, "old_os" => $old_os,
                         "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);

        return $list;
    } 



    /*
     * Return list of the last anomalie for each host (if any)
     * A total of '$limit' records is calculated (one record per host)
     * (used by control pannel/anomalies on host summary and by secondary specific anomalie screen)
     */
    function get_anom_list($conn, $limit = 10)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $list = array();
 
	$sql = "SELECT distinct inet_ntoa(ip) as ip, inet_ntoa(sensor) as sensor FROM host_os";

        if (!$ips = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            $num_anom = 0;
            while ((!$ips->EOF) && (($num_anom < $limit) || ($limit == "all"))) {
	        if ( (Session::hostAllowed($conn, $ips->fields["ip"])) and (Session::hostAllowed($conn, $ips->fields["sensor"])) ) {
			$sql = "SELECT *, inet_ntoa(sensor) AS ip_sensor, inet_ntoa(ip) AS ip_addr FROM host_os
				WHERE ip = inet_aton(?) AND sensor = inet_aton(?) ORDER BY date DESC";
			$params = array($ips->fields["ip"], $ips->fields["sensor"]);

			if (!$aux = &$conn->Execute($sql, $params)) {
			    print $conn->ErrorMsg();
			} elseif ($aux->fields["anom"] != "0") {
			    $ip        = $aux->fields["ip_addr"];    
			    $os        = $aux->fields["os"];
			    $date      = $aux->fields["date"];
			    $sensor    = $aux->fields["ip_sensor"];
			    $interface = $aux->fields["interface"];
			    $next_anom = true;
			    $is_anom = false;
			    while ($aux->MoveNext() and $next_anom) {    
			        if ($aux->fields["anom"] == "0")
				    $next_anom = false;
				$old_os  = $aux->fields["os"];
				$old_date = $aux->fields["date"];
				list($os_main, ) = explode(" ", $os, 2);
				list($previous_main, ) = explode(" ", $old_os, 2);
				if (strtolower($os_main) != strtolower($previous_main))
				  $is_anom = true;
                            }
			    if ($is_anom) {
				$num_anom++;
				$list[] = array("ip" => $ip, "os" => $os, "date" => $date, "old_os" => $old_os,
				                "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);
			    }
			} 
		}
                $ips->MoveNext();
            } 
        }
        return $list;
    } 

    /*
     * Return list of all last anomalies of a specific host
     * (used by control pannel/anomalies, showing record detail)
     */
    function get_anom_ip_list($conn, $host, $sensor)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
                
        $list = array();

        if ( !(Session::hostAllowed($conn, $host)) or !(Session::hostAllowed($conn, $sensor)) )
	    return $list;

        $sql = "SELECT *, inet_ntoa(sensor) AS ip_sensor, inet_ntoa(ip) AS ip_addr FROM host_os
                WHERE ip = inet_aton(?) AND sensor = inet_aton(?) ORDER BY date DESC";
        $params = array($host, $sensor);

        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif ($aux->fields["anom"] != "0") {
            $os  = $aux->fields["os"];
            $date = $aux->fields["date"];
            $sensor = $aux->fields["ip_sensor"];
            $interface = $aux->fields["interface"];
	    $next_anom = true;
	    while ($aux->MoveNext() && $next_anom) {
	        if ($aux->fields["anom"] == "0")
		    $next_anom = false;
                $old_os = $aux->fields["os"];
                $old_date = $aux->fields["date"];
                $list[] = array("ip" => $host, "os" => $os, "date" => $date, "old_os" => $old_os, "old_date" => $old_date,
		                "sensor" => $sensor, "interface" => $interface);
                $os = $old_os;
                $date = $old_date;
            }
            
        }
        return $list;
    }

    /*
     * Get seen OS for this host (not return anomalies)
     * (used in inventory...)
     */
    function get_ip_data($conn, $host)
    {
        $sql  = "SELECT * FROM host_os WHERE ip = inet_aton(?) ".
                "ORDER BY date DESC";
        $params = array($host);

	$first_os = "";
        
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            // Search the latest record with a anom == 0
            while (!$aux->EOF) {
                if ($aux->fields["anom"] == "0") {
                    return array("host" => $host, "os" => $aux->fields["os"]);
                }
		if($first_os == ""){$first_os = $aux->fields["os"];}
                $aux->MoveNext();
            }
            // If no "0 anom" record found, use the last record introduced
            // in the DB
            return array("host" => $host, "os" => $first_os);
        }
        return null;
    }

    function get_list_count($conn)
    {
        $sql = "SELECT count(distinct(ip)) AS num FROM host_os";
        
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["num"];
        }
    }

    function ack_ign($conn, $host, $date, $sensor)
    {
        $sql = "SELECT *, inet_ntoa(ip) as ip_addr, inet_ntoa(sensor) as ip_sensor from host_os 
                where ip= inet_aton(?) and date = ? and sensor=inet_aton(?)";
        $params = array($host, $date, $sensor);

        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $os = $aux->fields["os"];
            $iface = $aux->fields["interface"];
        }    
        
        $sql = "INSERT INTO host_os (ip, os, date, sensor, interface, anom) VALUES 
                (inet_aton(?), ?, CURRENT_TIMESTAMP, inet_aton(?), ?, '0')";
        $params = array($host, $os, $sensor, $iface);

        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    // XXX The API should be changed, to accept a $os instead of $host
    //     avoinding one DB query
    /***** XXX Deprecated ******/
    function get_os_pixmap($conn, $host, $pixmap_dir = '../pixmaps/')
    {
        if ( $os_data = Host_os::get_ip_data($conn, $host)) {
            $os = $os_data["os"];
            if (preg_match('/win/i', $os)) {
                return "<img src=\"$pixmap_dir/os/win.png\" alt=\"Windows\" />";
            } elseif (preg_match('/linux/i', $os)) {
                return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"Linux\" />";
            } elseif (preg_match('/bsd/i', $os)) {
                return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"BSD\" />";
            } elseif (preg_match('/mac/i', $os)) {
                return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"MacOS\" />";
            } elseif (preg_match('/sunos/i', $os)) {
                return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"SunOS\" />";
            } elseif (preg_match('/solaris/i', $os)) {
                return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"Solaris\" />";
            }
        }
        return "";
    }

    function get_os_pixmap_nodb($os, $pixmap_dir = '../pixmaps/')
    {
        if (preg_match('/win/i', $os)) {
            return "<img src=\"$pixmap_dir/os/win.png\" alt=\"Windows\" />";
        } elseif (preg_match('/linux/i', $os)) {
            return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"Linux\" />";
        } elseif (preg_match('/bsd/i', $os)) {
            return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"BSD\" />";
        } elseif (preg_match('/mac/i', $os)) {
            return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"MacOS\" />";
        } elseif (preg_match('/sun/i', $os)) {
            return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"SunOS\" />";
        } elseif (preg_match('/solaris/i', $os)) {
            return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"Solaris\" />";
        }
        return "";
    }

}
?>
