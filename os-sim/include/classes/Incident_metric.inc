<?php

class Incident_metric {

    var $id;
    var $incident_id;
    var $target;
    var $metric_type;
    var $metric_value;

    function Incident_metric ($id, $incident_id, 
                              $target, $metric_type, $metric_value)
    {
        $this->id           = intval($id);
        $this->incident_id  = intval($incident_id);
        $this->target       = htmlentities($target);
        $this->metric_type  = htmlentities($metric_type);
        $this->metric_value = intval($metric_value);
    }

    function get_id()           { return $this->id;             }
    function get_incident_id()  { return $this->incident_id;    }
    function get_target()       { return $this->target;         }
    function get_metric_type()  { return $this->metric_type;    }
    function get_metric_value() { return $this->metric_value;   }

    function get_list ($conn, $args)
    {
        $query = "SELECT * FROM incident_metric $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_metric (
                                        $rs->fields["id"],
                                        $rs->fields["incident_id"],
                                        $rs->fields["target"],
                                        $rs->fields["metric_type"],
                                        $rs->fields["metric_value"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
