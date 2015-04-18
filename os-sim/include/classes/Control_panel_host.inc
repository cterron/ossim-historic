<?php

class Control_panel_host {

    var $host_ip;
    var $time_range;
    var $max_c;
    var $max_a;
    var $avg_c;
    var $avg_a;

    function Control_panel_host ($host_ip, $time_range, 
                                 $max_c, $max_a, $avg_c, $avg_a)
    {
        $this->host_ip    = $host_ip;
        $this->time_range = $time_range;
        $this->max_c      = $max_c;
        $this->max_a      = $max_a;
        $this->avg_c      = $avg_c;
        $this->avg_a      = $avg_a;
    }

    function get_host_ip()    { return $this->host_ip; }
    function get_time_range() { return $this->time_range; }
    function get_max_c()      { return $this->max_c; }
    function get_max_a()      { return $this->max_a; }
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
                                                  $rs->fields["avg_c"],
                                                  $rs->fields["avg_a"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

}
?>
