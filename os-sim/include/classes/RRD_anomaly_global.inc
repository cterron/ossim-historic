<?php

class RRD_anomaly_global {

    var $what;
    var $count;
    var $anomaly_time;
    var $range;
    var $over;
    var $acked;

    function RRD_anomaly_global ($what, $count, $anomaly_time, $range,
                   $over, $acked)
    {
        $this->what         = $what;
        $this->count        = $count;
        $this->anomaly_time = $anomaly_time;
        $this->range        = $range;
        $this->over         = $over;
        $this->acked        = $acked;
    }

    function get_what()             { return $this->what; }
    function get_count()            { return $this->count; }
    function get_anomaly_time()     { return $this->anomaly_time; }
    function get_range()            { return $this->range; }
    function get_over()             { return $this->over; }
    function get_acked()            { return $this->acked; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM rrd_anomalies_global $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new RRD_anomaly_global ( $rs->fields["what"],
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

    function ack($conn, $what)
    {
        $query = "UPDATE rrd_anomalies_global SET acked = 1 where what = '$what';";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $what) {
        $query = "DELETE FROM rrd_anomalies_global WHERE what = '$what';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
