<?php

class Backlog_alert {

    var $backlog_id;
    var $alert_id;
    var $time_out;
    var $occurrence;
    var $rule_level;
    var $matched;


    function Backlog_alert ($backlog_id, $alert_id, $time_out, $occurrence,
                            $rule_level, $matched)
    {
        $this->backlog_id   = $backlog_id;
        $this->alert_id     = $alert_id;
        $this->time_out     = $time_out;
        $this->occurrence   = $occurrence;
        $this->rule_level   = $rule_level;
        $this->matched      = $matched;
    }

    function get_backlog_id()   { return $this->backlog_id; }
    function get_alert_id()     { return $this->alert_id;   }
    function get_time_out()     { return $this->time_out;   }
    function get_occurrence()   { return $this->occurrence; }
    function get_rule_level()   { return $this->rule_level; }
    function get_matched()      { return $this->matched;    }
    

    function get_count($conn) 
    {
        $query = "SELECT count(*) FROM backlog_alert;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT *, inet_ntoa(src_ip), inet_ntoa(dst_ip) 
            FROM backlog_alert $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Backlog_alert ($rs->fields["backlog_id"], 
                                       $rs->fields["alert_id"],
                                       $rs->fields["time_out"],
                                       $rs->fields["occurrence"],
                                       $rs->fields["rule_level"],
                                       $rs->fields["matched"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

}


?>
