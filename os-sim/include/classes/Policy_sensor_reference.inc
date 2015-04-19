<?php

require_once("ossim_db.inc");

class Policy_sensor_reference {

    var $policy_id;
    var $sensor_name;
    
    function Policy_sensor_reference ($policy_id, $sensor_name)
    {
        $this->policy_id = $policy_id;
        $this->sensor_name = $sensor_name;
    }

    function get_policy_id()     { return $this->policy_id; }
    function get_sensor_name()     { return $this->sensor_name; }
    
    function get_list($conn, $policy_id) 
    {
        $list = array();
        $policy_id = intval($policy_id);
        $query = OssimQuery("SELECT * FROM policy_sensor_reference 
                    WHERE policy_id = $policy_id");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Policy_sensor_reference 
                                    ($rs->fields["policy_id"],
                                     $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $sensor_list, $policy_id)
    {
        foreach($sensor_list as $sensor_name)
        {
            $sql = "INSERT INTO policy_sensor_reference VALUES (?, ?)"; 
            $params = array($policy_id, $sensor_name);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_policy_sensor_reference($conn, $policy_id, $sensor_name)
    {
        $policy_id = intval($policy_id);

        $sql= "SELECT * FROM policy_sensor_reference WHERE policy_id = ? AND sensor_name = ?";
        $params = array($policy_id, $sensor_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);

        $sql = "DELETE FROM policy_sensor_reference WHERE policy_id = ?";
        $params = array($policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
