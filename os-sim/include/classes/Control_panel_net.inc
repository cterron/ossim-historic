<?php

class Control_panel_net {

    var $net_name;
    var $time_range;
    var $max_c;
    var $max_a;
    var $max_c_date;
    var $max_a_date;
    var $avg_c;
    var $avg_a;

    function Control_panel_net ($net_name, $time_range, 
                                $max_c, $max_a, 
                                $max_c_date, $max_a_date,
                                $avg_c, $avg_a)
    {
        $this->net_name    = $net_name;
        $this->time_range = $time_range;
        $this->max_c      = $max_c;
        $this->max_a      = $max_a;
        $this->max_c_date = $max_c_date;
        $this->max_a_date = $max_a_date;
        $this->avg_c      = $avg_c;
        $this->avg_a      = $avg_a;
    }

    function get_net_name()    { return $this->net_name; }
    function get_time_range() { return $this->time_range; }
    function get_max_c()      { return $this->max_c; }
    function get_max_a()      { return $this->max_a; }
    function get_max_c_date() { return $this->max_c_date; }
    function get_max_a_date() { return $this->max_a_date; }
    function get_avg_c()      { return $this->avg_c; }
    function get_avg_a()      { return $this->avg_a; }

    function get_list($conn, $args = "", $limit)
    {
        $count = 0;
        
        if (!$rs = &$conn->Execute("SELECT * FROM control_panel_net $args")) 
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                // only insert in list if net exists in net_qualification
                if (!$rs2 = &$conn->Execute("SELECT * FROM net_qualification
                                             WHERE net_name = '" .
                                             $rs->fields["net_name"] . "';"))
                {
                    print $conn->ErrorMsg();
                }
                if (($rs2->fields[0]) && ($count < $limit)) {
                
                    $list[] = new Control_panel_net 
                                        ($rs->fields["net_name"],
                                         $rs->fields["time_range"],
                                         $rs->fields["max_c"],
                                         $rs->fields["max_a"],
                                         $rs->fields["max_c_date"],
                                         $rs->fields["max_a_date"],
                                         $rs->fields["avg_c"],
                                         $rs->fields["avg_a"]);
                    $count++;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $net_name) {
        $query = "DELETE FROM control_panel_net WHERE net_name = '$net_name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    /* 
     * get a list of nets that exceed his threshold 
     * ordered by compromise or attack 
     */
    function get_metric_list($conn, $time_range, $type) {

        if ($type == 'compromise') {
            $query = "SELECT * FROM control_panel_net 
                WHERE time_range = '$time_range' 
                ORDER BY max_c_date DESC LIMIT 50";
        } elseif($type == 'attack') {
            $query = "SELECT * FROM control_panel_net 
                WHERE time_range = '$time_range' 
                ORDER BY max_a_date DESC LIMIT 50";
        }
        
        if (!$rs = &$conn->Execute("$query")) 
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            
                // only insert in list if net exists in net_qualification
                if (!$rs2 = &$conn->Execute("SELECT * FROM net_qualification
                                             WHERE net_name = '" .
                                             $rs->fields["net_name"] . "';"))
                {
                    print $conn->ErrorMsg();
                }
                if ($rs2->fields[0]) {
                
                    $list[] = new Control_panel_net ($rs->fields["net_name"],
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
