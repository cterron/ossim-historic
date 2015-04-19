<?php

require_once("ossim_db.inc");

class Control_panel_net {

    var $net_name;
    var $time_range;
    var $max_c;
    var $max_a;
    var $max_c_date;
    var $max_a_date;

    function Control_panel_net ($net_name, $time_range, 
                                $max_c, $max_a, 
                                $max_c_date, $max_a_date)
    {
        $this->net_name   = $net_name;
        $this->time_range = $time_range;
        $this->max_c      = $max_c;
        $this->max_a      = $max_a;
        $this->max_c_date = $max_c_date;
        $this->max_a_date = $max_a_date;
    }

    function get_net_name()   { return $this->net_name; }
    function get_time_range() { return $this->time_range; }
    function get_max_c()      { return $this->max_c; }
    function get_max_a()      { return $this->max_a; }
    function get_max_c_date() { return $this->max_c_date; }
    function get_max_a_date() { return $this->max_a_date; }

    function get_list($conn, $args = "", $limit)
    {
        $count = 0;
        
        $query = OssimQuery("SELECT * FROM control_panel $args");
        
        if (!$rs = &$conn->Execute($query)) 
        {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {

                // only insert in list if net exists in net_qualification
                $query = OssimQuery("SELECT * FROM net_qualification
                                             WHERE net_name = '" .
                                             $rs->fields["id"] . "'");
                if (!$rs2 = &$conn->Execute($query))
                {
                    print $conn->ErrorMsg();
                }
                if (($rs2->fields[0]) && ($count < $limit)) {
                
                    $list[] = new Control_panel_net 
                                        ($rs->fields["id"],
                                         $rs->fields["time_range"],
                                         $rs->fields["max_c"],
                                         $rs->fields["max_a"],
                                         $rs->fields["max_c_date"],
                                         $rs->fields["max_a_date"]);
                    $count++;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $net_name) {
        $sql = "DELETE FROM control_panel WHERE id = ?";
        $params = array($net_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    /* 
     * get a list of nets that exceed his threshold 
     * ordered by compromise or attack 
     */
    function get_metric_list($conn, $time_range, $type)
    {
        if ($type == 'compromise') {
            $sql = "SELECT * FROM control_panel WHERE rrd_type = 'net' AND
                    time_range = ? ORDER BY max_c_date DESC";
            $params = array($time_range);
        } elseif($type == 'attack') {
            $sql = "SELECT * FROM control_panel WHERE rrd_type = 'net' AND
                    time_range = ? ORDER BY max_a_date DESC";
            $params = array($time_range);
        }

        if (!$rs = &$conn->Execute($sql, $params)) 
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                // only insert in list if net exists in net_qualification
                $sql = "SELECT * FROM net_qualification  WHERE net_name = ?";
                $params = array($rs->fields["id"]);
                if (!$rs2 = &$conn->Execute($sql, $params))
                {
                    print $conn->ErrorMsg();
                }
                if ($rs2->fields[0]) {

                    if ($net_list = Net::get_list($conn,
                        "WHERE name = '" . $rs->fields["id"] . "'"))
                    {
                        if (Session::netAllowed($conn, $net_list[0]->get_name()))
                        {

                            $list[] = new Control_panel_net (
                                                $rs->fields["id"],
                                                $rs->fields["time_range"],
                                                $rs->fields["max_c"],
                                                $rs->fields["max_a"],
                                                $rs->fields["max_c_date"],
                                                $rs->fields["max_a_date"]);
                        }
                    }
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

}
?>
