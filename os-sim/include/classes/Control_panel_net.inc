<?php

class Control_panel_net {

    var $net_name;
    var $time_range;
    var $max_c;
    var $max_a;
    var $min_c;
    var $min_a;
    var $avg_c;
    var $avg_a;

    function Control_panel_net ($net_name, $time_range, 
                                 $max_c, $max_a, $min_c, $min_a, 
                                 $avg_c, $avg_a)
    {
        $this->net_name    = $net_name;
        $this->time_range = $time_range;
        $this->max_c      = $max_c;
        $this->max_a      = $max_a;
        $this->min_c      = $min_c;
        $this->min_a      = $min_a;
        $this->avg_c      = $avg_c;
        $this->avg_a      = $avg_a;
    }

    function get_net_name()    { return $this->net_name; }
    function get_time_range() { return $this->time_range; }
    function get_max_c()      { return $this->max_c; }
    function get_max_a()      { return $this->max_a; }
    function get_min_c()      { return $this->min_c; }
    function get_min_a()      { return $this->min_a; }
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
                                         $rs->fields["min_c"],
                                         $rs->fields["min_a"],
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

}
?>
