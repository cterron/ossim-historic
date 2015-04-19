<?php

require_once ("ossim_db.inc");

class Host_mac{

    function get_list($conn, $inf, $sup)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
        
        $nanoms = $sup - $inf;
        
        $sql = "SELECT distinct ip, sensor FROM host_mac LIMIT ? OFFSET ?";
        $params = array($nanoms, $inf);

        if (!$ips = &$conn->Execute($sql, $params)){
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$ips->EOF){
                $sql2 = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr
                         from host_mac where  ip= ? and  sensor = ? order by date desc";
                $params2 = array($ips->fields["ip"], $ips->fields["sensor"]);
                if (!$aux = &$conn->Execute($sql2, $params2)) {
                    print $conn->ErrorMsg();
                } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) 
                            or (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
                {
                    if (($aux->fields["anom"] == "0") || ($aux->RecordCount() == "1")) {
                        $list[] =  array("ip" => $aux->fields["ip_addr"], "mac" => $aux->fields["mac"],
                                         "date" => $aux->fields["date"], "old_mac" => $aux->fields["mac"],
                                         "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                         "vendor" => $aux->fields["vendor"], "old_vendor" => $aux->fields["vendor"],
                                         "interface" => $aux->fields["interface"]);
                    } else {
                        $ip = $aux->fields["ip_addr"];
                        $mac = $aux->fields["mac"];
                        $vendor = $aux->fields["vendor"];
                        $date = $aux->fields["date"];
                        $sensor = $aux->fields["ip_sensor"];
                        $interface = $aux->fields["interface"];
                        $aux->MoveNext();
                        while ((!$aux->EOF) && ($aux->fields["anom"] != "0")){
                            $old_mac  = $aux->fields["mac"];
                            $old_date = $aux->fields["date"];
                            $old_vendor = $aux->fields["vendor"];
                            $aux->MoveNext();
                        }
                        if ($aux->fields["anom"] == "0"){
                            $list[] =  array("ip" => $ip, "mac" => $mac, "date" => $date,
                                             "old_mac" => $aux->fields["mac"], "old_date" => $aux->fields["date"],
                                             "sensor" => $sensor,"vendor" => $vendor, 
                                             "old_vendor" => $aux->fields["vendor"], "interface" => $interface);
                        } else {
                            $list[] =  array("ip" => $ip, "mac" => $mac, "date" => $date,
                                             "old_mac" => $old_mac, "old_date" => $old_date,
                                             "sensor" => $sensor,"vendor" => $vendor,
                                             "old_vendor" => $old_vendor, "interface" => $interface);
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

        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as  ip_addr from host_mac 
                where ip = inet_aton(?) and sensor = inet_aton(?) order by date desc";
        $params = array($host, $sensor);

        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
        {
            if ($aux->RecordCount() == "1") {
                $list[] =  array("ip" => $aux->fields["ip_addr"], "mac" => $aux->fields["mac"],
                                 "date" => $aux->fields["date"], "old_mac" => $aux->fields["mac"],
                                 "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                 "vendor" => $aux->fields["vendor"], "old_vendor" => $aux->fields["vendor"],
                                 "interface" => $aux->fields["interface"]);
            } else {
                if ($aux->fields["anom"] == "0") {
                    $list[] =  array("ip" => $aux->fields["ip_addr"], "mac" => $aux->fields["mac"],
                                     "date" => $aux->fields["date"], "old_mac" => $aux->fields["mac"],
                                     "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                     "vendor" => $aux->fields["vendor"], "old_vendor" => $aux->fields["vendor"],
                                     "interface" => $aux->fields["interface"]);
                    $aux->MoveNext();
                }    
                $ip = $aux->fields["ip_addr"];
                $mac = $aux->fields["mac"];
                $vendor = $aux->fields["vendor"];
                $date = $aux->fields["date"];
                $sensor = $aux->fields["ip_sensor"];
                $interface = $aux->fields["interface"];
                $aux->MoveNext();
                while (!$aux->EOF){
                    if ($aux->fields["anom"] == "0") {
                        $list[] =  array("ip" => $aux->fields["ip_addr"], "mac" => $aux->fields["mac"],
                                         "date" => $aux->fields["date"], "old_mac" => $aux->fields["mac"],
                                         "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                         "vendor" => $aux->fields["vendor"], "old_vendor" => $aux->fields["vendor"],
                                         "interface" => $aux->fields["interface"]);
                        $aux->MoveNext();
                    }
                    $old_mac  = $aux->fields["mac"];
                    $old_date = $aux->fields["date"];
                    $old_vendor = $aux->fields["vendor"];
                    $list[] =  array("ip" => $ip, "mac" => $mac, "date" => $date, "old_mac" => $old_mac,
                                     "old_date" => $old_date, "sensor" => $sensor, "vendor" => $vendor,
                                     "old_vendor" => $old_vendor, "interface" => $interface);
                    if (!$aux->EOF) $aux->MoveNext();
                    $mac = $old_mac;
                    $date = $old_date;
                    $vendor = $old_vendor;
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

        $sql = "SELECT distinct ip, sensor FROM host_mac";

        if (!$ips = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            $num_anom = 0;
            while ((!$ips->EOF) && (($num_anom < $limit) || ($limit == "all"))) {
                $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr from host_mac where 
                        ip = ?  and sensor = ? order by date desc";
                $params = array($ips->fields["ip"], $ips->fields["sensor"]);
                if (!$aux = &$conn->Execute($sql, $params)) {
                    print $conn->ErrorMsg();
                } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                           (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
                {
                    $ip        = $aux->fields["ip_addr"];    
                    $mac       = $aux->fields["mac"];
                    $vendor    = $aux->fields["vendor"];
                    $date      = $aux->fields["date"];
                    $sensor    = $aux->fields["ip_sensor"];
                    $interface = $aux->fields["interface"];
                    if ((!$aux->EOF) && ($aux->RecordCount() > 1)) {
                        $aux->MoveNext();
                    }    
                    if ($aux->RecordCount() > 1) {    
                        $old_mac  = $aux->fields["mac"];
                        $old_vendor = $aux->fields["vendor"];
                        $old_date = $aux->fields["date"];
                        $control = 0;    
                        while ((!$aux->EOF) && ($control == "0")) {
                            $old_date = $aux->fields["date"];
                            $old_mac  = $aux->fields["mac"];
                            $old_vendor = $aux->fields["vendor"];
                            if ($aux->fields["anom"] == "0") $control = "1"; 
                            $aux->MoveNext();
                        }
                        $num_anom++;
                        $list[] = array("ip" => $ip, "mac" => $mac, "date" => $date, "old_mac" => $old_mac,
                                        "old_date" => $old_date, "sensor" => $sensor, "vendor" => $vendor,
                                        "old_vendor" => $old_vendor, "interface" => $interface);
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
                
        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr 
                from host_mac where ip = inet_aton(?) and sensor = inet_aton(?) order by date desc";
        $params = array($host, $sensor);

        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
        {
            $mac  = $aux->fields["mac"];
            $vendor = $aux->fields["vendor"];
            $date = $aux->fields["date"];
            $sensor = $aux->fields["ip_sensor"];
            $interface = $aux->fields["interface"];
            if ((!$aux->EOF) && ($aux->RecordCount() <= 1)) {
                $aux->MoveNext();
            }             
            while ((!$aux->EOF) && ($aux->fields["anom"] == "1")) {
                $old_mac = $aux->fields["mac"];
                $old_date = $aux->fields["date"];
                $old_vendor = $aux->fields["vendor"];
                $list[] = array("ip" => $host, "mac" => $mac, "date" => $date, "vendor" => $vendor, "old_vendor" => $old_vendor,
                                "old_mac" => $old_mac, "old_date" => $old_date, "sensor" => $sensor, "interface" => $interface);
                $aux->MoveNext();
                $mac = $old_mac;
                $date = $old_date;
                $vendor = $old_vendor;
            }
            
        }
        return $list;
    }

    function get_ip_data($conn, $host)
    {
        $sql = "SELECT * from host_mac where ip = inet_aton(?) order by date desc";
        $params = array($host);

        if (!$aux = &$conn->Execute($sql, $params)){
            print $conn->ErrorMsg();
        } else {
            $mac = $aux->fields["mac"];
            $vendor = $aux->fields["vendor"];
            if (($aux->fields["anom"] != "0") && ($aux->RecordCount() > 1)) {
                while (!$aux->EOF) {
                    if ($aux->fields["anom"] == "0") {
                        return array("host" => $host, "mac" => $aux->fields["mac"], "vendor" => $aux->fields["vendor"]);
                    }
                    $aux->MoveNext();
                }
            } 
            return array("host" => $host, "mac" => $mac, "vendor" => $vendor);
        }
    }

    function get_list_count($conn)
    {
        $sql = "SELECT count(distinct(ip)) from host_mac";
        
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(distinct(ip))"];
        }
     
    }

    function ack_ign($conn, $host, $date, $sensor)
    {
        $sql = "SELECT *, inet_ntoa(ip) as ip_addr, inet_ntoa(sensor) as ip_sensor from host_mac 
                where ip = inet_aton(?) and date = ? and sensor = inet_aton(?)";
        $params = array($host, $date, $sensor);
        
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $mac = $aux->fields["mac"];
            $vendor = $aux->fields["vendor"];
            $iface = $aux->fields["interface"];
        }    
        
        $sql = "INSERT INTO host_mac (ip, mac, vendor, date, sensor, interface, anom) VALUES 
                (inet_aton(?), ?, ?, CURRENT_TIMESTAMP, inet_aton(?), ?, '0')";
        $params = array($host, $mac, $vendor, $sensor, $iface);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


}
?>
