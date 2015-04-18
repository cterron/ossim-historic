<?php

class Policy_port_reference {

    var $policy_id;
    var $port_group_name;
    
    function Policy_port_reference ($policy_id, $port_group_name)
    {
        $this->policy_id        = $policy_id;
        $this->port_group_name  = $port_group_name;
    }

    function get_policy_id()         { return $this->policy_id; }
    function get_port_group_name()   { return $this->port_group_name; }
    
    function get_list($conn, $policy_id) 
    {
        $query = "SELECT * FROM policy_port_reference 
                    WHERE policy_id = $policy_id";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy_port_reference
                                ($rs->fields["policy_id"],
                                 $rs->fields["port_group_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $port_list, $policy_id = "")
    {

        foreach ($port_list as $port_group_name) {
            if ($policy_id != "") {
                $query = "INSERT INTO policy_port_reference 
                            VALUES ($policy_id, '$port_group_name');";
            } else {
                $query = "INSERT INTO policy_port_reference 
                            VALUES (LAST_INSERT_ID(), '$port_group_name');";
            }
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function in_policy_port_reference($conn, $policy_id, $port_group_name)
    {
        $query = "SELECT * FROM policy_port_reference 
                    WHERE policy_id = $policy_id AND 
                          port_group_name = '$port_group_name';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

/*
    function update($conn, $policy_id, $port_group_name)
    {
        $query = "UPDATE policy_port_reference SET policy_id = $policy_id, 
            port_group_name = '$port_group_name' WHERE policy_id = $policy_id 
            AND port_group_name = '$port_group_name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) {
        $query = "DELETE FROM policy_port_reference 
                    WHERE policy_id = $policy_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
