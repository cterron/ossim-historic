<?php

class RRD_anomaly {


    var $ip;
    var $what;
    var $count;
    var $anomaly_time;
    var $range;
    var $over;
    var $acked;

    function RRD_anomaly ($ip, $what, $count, $anomaly_time, $range,
                   $over, $acked)
    {
        $this->ip           = $ip;
        $this->what         = $what;
        $this->count        = $count;
        $this->anomaly_time = $anomaly_time;
        $this->range        = $range;
        $this->over         = $over;
        $this->acked        = $acked;
    }

    function get_ip()               { return $this->ip; }
    function get_what()             { return $this->what; }
    function get_count()            { return $this->count; }
    function get_anomaly_time()     { return $this->anomaly_time; }
    function get_range()            { return $this->range; }
    function get_over()             { return $this->over; }
    function get_acked()            { return $this->acked; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM rrd_anomalies $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new RRD_anomaly ( $rs->fields["ip"], 
                                            $rs->fields["what"],
                                            $rs->fields["count"],
                                            $rs->fields["anomaly_time"],
                                            $rs->fields["range"],
                                            $rs->fields["over"],
                                            $rs->fields["acked"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function ack($conn, $ip, $what)
    {
        $query = "UPDATE rrd_anomalies SET acked = 1 where ip = '$ip' and what =
        '$what';";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $ip, $what) {
        $query = "DELETE FROM rrd_anomalies WHERE ip = '$ip' and what = '$what';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
