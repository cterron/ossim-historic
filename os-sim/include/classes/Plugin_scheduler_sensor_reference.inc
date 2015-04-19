<?php

require_once("ossim_db.inc");

class Plugin_scheduler_sensor_reference {

    var $plugin_scheduler_id;
    var $sensor_name;
    
    function Plugin_scheduler_sensor_reference ($plugin_scheduler_id, $sensor_name)
    {
        $this->plugin_scheduler_id = $plugin_scheduler_id;
        $this->sensor_name = $sensor_name;
    }

    function get_plugin_scheduler_id()  { return $this->plugin_scheduler_id; }
    function get_sensor_name()     	{ return $this->sensor_name; }
    
    function get_list($conn, $plugin_scheduler_id) 
    {
        $list = array();
        $plugin_scheduler_id = intval($plugin_scheduler_id);
        $query = OssimQuery("SELECT * FROM plugin_scheduler_sensor_reference 
                    WHERE plugin_scheduler_id = $plugin_scheduler_id");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Plugin_scheduler_sensor_reference 
                                    ($rs->fields["plugin_scheduler_id"],
                                     $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $sensor_list, $plugin_scheduler_id)
    {
        foreach($sensor_list as $sensor_name)
        {
            $sql = "INSERT INTO plugin_scheduler_sensor_reference VALUES (?, ?)"; 
            $params = array($plugin_scheduler_id, $sensor_name);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_plugin_scheduler_sensor_reference($conn, $plugin_scheduler_id, $sensor_name)
    {
        $plugin_scheduler_id = intval($plugin_scheduler_id);

        $sql= "SELECT * FROM plugin_scheduler_sensor_reference WHERE plugin_scheduler_id = ? AND sensor_name = ?";
        $params = array($plugin_scheduler_id, $sensor_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $plugin_scheduler_id) {
        $plugin_scheduler_id = intval($plugin_scheduler_id);

        $sql = "DELETE FROM plugin_scheduler_sensor_reference WHERE plugin_scheduler_id = ?";
        $params = array($plugin_scheduler_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
