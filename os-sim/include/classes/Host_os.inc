<?php

require_once ("ossim_db.inc");

class Host_os{

    function get_list($conn, $inf, $sup)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
        
        $nanoms = $sup - $inf;

        $sql = "SELECT distinct ip, sensor FROM host_os LIMIT ? OFFSET ?";
        $params = array($nanoms, $inf);

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
                            or (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
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



    function get_ip_list($conn, $host, $sensor)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as  ip_addr from host_os where 
                ip = inet_aton(?) and sensor = inet_aton(?)  order by date desc";
        $params = array($host, $sensor);
        
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
        {
            if ($aux->RecordCount() == "1") {
                $list[] =  array("ip" => $aux->fields["ip_addr"], "os" => $aux->fields["os"],
                                 "date" => $aux->fields["date"], "old_os" => $aux->fields["os"],
                                 "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                 "interface" => $aux->fields["interface"]);
            } else {
                if ($aux->fields["anom"] == "0") {
                    $list[] =  array("ip" => $aux->fields["ip_addr"], "os" => $aux->fields["os"],
                                     "date" => $aux->fields["date"], "old_os" => $aux->fields["os"],
                                     "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                     "interface" => $aux->fields["interface"]);
                    $aux->MoveNext();
                }    
                $ip = $aux->fields["ip_addr"];
                $os = $aux->fields["os"];
                $date = $aux->fields["date"];
                $sensor = $aux->fields["ip_sensor"];
                $interface = $aux->fields["interface"];
                $aux->MoveNext();
                while (!$aux->EOF){
                    if ($aux->fields["anom"] == "0") {
                        $list[] =  array("ip" => $aux->fields["ip_addr"], "os" => $aux->fields["os"],
                                         "date" => $aux->fields["date"], "old_os" => $aux->fields["os"],
                                         "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                         "interface" => $aux->fields["interface"]);
                        $aux->MoveNext();
                    }
                    $old_os  = $aux->fields["os"];
                    $old_date = $aux->fields["date"];
                    $list[] =  array("ip" => $ip, "os" => $os, "date" => $date, "old_os" => $old_os,
                                     "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);
                    if (!$aux->EOF) $aux->MoveNext();
                    $os = $old_os;
                    $date = $old_date;
                }
            }
        }
        return $list;
    } 



    function get_anom_list($conn, $limit = 10)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $list = array();

        $sql = "SELECT distinct ip, sensor FROM host_os";
        if (!$ips = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            $num_anom = 0;
            while ((!$ips->EOF) && (($num_anom < $limit) || ($limit == "all"))) {
                $sql2 = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip)
                         as ip_addr from host_os where ip= ? and sensor = ? order by date desc";
                $params2 = array($ips->fields["ip"], $ips->fields["sensor"]);

                if (!$aux = &$conn->Execute($sql2, $params2)) {
                    print $conn->ErrorMsg();
                } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                           (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
                {
                    $ip        = $aux->fields["ip_addr"];    
                    $os        = $aux->fields["os"];
                    $date      = $aux->fields["date"];
                    $sensor    = $aux->fields["ip_sensor"];
                    $interface = $aux->fields["interface"];
                    if ((!$aux->EOF) && ($aux->RecordCount() > 1)) {
                        $aux->MoveNext();
                    }    
                    if ($aux->RecordCount() > 1) {    
                        $old_os  = $aux->fields["os"];
                        $old_date = $aux->fields["date"];
                        $control = 0;    
                        while ((!$aux->EOF) && ($control == "0")) {
                            $old_date = $aux->fields["date"];
                            $old_os  = $aux->fields["os"];
                            if ($aux->fields["anom"] == "0") $control = "1"; 
                            $aux->MoveNext();
                        }
                        
                        $num_anom++;

                        list($os_main, ) = split(" ", $os, 2);
                        list($previous_main, ) = split(" ", $old_os, 2);
                        if ($os_main != $previous_main) {
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

    function get_anom_ip_list($conn, $host, $sensor)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
                
        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr from host_os where 
                ip = inet_aton(?) and sensor = inet_aton(?) order by date desc";
        $params = array($host, $sensor);
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
        {
            $os  = $aux->fields["os"];
            $date = $aux->fields["date"];
            $sensor = $aux->fields["ip_sensor"];
            $interface = $aux->fields["interface"];
            if ((!$aux->EOF) && ($aux->RecordCount() <= 1)) {
                $aux->MoveNext();
            }             
            while ((!$aux->EOF) && ($aux->fields["anom"] == "1")) {
                $old_os = $aux->fields["os"];
                $old_date = $aux->fields["date"];
                $list[] = array("ip" => $host, "os" => $os, "date" => $date, "old_os" => $old_os, "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);
                $aux->MoveNext();
                $os = $old_os;
                $date = $old_date;
            }
            
        }
        return $list;
    }

    function get_ip_data($conn, $host)
    {
        $sql  = "SELECT * from host_os where ip = inet_aton(?) order by date desc";
        $params = array($host);
        
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $os = $aux->fields["os"];
            if (($aux->fields["anom"] == "0") && ($aux->RecordCount() > 1)) {
                while (!$aux->EOF) {
                    return array("host" => $host, "os" => $aux->fields["os"]);
                $aux->MoveNext();
                }
            } 
            return array("host" => $host, "os" => $os);
        }
    }

    function get_list_count($conn)
    {
        $sql = "SELECT count(distinct(ip)) from host_os";
        
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(distinct(ip))"];
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

    function get_os_pixmap($conn, $host, $pixmap_dir = '../pixmaps/') {


       if ( $os_data = Host_os::get_ip_data($conn, $host)){
            $os = $os_data["os"];
        if (preg_match('/win/i', $os)) {
                return "<img src=\"$pixmap_dir/os/win.png\" alt=\"win\" />";
            } elseif (preg_match('/linux/i', $os)) {
                return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"linux\" />";
            } elseif (preg_match('/bsd/i', $os)) {
                return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"bsd\" />";
            } elseif (preg_match('/mac/i', $os)) {
                return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"mac\" />";
            } elseif (preg_match('/sun|solaris/i', $os)) {
                return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"sunos\" />";
            }
          } else return "";
    }

}
?>
