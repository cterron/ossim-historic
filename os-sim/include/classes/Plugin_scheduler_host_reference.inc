<?php

require_once("ossim_db.inc");

class Plugin_scheduler_host_reference {

    var $plugin_scheduler_id;
    var $host_ip;
    
    function Plugin_scheduler_host_reference ($plugin_scheduler_id, $host_ip)
    {
        $this->plugin_scheduler_id = $plugin_scheduler_id;
        $this->host_ip = $host_ip;
    }

    function get_plugin_scheduler_id()  { return $this->plugin_scheduler_id; }
    function get_host_ip() 	        { return $this->host_ip; }
   
    function get_list($conn, $plugin_scheduler_id) 
    {
        $list = array();
        $plugin_scheduler_id = intval($plugin_scheduler_id);
        $query = OssimQuery("SELECT * FROM plugin_scheduler_host_reference 
                    WHERE plugin_scheduler_id = $plugin_scheduler_id");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Plugin_scheduler_host_reference 
                                    ($rs->fields["plugin_scheduler_id"],
                                     $rs->fields["ip"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_list, $plugin_scheduler_id)
    {
        foreach($host_list as $host_ip)
        {
            $sql = "INSERT INTO plugin_scheduler_host_reference VALUES (?, ?)"; 
            $params = array($plugin_scheduler_id, $host_ip);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
        }
    }

    function in_plugin_scheduler_host_reference($conn, $plugin_scheduler_id, $host_ip)
    {
        $plugin_scheduler_id = intval($plugin_scheduler_id);

        $sql= "SELECT * FROM plugin_scheduler_host_reference WHERE plugin_scheduler_id = ? AND ip = ?";
        $params = array($plugin_scheduler_id, $host_ip);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function delete($conn, $plugin_scheduler_id) {
        $plugin_scheduler_id = intval($plugin_scheduler_id);

        $sql = "DELETE FROM plugin_scheduler_host_reference WHERE plugin_scheduler_id = ?";
        $params = array($plugin_scheduler_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function get_name($conn){
        $sql = "SELECT hostname FROM host WHERE ip = ?";
        if (!$rs = &$conn->Execute($sql, $this->host_ip)) {
                 print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

}

?>
