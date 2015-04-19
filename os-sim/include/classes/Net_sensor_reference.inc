<?php

require_once("ossim_db.inc");

class Net_sensor_reference {

    var $net_name;
    var $sensor_name;
    
    function Net_sensor_reference ($net_name, $sensor_name)
    {
        $this->net_name      = $net_name;    
        $this->sensor_name  = $sensor_name;
    }

    function get_net_name()      { return $this->net_name; }
    function get_sensor_name()  { return $this->sensor_name; }
    
    function get_list($conn, $net) 
    {
        $query = OssimQuery("SELECT * FROM net_sensor_reference WHERE net_name
        = '$net'");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Net_sensor_reference 
                                                ($rs->fields["net_name"],
                                                 $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_name, $sensors)
    {
        foreach ($sensors as $sensor_name) {
            $sql = "INSERT INTO net_sensor_reference VALUES (?, ?)";
            $params = array($net_name, $sensor_name);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

   function delete($conn, $net_name, $sensor_name = "%") 
    {
        $sql = "DELETE FROM net_sensor_reference WHERE net_name = ? AND sensor_name LIKE ?";
        $params = array($net_name, $sensor_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_net_sensor_reference($conn, $net, $sensor_name)
    {
        $sql = "SELECT * FROM net_sensor_reference WHERE net_name = ? AND sensor_name = ?";
        $params = array($net, $sensor_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    function check_orphan($conn)
    {
        $sql = "SELECT * FROM net_sensor_reference LEFT JOIN sensor ON sensor.name
                = net_sensor_reference.sensor_name WHERE name IS NULL";

        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else { 
            while (!$rs->EOF) {
                Net_sensor_reference::delete($conn, $rs->fields["net_name"], $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }        
        }
    }
}

?>
