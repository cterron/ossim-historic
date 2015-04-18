<?php

class Policy_net_reference {

    var $policy_id;
    var $net_name;
    var $direction;
    
    function Policy_net_reference ($policy_id, $net_name, $direction)
    {
        $this->policy_id = $policy_id;
        $this->net_name = $net_name;
        $this->direction = $direction;
    }

    function get_policy_id()     { return $this->policy_id; }
    function get_net_name()     { return $this->net_name; }
    function get_direction()     { return $this->direction; }
    
    function get_list($conn, $policy_id, $direction) 
    {
        $policy_id = intval($policy_id);
        $query = "SELECT * FROM policy_net_reference 
                    WHERE policy_id = $policy_id AND direction = '$direction'";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy_net_reference ($rs->fields["policy_id"],
                                                     $rs->fields["net_name"],
                                                     $rs->fields["direction"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_list, $direction, $policy_id = "")
    {
        foreach($net_list as $net_name)
        {
            if ($policy_id != "") {
                $query = "INSERT INTO policy_net_reference VALUES 
                    ($policy_id, '$net_name', '$direction');";
            } else {
                $query = "INSERT INTO policy_net_reference VALUES 
                    (LAST_INSERT_ID(), '$net_name', '$direction');";
            }
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function in_policy_net_reference($conn, $policy_id, $net_name, $direction)
    {
        $policy_id = intval($policy_id);
        $query = "SELECT * FROM policy_net_reference 
                    WHERE policy_id = $policy_id and net_name = '$net_name'
                    and direction = '$direction';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

/*
    function update($conn, $policy_id, $net_name, $direction)
    {
        $query = "UPDATE policy_net_reference SET policy_id = policy_id, 
            net_name = '$net_name', direction = $direction WHERE policy_id =
            $policy_id AND net_name = '$net_name' AND direction = $direction;";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);
        $query = "DELETE FROM policy_net_reference 
                    WHERE policy_id = $policy_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
