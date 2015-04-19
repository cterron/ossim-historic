<?php

class Incident_alarm {

    var $id;
    var $incident_id;
    var $src_ips;
    var $dst_ips;
    var $src_ports;
    var $dst_ports;

    function Incident_alarm ($id, $incident_id, 
                             $src_ips, $dst_ips, $src_ports, $dst_ports)
    {
        $this->id           = intval($id);
        $this->incident_id  = intval($incident_id);
        $this->src_ips      = $src_ips;
        $this->dst_ips      = $dst_ips;
        $this->src_ports    = $src_ports;
        $this->dst_ports    = $dst_ports;
    }

    function get_id()           { return $this->id;         }
    function get_incident_id()  { return $this->incident_id;}
    function get_src_ips()      { return $this->src_ips;    }
    function get_dst_ips()      { return $this->dst_ips;    }
    function get_src_ports()    { return $this->src_ports;  }
    function get_dst_ports()    { return $this->dst_ports;  }

    function get_list ($conn, $args)
    {
        $query = "SELECT * FROM incident_alarm $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_alarm (
                                        $rs->fields["id"],
                                        $rs->fields["incident_id"],
                                        $rs->fields["src_ips"],
                                        $rs->fields["dst_ips"],
                                        $rs->fields["src_ports"],
                                        $rs->fields["dst_ports"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
