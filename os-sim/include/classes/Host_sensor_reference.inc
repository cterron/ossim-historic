<?php

require_once("ossim_db.inc");

class Host_sensor_reference {

    var $host_ip;
    var $sensor_name;
    
    function Host_sensor_reference ($host_ip, $sensor_name)
    {
        $this->host_ip      = $host_ip;    
        $this->sensor_name  = $sensor_name;
    }

    function get_host_ip()      { return $this->host_ip; }
    function get_sensor_name()  { return $this->sensor_name; }
    
    function get_list($conn, $ip) 
    {
        $sql = "SELECT * FROM host_sensor_reference WHERE host_ip = ?";
        $params = array($ip);

        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Host_sensor_reference 
                                                ($rs->fields["host_ip"],
                                                 $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_ip, $sensors)
    {
        foreach ($sensors as $sensor_name) {
            $sql = "INSERT INTO host_sensor_reference VALUES (?, ?)";
            $params = array($host_ip, $sensor_name);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $host_ip, $sensor_name = "%") 
    {
        $sql = "DELETE FROM host_sensor_reference WHERE host_ip = ? AND sensor_name LIKE ?";
        $params = array($host_ip, $sensor_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_host_sensor_reference($conn, $ip, $sensor_name)
    {
                                                                                
        $sql = "SELECT * FROM host_sensor_reference WHERE host_ip = ? AND sensor_name = ?";
        $params = array($ip, $sensor_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    function check_orphan($conn)
    {
        $sql = "SELECT * FROM host_sensor_reference LEFT JOIN sensor ON
                sensor.name = host_sensor_reference.sensor_name WHERE descr IS NULL";

        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Host_sensor_reference::delete($conn, $rs->fields["host_ip"], $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
    }
}

?>
