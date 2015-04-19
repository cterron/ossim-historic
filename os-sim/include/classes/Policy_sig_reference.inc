<?php

require_once("ossim_db.inc");

/*
 * @deprecated class/file
 */
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
        $policy_id = intval($policy_id);

        $sql = "SELECT * FROM policy_sig_reference WHERE policy_id = ?";
        $params = array($policy_id);
        
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        $policy_id = intval($policy_id);

        $sql = "SELECT * FROM policy_sig_reference WHERE policy_id = ? AND sig_group_name= ?";
        
        $params = array($policy_id, $sig_group_name);
        
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        
        return $rs->fields[0];
    }

    function insert($conn, $sig_list, $policy_id = "")
    {

        foreach ($sig_list as $sig_group_name) {
            if ($policy_id != "") {
                $sql = "INSERT INTO policy_sig_reference VALUES (?, ?)";
                $params = array($policy_id, $sig_group_name);
            } else {
                $sql = "INSERT INTO policy_sig_reference VALUES (LAST_INSERT_ID(), ?)";
                $params = array($sig_group_name);
            }
            if ($conn->Execute($sql, $params) === false) {
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
            AND sig_group_name = '$sig_group_name'";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);

        $query = OssimQuery("DELETE FROM policy_sig_reference 
                    WHERE policy_id = $policy_id");
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
