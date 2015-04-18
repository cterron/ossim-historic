<?php

class Host_netbios {

    var $ip;
    var $name;
    var $wgroup;

    function Host_netbios ($ip, $name, $wgroup)
    {
        $this->ip       = $ip;
        $this->name     = $name;
        $this->wgroup   = $wgroup;
    }

    function get_ip()       { return $this->ip; }
    function get_name()     { return $this->name; }
    function get_wgroup()   { return $this->wgroup; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM host_netbios $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_netbios ( $rs->fields["ip"],
                                             $rs->fields["name"],
                                             $rs->fields["wgroup"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}


?>

