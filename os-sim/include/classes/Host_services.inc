<?php

class Host_services {

    var $ip;
    var $service;
    var $version;

    function Host_services ($ip, $service, $version)
    {
        $this->ip           = $ip;
        $this->service      = $service;
        $this->version      = $version;
    }

    function get_ip()           { return $this->ip; }
    function get_service()      { return $this->service; }
    function get_version()      { return $this->version; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM host_services $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_services ( $rs->fields["ip"],
                                            $rs->fields["service"],
                                            $rs->fields["version"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $ip) 
    {
        $query = "DELETE FROM host_services WHERE ip = '$ip'";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
    }

    function insert($conn, $ip, $service, $version)
    {
        $query = "INSERT INTO host_services 
            VALUES ('$ip', '$service', '$version')";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
    }
}


?>

