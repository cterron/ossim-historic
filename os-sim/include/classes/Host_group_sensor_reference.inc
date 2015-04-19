<?php

require_once("ossim_db.inc");

class Host_group_sensor_reference {

    var $group_name;
    var $sensor_name;
    
    function Host_group_sensor_reference ($group_name, $sensor_name)
    {
        $this->group_name   = $group_name;    
        $this->sensor_name  = $sensor_name;
    }

    function get_group_name()      { return $this->group_name; }
    function get_sensor_name()  { return $this->sensor_name; }
    
    function get_list($conn, $group_name) 
    {
        $sql = "SELECT * FROM host_group_sensor_reference WHERE group_name = ?";
        $params = array($group_name);
        $list = array();

        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Host_group_sensor_reference 
                                                ($rs->fields["group_name"],
                                                 $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $group_name, $sensors)
    {
        foreach ($sensors as $sensor_name) {
            $sql = "INSERT INTO host_group_sensor_reference VALUES (?, ?)";
            $params = array($group_name, $sensor_name);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $group_name, $sensor_name = "%") 
    {
        $sql = "DELETE FROM host_group_sensor_reference WHERE group_name = ? AND sensor_name LIKE ?";
        $params = array($group_name, $sensor_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_host_group_sensor_reference($conn, $group_name, $sensor_name)
    {
                                                                                
        $sql = "SELECT * FROM host_group_sensor_reference WHERE group_name = ? AND sensor_name = ?";
        $params = array($group_name, $sensor_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    function check_orphan($conn)
    {
        $sql = "SELECT * FROM host_group_sensor_reference LEFT JOIN sensor ON
                sensor.name = host_group_sensor_reference.sensor_name WHERE descr IS NULL";

        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Host_sensor_reference::delete($conn, $rs->fields["group_name"], $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
    }
}

?>
