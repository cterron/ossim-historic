<?php

class Host_position {

    var $host_ip;
    var $x;
    var $y;
    var $z;

    function Host_position ($host_ip, $x, $y, $z)
    {
        $this->host_ip = $host_ip;
        $this->x       = $x;
        $this->y       = $y;
        $this->z       = $z;
    }

    function get_host_ip() { return $this->host_ip; }
    function get_x()       { return $this->x; }
    function get_y()       { return $this->y; }
    function get_z()       { return $this->z; }

    function get_list($conn, $where = "", $order = "") 
    {
        $query = "SELECT * FROM host_position $where $order";
        if (!$rs = &$conn->Execute("$query")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_position ($rs->fields["host_ip"], 
                                             $rs->fields["x"],
                                             $rs->fields["y"],
                                             $rs->fields["z"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $x, $y, $z)
    {
        $query = "INSERT INTO host_position VALUES ('$ip', $x, $y, $z);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $ip, $x, $y, $z)
    {
        $query = "UPDATE host_position SET host_ip = '$ip', 
                    x = $x, y = $y, z = $z WHERE host_ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $ip) {
        $query = "DELETE FROM host_position WHERE host_ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
