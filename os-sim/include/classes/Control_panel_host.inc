<?php

require_once ('classes/Host.inc');
require_once ('classes/Conf.inc');

class Control_panel_host {

    var $host_ip;
    var $time_range;
    var $max_c;
    var $max_a;
    var $max_c_date;
    var $max_a_date;
    var $avg_c;
    var $avg_a;

    function Control_panel_host ($host_ip, $time_range, 
                                 $max_c, $max_a, 
                                 $max_c_date, $max_a_date,
                                 $avg_c, $avg_a)
    {
        $this->host_ip    = $host_ip;
        $this->time_range = $time_range;
        $this->max_c      = $max_c;
        $this->max_a      = $max_a;
        $this->max_c_date = $max_c_date;
        $this->max_a_date = $max_a_date;
        $this->avg_c      = $avg_c;
        $this->avg_a      = $avg_a;
    }

    function get_host_ip()    { return $this->host_ip; }
    function get_time_range() { return $this->time_range; }
    function get_max_c()      { return $this->max_c; }
    function get_max_a()      { return $this->max_a; }
    function get_max_c_date() { return $this->max_c_date; }
    function get_max_a_date() { return $this->max_a_date; }
    function get_avg_c()      { return $this->avg_c; }
    function get_avg_a()      { return $this->avg_a; }

    function get_list($conn, $args, $limit)
    {
        $query = "SELECT * FROM control_panel_host $args LIMIT $limit;";
        if (!$rs = &$conn->Execute("$query")) 
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                $list[] = new Control_panel_host ($rs->fields["host_ip"],
                                                  $rs->fields["time_range"],
                                                  $rs->fields["max_c"],
                                                  $rs->fields["max_a"],
                                                  $rs->fields["max_c_date"],
                                                  $rs->fields["max_a_date"],
                                                  $rs->fields["avg_c"],
                                                  $rs->fields["avg_a"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    /* 
     * get a list of hosts that exceed his threshold 
     * ordered by compromise or attack 
     */
    function get_metric_list($conn, $time_range, $type) {

        if ($type == 'compromise') {
            $query = "SELECT * FROM control_panel_host 
                WHERE time_range = '$time_range' AND max_c > 0
                ORDER BY max_c_date DESC";
        } elseif($type == 'attack') {
            $query = "SELECT * FROM control_panel_host 
                WHERE time_range = '$time_range' AND max_a > 0
                ORDER BY max_a_date DESC";
        }
        
        if (!$rs = &$conn->Execute("$query")) 
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                /* get host threshold */
                if (!strcmp($type, 'compromise')) {
                    $threshold = Host::ipthresh_c($conn,
                                                  $rs->fields["host_ip"]);
                } elseif (!strcmp($type, 'attack')) {
                    $threshold = Host::ipthresh_a($conn,
                                                  $rs->fields["host_ip"]);
                } else {
                    /* defensive programming */
                    $conf = Conf::get_conf($conn);
                    $threshold = $conf->get_threshold();
                }

                if (((!strcmp($type, 'compromise')) and 
                        ($rs->fields["max_c"] > $threshold)) ||
                    ((!strcmp($type, 'attack')) and 
                        ($rs->fields["max_a"] > $threshold))) 
                {
                    $list[] = new Control_panel_host ($rs->fields["host_ip"],
                                                      $rs->fields["time_range"],
                                                      $rs->fields["max_c"],
                                                      $rs->fields["max_a"],
                                                      $rs->fields["max_c_date"],
                                                      $rs->fields["max_a_date"],
                                                      $rs->fields["avg_c"],
                                                      $rs->fields["avg_a"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

}
?>
