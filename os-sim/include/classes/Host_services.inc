<?php

require_once ("ossim_db.inc");

class Host_services{

    function get_list($conn, $inf, $sup)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
        
		$list = array();
		$nanoms = 0;

		$nanoms = (int) $nanoms;
		$sup = (int) $sup;
		$inf = (int) $sup;

		$nanoms = $sup - $inf;

        $sql = "SELECT distinct ip, port, sensor FROM host_services LIMIT ? OFFSET ?";
        $params = array($nanoms, $inf);

        if (!$ips = &$conn->Execute($sql, $params)){
            print $conn->ErrorMsg();
        } else {
            while (!$ips->EOF){
                $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr from host_services where
                        ip= ? and  sensor = ? and  port = ? order by date desc";
                $params = array($ips->fields["ip"], $ips->fields["sensor"], $ips->fields["port"]);
                if (!$aux = &$conn->Execute($sql, $params)) {
                    print $conn->ErrorMsg();
                } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) 
                            or (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
                {
                    if (($aux->fields["anom"] == "0") || ($aux->RecordCount() == "1")) {
                        $list[] =  array("ip" => $aux->fields["ip_addr"], "port" => $aux->fields["port"],
                                         "date" => $aux->fields["date"], "protocol" => $aux->fields["protocol"],
                                         "service" => $aux->fields["service"], "service_type" => $aux->fields["service_type"],
                                         "version" => $aux->fields["version"], "origin" => $aux->fields["origin"],
                                         "old_date" => $aux->fields["date"], "sensor" => $aux->fields["ip_sensor"],
                                         "old_protocol" => $aux->fields["protocol"], "old_service" => $aux->fields["service"],
                                         "old_service_type" => $aux->fields["service_type"], "old_version" => $aux->fields["version"],
                                         "old_origin" => $aux->fields["origin"], "interface" => $aux->fields["interface"]);
                    } else {
                        $ip = $aux->fields["ip_addr"];
                        $port = $aux->fields["port"];
                        $protocol = $aux->fields["protocol"];
                        $service = $aux->fields["service"];
                        $service_type = $aux->fields["service_type"];
                        $version = $aux->fields["version"];
                        $origin = $aux->fields["origin"];
                        $date = $aux->fields["date"];
                        $sensor = $aux->fields["ip_sensor"];
                        $interface = $aux->fields["interface"];
                        $aux->MoveNext();
                        while ((!$aux->EOF) && ($aux->fields["anom"] != "0")){
                            $old_protocol = $aux->fields["protocol"];
                            $old_service = $aux->fields["service"];
                            $old_service_type = $aux->fields["service_type"];
                            $old_version = $aux->fields["version"];
                            $old_origin = $aux->fields["origin"];
                            $old_date = $aux->fields["date"];
                            $aux->MoveNext();
                        }
                        if ($aux->fields["anom"] == "0"){
                            
                            $list[] =  array("ip" => $ip, "port" => $port, "date" => $date,
                                             "old_date" => $aux->fields["date"], "protocol" => $protocol,
                                             "old_protocol" => $aux->fields["protocol"], "version" => $version,
                                             "old_version" => $aux->fields["version"], "service" => $service,
                                             "old_service" => $aux->fields["service"], "origin" => $origin,
                                             "old_origin" => $aux->fields["origin"],  "sensor" => $sensor,
                                             "interface" => $interface);
                        } else {
                            $list[] =  array("ip" => $ip, "port" => $port, "date" => $date,
                                             "old_date" => $old_date, "protocol" => $protocol,
                                             "old_protocol" => $old_protocol, "version" => $version,
                                             "old_version" => $old_version, "service" => $service,
                                             "old_service" => $old_service, "origin" => $origin,
                                             "old_origin" => $old_origin,  "sensor" => $sensor,
                                             "interface" => $interface);

                        }
                
                    }
            }
            $ips->MoveNext();
            }
        } 
        return $list;
    } 



    function get_ip_list($conn, $host, $sensor, $port)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr from host_services where 
                ip = inet_aton(?) and port = ? and sensor = inet_aton(?)  order by date desc";
        
        $params = array($host, $port, $sensor);
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif ((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"])))
        {
            if ($aux->RecordCount() == "1") {
                $list[] =  array("ip" => $aux->fields["ip_addr"], "port" => $aux->fields["port"],
                                 "protocol" => $aux->fields["protocol"], "old_protocol" => $aux->fields["protocol"],
                                 "version" => $aux->fields["version"], "old_version" => $aux->fields["version"],
                                 "service" => $aux->fields["service"], "old_service" => $aux->fields["service"],
                                 "service_type" => $aux->fields["service_type"], "old_service_type" => $aux->fields["service_type"],
                                 "origin" => $aux->fields["origin"], "old_origin" => $aux->fields["origin"],
                                 "date" => $aux->fields["date"], "old_date" => $aux->fields["date"],
                                 "sensor" => $aux->fields["ip_sensor"], "interface" => $aux->fields["interface"]);
            } else {
                if ($aux->fields["anom"] == "0") {
                    $list[] = array("ip" => $aux->fields["ip_addr"], "port" => $aux->fields["port"],
                                 "protocol" => $aux->fields["protocol"], "old_protocol" => $aux->fields["protocol"],
                                 "version" => $aux->fields["version"], "old_version" => $aux->fields["version"],
                                 "service" => $aux->fields["service"], "old_service" => $aux->fields["service"],
                                 "service_type" => $aux->fields["service_type"], "old_service_type" => $aux->fields["service_type"],
                                 "origin" => $aux->fields["origin"], "old_origin" => $aux->fields["origin"],
                                 "date" => $aux->fields["date"], "old_date" => $aux->fields["date"],
                                 "sensor" => $aux->fields["ip_sensor"], "interface" => $aux->fields["interface"]);

                    $aux->MoveNext();
                }    
                $ip = $aux->fields["ip_addr"];
                $port = $aux->fields["port"];
                $protocol = $aux->fields["protocol"];
                $version = $aux->fields["version"];
                $service = $aux->fields["service"];
                $service_type = $aux->fields["service_type"];
                $origin = $aux->fields["origin"];
                $date = $aux->fields["date"];
                $sensor = $aux->fields["ip_sensor"];
                $interface = $aux->fields["interface"];
                $aux->MoveNext();
                while (!$aux->EOF){
                    if ($aux->fields["anom"] == "0") {
                        $list[] =   array("ip" => $aux->fields["ip_addr"], "port" => $aux->fields["port"],
                                        "protocol" => $aux->fields["protocol"], "old_protocol" => $aux->fields["protocol"],
                                        "version" => $aux->fields["version"], "old_version" => $aux->fields["version"],
                                        "service" => $aux->fields["service"], "old_service" => $aux->fields["service"],
                                        "service_type" => $aux->fields["service_type"], "old_service_type" => $aux->fields["service_type"],
                                        "origin" => $aux->fields["origin"], "old_origin" => $aux->fields["origin"],
                                        "date" => $aux->fields["date"], "old_date" => $aux->fields["date"],
                                        "sensor" => $aux->fields["ip_sensor"], "interface" => $aux->fields["interface"]);
                        $aux->MoveNext();
                    }
                    $old_protocol  = $aux->fields["protocol"];
                    $old_date = $aux->fields["date"];
                    $old_version = $aux->fields["version"];
                    $old_service = $aux->fields["service"];
                    $old_service_type = $aux->fields["service_type"];
                    $old_origin = $aux->fields["origin"];
                    $list[] =  array("ip" => $ip, "port" => $aux->fields["port"], "date" => $date,
                                     "old_date" => $old_date, "protocol" => $protocol, "old_protocol" => $old_protocol,
                                     "version" => $version, "old_version" => $old_version, "service" => $service,
                                     "old_service" => $old_service, "service_type" => $service_type,
                                     "old_service_type" => $old_service_type, "origin" => $origin,
                                     "old_origin" => $old_origin, "sensor" => $sensor, "interface" => $interface);
                    if (!$aux->EOF) $aux->MoveNext();
                    $protocol = $old_protocol;
                    $date = $old_date;
                    $version = $old_version;
                    $service = $old_service;
                    $service_type = $old_service_type;
                    $origin = $old_origin;
                }
            }
        }
        return $list;
    } 



    function get_anom_list($conn, $limit = 10)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');

        $sql = "SELECT distinct ip, port, sensor FROM host_services";
        $list = array();
        if (!$ips = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            $num_anom = 0;
            while ((!$ips->EOF) && (($num_anom < $limit) || ($limit == "all"))) {
                $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor,  inet_ntoa(ip) as ip_addr from host_services where
                        ip = ? and sensor = ? and port = ? order by date desc";
                $params = array($ips->fields["ip"], $ips->fields["sensor"], $ips->fields["port"]);
                if (!$aux = &$conn->Execute($sql, $params)) {
                    print $conn->ErrorMsg();
                } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or 
                           (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
                {
                    $ip        = $aux->fields["ip_addr"];
                    $port      = $aux->fields["port"];
                    $protocol  = $aux->fields["protocol"];
                    $version   = $aux->fields["version"];
                    $service   = $aux->fields["service"];
                    $service_type = $aux->fields["service_type"];
                    $origin    = $aux->fields["origin"];
                    $date      = $aux->fields["date"];
                    $sensor    = $aux->fields["ip_sensor"];
                    $interface = $aux->fields["interface"];
                    if ((!$aux->EOF) && ($aux->RecordCount() > 1)) {
                        $aux->MoveNext();
                    }    
                    if ($aux->RecordCount() > 1) { 
                        $old_protocol  = $aux->fields["protocol"];
                        $old_version   = $aux->fields["version"];
                        $old_service   = $aux->fields["service"];
                        $old_service_type = $aux->fields["service_type"];
                        $old_origin    = $aux->fields["origin"];
                        $old_date = $aux->fields["date"];
                        $control = 0;    
                        while ((!$aux->EOF) && ($control == "0")) {
                            $old_protocol  = $aux->fields["protocol"];
                            $old_version   = $aux->fields["version"];
                            $old_service   = $aux->fields["service"];
                            $old_service_type = $aux->fields["service_type"];
                            $old_origin    = $aux->fields["origin"];
                            $old_date = $aux->fields["date"];
                            if ($aux->fields["anom"] == "0") $control = "1"; 
                            $aux->MoveNext();
                        }
                        $num_anom++;
                        $list[] = array("ip" => $ip, "port" => $port, "date" => $date, "old_date" => $old_date,
                                        "protocol" => $protocol, "old_protocol" => $old_protocol, "service" => $service,
                                        "old_service" => $old_service, "service_type" => $service_type,
                                        "old_service_type" => $old_service_type, "version" => $version, "old_version" => $old_version, 
                                        "origin" => $origin, "old_origin" => $old_origin,
                                        "sensor" => $sensor, "interface" => $interface);
                    }
                } 
            $ips->MoveNext();
            } 
        }
        return $list;
    } 

    function get_anom_ip_list($conn, $host, $sensor, $port)
    {
        require_once('classes/Session.inc');
        require_once('classes/Security.inc');
                
        $sql = "SELECT *, inet_ntoa(sensor) as ip_sensor, inet_ntoa(ip) as ip_addr from host_services
                where ip = inet_aton(?) and port = ? and sensor = inet_aton(?) order by date desc";
        $params = array($host, $port, $sensor);
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif (((Session::hostAllowed($conn, $aux->fields["ip_addr"])) or
                    (Session::hostAllowed($conn, $aux->fields["ip_sensor"]))) and ($aux->fields["anom"] != "0"))
        {
            $ip        = $aux->fields["ip_addr"];
            $port      = $aux->fields["port"];
            $protocol  = $aux->fields["protocol"];
            $version   = $aux->fields["version"];
            $service   = $aux->fields["service"];
            $service_type = $aux->fields["service_type"];
            $origin    = $aux->fields["origin"];
            $date = $aux->fields["date"];
            $sensor = $aux->fields["ip_sensor"];
            $interface = $aux->fields["interface"];
            if ((!$aux->EOF) && ($aux->RecordCount() <= 1)) {
                $aux->MoveNext();
            }             
            while ((!$aux->EOF) && ($aux->fields["anom"] == "1")) {
                $old_date = $aux->fields["date"];
                $old_protocol  = $aux->fields["protocol"];
                $old_version   = $aux->fields["version"];
                $old_service   = $aux->fields["service"];
                $old_service_type = $aux->fields["service_type"];
                $old_origin    = $aux->fields["origin"];
 
                $list[] =  array("ip" => $ip, "port" => $aux->fields["port"], "date" => $date,
                                 "old_date" => $old_date, "protocol" => $protocol, "old_protocol" => $old_protocol,
                                 "version" => $version, "old_version" => $old_version, "service" => $service,
                                 "old_service" => $old_service, "service_type" => $service_type,
                                 "old_service_type" => $old_service_type, "origin" => $origin,
                                 "old_origin" => $old_origin, "sensor" => $sensor, "interface" => $interface);
 
                $aux->MoveNext();
                $protocol = $old_protocol;
                $date = $old_date;
                $version = $old_version;
                $service = $old_service;
                $service_type = $old_service_type;
                $origin = $old_origin;
        
            }
            
        }
        return $list;
    }

    function get_ip_data($conn, $host, $origin)
    {
        $sql = "SELECT distinct(port) from host_services where ip =
                inet_aton(?) order by date desc";
        $params = array($host);
        $list = array(); 
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            while (!$aux->EOF) {
                $sql2 = "SELECT * from host_services where ip =
                inet_aton('$host') and port = '".$aux->fields['port']."'";
                 
                if ($origin != "") {
                    $sql2 .= " and origin = '$origin' ";
                }
                if (!$aux2 = &$conn->Execute($sql2)){
                    print $conn->ErrorMsg();
                } else {
                    if ($aux2->RecordCount() == 0) {
                        $aux->MoveNext();
                    } else {
                        $port      = $aux2->fields["port"];
                        $protocol  = $aux2->fields["protocol"];
                        $version   = $aux2->fields["version"];
                        $service   = $aux2->fields["service"];
                        $service_type = $aux2->fields["service_type"];
                        $anom_origin    = $aux2->fields["origin"];
                        $date = $aux2->fields["date"];
                        if (($aux2->fields["anom"] != "0") && ($aux2->RecordCount() > 1)) {
                            while (!$aux2->EOF) {
                                if ($aux2->fields["anom"] == "0") {
                                    $list[] =  array("host" => $host, "port" => $aux2->fields["port"], "protocol" => $aux2->fields["protocol"], 
                                                 "version" => $aux2->fields["version"], "service" => $aux2->fields["service"], 
                                                 "service_type" => $aux2->fields["service_type"], "date" => $aux2->fields["date"]);
                                }      
                                $aux2->MoveNext();
                            }
                        } else {  //if  
                            $list[] =  array("host" => $host, "port" => $port, "protocol" => $protocol, "version" => $version, 
                                         "service" => $service, "service_type" => $service_type, "date" => $date);
                        }
                    }
                }
            $aux->MoveNext();

            }
       }
       return $list;
    } 

    function get_list_count($conn)
    {
        $sql = "select count(*) from (SELECT distinct ip, sensor, port from host_services) table_alias";
        
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
     
    }

    function ack_ign($conn, $host, $port, $date, $sensor)
    {
        $sql = "SELECT *, inet_ntoa(ip) as ip_addr, inet_ntoa(sensor) as
                ip_sensor from host_services where ip= inet_aton(?) 
                and port = ? and date = ? and sensor=inet_aton(?)";
        $params = array($host, $port, $date, $sensor);
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $protocol = $aux->fields["protocol"];
            $version = $aux->fields["version"];
            $service = $aux->fields["service"];
            $service_type = $aux->fields["service_type"];
            $origin = $aux->fields["origin"];
            $interface = $aux->fields["interface"];
        }    
        
        $sql = "INSERT INTO host_services (ip, port, protocol, service,
                service_type, origin, date, sensor, interface, anom) 
                VALUES (inet_aton(?), ?, ?, ?, ?, ?, CURRENT_TIMESTAMP,
                inet_aton(?), ?, '0')";
        $params = array($host, $port, $protocol, $service, $service_type,
        $origin, $sensor, $interface);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function insert($conn, $host, $port, $date, $sensor, $protocol, $service, $service_type, $version, $origin)
    {
        $sql = "SELECT *, inet_ntoa(ip) as ip_addr, inet_ntoa(sensor) as
                ip_sensor from host_services where ip= inet_aton(?) 
                and port = ? and sensor=inet_aton(?)";
        $params = array($host, $port, $sensor);
        if (!$aux = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $protocol_aux = $aux->fields["protocol"];
            $version_aux = $aux->fields["version"];
            $service_aux = $aux->fields["service"];
            $service_type_aux = $aux->fields["service_type"];
        }    
        
        if (($protocol_aux != $protocol) || ($version_aux != $version) ||
            ($service_aux != $service) || ($service_type_aux != $service_type))
        {
            $sql = "INSERT INTO host_services (ip, port, protocol, service,
                    service_type, origin, date, sensor, version, anom) VALUES 
                    (inet_aton(?), ?, ?, ?, ?, ?, CURRENT_TIMESTAMP, inet_aton(?),
                    ?,'0')";
            $params = array($host, $port, $protocol, $service, $service_type,
                            $origin, $sensor, $version);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $host)
    {
        $sql = "DELETE from host_services where ip = inet_aton(?) ";
        $params = array($host);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }



}
?>
