<?php

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
        $policy_id = intval($policy_id);
        $query = "SELECT * FROM policy_sensor_reference 
                    WHERE policy_id = $policy_id;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy_sensor_reference 
                                    ($rs->fields["policy_id"],
                                     $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $sensor_list, $policy_id = "")
    {
        foreach($sensor_list as $sensor_name)
        {
            if ($policy_id != "") {
                $query = "INSERT INTO policy_sensor_reference VALUES 
                    ($policy_id, '$sensor_name');";
            } else {
                $query = "INSERT INTO policy_sensor_reference VALUES 
                    (LAST_INSERT_ID(), '$sensor_name');";
            }
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function in_policy_sensor_reference($conn, $policy_id, $sensor_name)
    {
        $policy_id = intval($policy_id);

        $query = "SELECT * FROM policy_sensor_reference 
                    WHERE policy_id = $policy_id AND 
                          sensor_name='$sensor_name';";
// Debug ? //       echo $query;
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

/*
    function update($conn, $policy_id, $sensor_name)
    {
        $query = "UPDATE policy_sensor_reference SET policy_id = policy_id, 
            sensor_name = '$sensor_name'  WHERE policy_id =
            $policy_id AND sensor_name = '$sensor_name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/

    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);

        $query = "DELETE FROM policy_sensor_reference 
                    WHERE policy_id = $policy_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
