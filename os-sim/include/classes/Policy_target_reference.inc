<?php

require_once("ossim_db.inc");

class Policy_target_reference {

    var $policy_id;
    var $target_name;
    
    function Policy_target_reference ($policy_id, $target_name)
    {
        $this->policy_id = $policy_id;
        $this->target_name = $target_name;
    }

    function get_policy_id()     { return $this->policy_id; }
    function get_target_name()     { return $this->target_name; }
    
    function get_list($conn, $policy_id) 
    {
        $list = array();
        $policy_id = intval($policy_id);
        $query = OssimQuery("SELECT * FROM policy_target_reference 
                    WHERE policy_id = $policy_id");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Policy_target_reference 
                                    ($rs->fields["policy_id"],
                                     $rs->fields["target_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $target_list, $policy_id)
    {
        foreach($target_list as $target_name)
        {
            $sql = "INSERT INTO policy_target_reference VALUES (?, ?)"; 
            $params = array($policy_id, $target_name);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_policy_target_reference($conn, $policy_id, $target_name)
    {
        $policy_id = intval($policy_id);

        $sql= "SELECT * FROM policy_target_reference WHERE policy_id = ? AND target_name = ?";
        $params = array($policy_id, $target_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);

        $sql = "DELETE FROM policy_target_reference WHERE policy_id = ?";
        $params = array($policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
