<?php

require_once("ossim_db.inc");

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
        $list = array();
        $policy_id = intval($policy_id);
        $query = OssimQuery("SELECT * FROM policy_port_reference 
                    WHERE policy_id = $policy_id");
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

    function insert($conn, $port_list, $policy_id)
    {
        foreach ($port_list as $port_group_name) {
            $sql = "INSERT INTO policy_port_reference VALUES (?, ?)";
            $params = array($policy_id, $port_group_name);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_policy_port_reference($conn, $policy_id, $port_group_name)
    {
        $policy_id = intval($policy_id);
        $sql = "SELECT * FROM policy_port_reference 
                    WHERE policy_id = ? AND 
                          port_group_name = ?";
        $params = array($policy_id, $port_group_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);
        $sql = "DELETE FROM policy_port_reference 
                    WHERE policy_id = ?";
        $params = array($policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
