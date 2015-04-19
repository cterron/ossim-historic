<?php

require_once("ossim_db.inc");

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

    function get_list($conn, $where = "", $order = "", $inf, $sup) 
    {
        $list = array();
        $nanoms = $sup - $inf;
        $query = OssimQuery("SELECT * FROM rrd_anomalies_global $where
        $order LIMIT $nanoms OFFSET $inf");
        if (!$rs = &$conn->Execute($query)) {
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
        $sql = "UPDATE rrd_anomalies_global SET acked = 1 where what = ?";
        $params = array($what);
        if ($conn->Execute($sql, $params) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $what) {
        $sql = "DELETE FROM rrd_anomalies_global WHERE what = ?";
        $params = array($what);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function get_list_count($conn) {
        $sql = "SELECT count(*) FROM rrd_anomalies_global";

        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }
}


?>
