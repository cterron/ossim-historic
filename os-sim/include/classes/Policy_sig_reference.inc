<?php

class Policy_sig_reference {

    var $policy_id;
    var $sig_group_name;
    
    function Policy_sig_reference ($policy_id, $sig_group_name)
    {
        $this->policy_id        = $policy_id;
        $this->sig_group_name  = $sig_group_name;
    }

    function get_policy_id()         { return $this->policy_id; }
    function get_sig_group_name()   { return $this->sig_group_name; }
    
    function get_list($conn, $policy_id) 
    {
        $query = "SELECT * FROM policy_sig_reference 
                    WHERE policy_id = $policy_id";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy_sig_reference
                                ($rs->fields["policy_id"],
                                 $rs->fields["sig_group_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function in_policy_sig_reference($conn, $policy_id, $sig_group_name)
    {
        $query = "SELECT * FROM policy_sig_reference 
                    WHERE policy_id = $policy_id AND 
                          sig_group_name='$sig_group_name';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function insert($conn, $sig_list, $policy_id = "")
    {

        foreach ($sig_list as $sig_group_name) {
            if ($policy_id != "") {
                $query = "INSERT INTO policy_sig_reference 
                            VALUES ($policy_id, '$sig_group_name');";
            } else {
                $query = "INSERT INTO policy_sig_reference 
                            VALUES (LAST_INSERT_ID(), '$sig_group_name');";
            }
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }
/*
    function update($conn, $policy_id, $sig_group_name)
    {
        $query = "UPDATE policy_sig_reference SET policy_id = $policy_id, 
            sig_group_name = '$sig_group_name' WHERE policy_id = $policy_id 
            AND sig_group_name = '$sig_group_name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) {
        $query = "DELETE FROM policy_sig_reference 
                    WHERE policy_id = $policy_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
