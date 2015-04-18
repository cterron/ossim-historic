<?php

class Host_services {

    var $ip;
    var $port;
    var $protocol;
    var $service;
    var $service_type;
    var $version;
    var $date;
    var $origin;

    function Host_services ($ip, $port, $protocol, $service, $service_type, $version, $date, $origin)
    {
        $this->ip           = $ip;
        $this->port         = $port; 
        $this->protocol     = $protocol;
        $this->service      = $service;
        $this->service_type = $service_type;
        $this->version      = $version;
        $this->date         = $date;
        $this->origin       = $origin;
    }

    function get_ip()           { return $this->ip; }
    function get_port()         { return $this->port; }
    function get_protocol()     { return $this->protocol; }
    function get_service()      { return $this->service; }
    function get_service_type() { return $this->service_type; }
    function get_version()      { return $this->version; }
    function get_date()         { return $this->date; }
    function get_origin()       { return $this->origin; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM host_services $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                $list[] = new Host_services ( $rs->fields["inet_ntoa(ip)"],
                                            $rs->fields["port"],
                                            $rs->fields["protocol"],
                                            $rs->fields["service"],
                                            $rs->fields["service_type"],
                                            $rs->fields["version"],
                                            $rs->fields["date"],
                                            $rs->fields["origin"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $ip) 
    {
        $query = "DELETE FROM host_services WHERE ip = inet_aton('$ip') and origin = 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
    }

    function insert($conn, $ip, $port, $protocol, $service, $service_type, $version, $date, $origin)
    {
        $query = "INSERT INTO host_services (ip, port, protocol, service, service_type, version, date, origin) VALUES (inet_aton('$ip'), $port, $protocol, '$service', '$service_type', '$version', '$date', $origin)";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
    }
}


?>

