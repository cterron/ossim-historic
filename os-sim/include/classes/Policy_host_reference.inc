<?php

class Policy_host_reference {

    var $policy_id;
    var $host_ip;
    var $direction;
    
    function Policy_host_reference ($policy_id, $host_ip, $direction)
    {
        $this->policy_id = $policy_id;
        $this->host_ip = $host_ip;
        $this->direction = $direction;
    }

    function get_policy_id()     { return $this->policy_id; }
    function get_host_ip()     { return $this->host_ip; }
    function get_direction()     { return $this->direction; }
    
    function get_list($conn, $policy_id, $direction) 
    {
        $policy_id = intval($policy_id);
        $query = "SELECT * FROM policy_host_reference 
                    WHERE policy_id = $policy_id 
                    AND direction = '$direction';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy_host_reference ($rs->fields["policy_id"],
                                                     $rs->fields["host_ip"],
                                                     $rs->fields["direction"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_list, $direction, $policy_id = "")
    {
        foreach($host_list as $host_ip)
        {
            if ($policy_id != "") {
                $query = "INSERT INTO policy_host_reference VALUES 
                    ($policy_id, '$host_ip', '$direction');";
            } else {
                $query = "INSERT INTO policy_host_reference VALUES 
                    (LAST_INSERT_ID(), '$host_ip', '$direction');";
            }
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function in_policy_host_reference($conn, $policy_id, $host_ip, $direction)
    {
        $policy_id = intval($policy_id);
        $query = "SELECT * FROM policy_host_reference 
                    WHERE policy_id = $policy_id and host_ip = '$host_ip' and
                    direction = '$direction';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

/*
    function update($conn, $policy_id, $host_ip, $direction)
    {
        $query = "UPDATE policy_host_reference SET policy_id = policy_id, 
            host_ip = '$host_ip', direction = $direction WHERE policy_id =
            $policy_id AND host_ip = '$host_ip' AND direction = $direction;";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) 
    {
        $policy_id = intval($policy_id);
        $query = "DELETE FROM policy_host_reference 
                    WHERE policy_id = $policy_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
