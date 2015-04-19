<?php

require_once ("ossim_db.inc");

class Incident_anomaly {

    var $id;
    var $incident_id;
    var $anom_type;
    var $ip;
    var $data_orig;
    var $data_new;

    function Incident_anomaly ($id, $incident_id, $anom_type, $ip, $data_orig, $data_new)
    {
        $this->id           = intval($id);
        $this->incident_id  = intval($incident_id);
        $this->anom_type    = $anom_type;
        $this->ip           = $ip;
        $this->data_orig    = $data_orig;
        $this->data_new     = $data_new;
    }

    function get_id()           { return $this->id;         }
    function get_incident_id()  { return $this->incident_id;}
    function get_anom_type()    { return $this->anom_type;  }
    function get_ip()           { return $this->ip;         }
    function get_data_orig()    { return $this->data_orig;  }
    function get_data_new()     { return $this->data_new;   }

    function get_list ($conn, $args)
    {
        $list = array();
        $query = OssimQuery("SELECT * FROM incident_anomaly $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_anomaly (
                                        $rs->fields["id"],
                                        $rs->fields["incident_id"],
                                        $rs->fields["anom_type"],
                                        $rs->fields["ip"],
                                        $rs->fields["data_orig"],
                                        $rs->fields["data_new"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
