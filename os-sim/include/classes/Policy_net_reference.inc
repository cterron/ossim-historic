<?php

require_once("ossim_db.inc");

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
        $list = array();
        $policy_id = intval($policy_id);
        $query = OssimQuery("SELECT * FROM policy_net_reference 
                    WHERE policy_id = $policy_id AND direction = '$direction'");
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

    function insert($conn, $net_list, $direction, $policy_id)
    {
        foreach($net_list as $net_name)
        {
            $sql = "INSERT INTO policy_net_reference VALUES (?, ?, ?)";
            $params = array($policy_id, $net_name, $direction);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_policy_net_reference($conn, $policy_id, $net_name, $direction)
    {
        $policy_id = intval($policy_id);
        $sql = "SELECT * FROM policy_net_reference WHERE policy_id = ? and net_name = ? and direction = ?";
        $params = array($policy_id, $net_name, $direction);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);
        $sql = "DELETE FROM policy_net_reference 
                    WHERE policy_id = ?";
        $params = array($policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
