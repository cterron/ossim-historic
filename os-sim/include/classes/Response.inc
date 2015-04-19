<?php

require_once ('classes/Response_host.inc');
require_once ('classes/Response_net.inc');
require_once ('classes/Response_port.inc');
require_once ('classes/Response_plugin.inc');
require_once ('classes/Response_action.inc');
require_once ("ossim_db.inc");

class Response {

    var $id;
    var $descr;

    function Response ($id, $descr)
    {
        $this->id          = $id;
        $this->descr       = htmlentities($descr);
    }

    function get_id() {
        return $this->id;
    }
    function get_descr() {
        return $this->descr;
    }
    function get_source_hosts($conn) {
        return Response_host::get_list($conn, 
            "WHERE _type = 'source' AND response_id = '".$this->id."'");
    }
    function get_dest_hosts($conn) {
        return Response_host::get_list($conn, 
            "WHERE _type = 'dest' AND response_id = '".$this->id."'");
    }
    function get_sensors($conn) {
        return Response_host::get_list($conn, 
            "WHERE _type = 'sensor' AND response_id = '".$this->id."'");
    }
    function get_source_nets($conn) {
        return Response_net::get_list($conn, 
            "WHERE _type = 'source' AND response_id = '".$this->id."'");
    }
    function get_dest_nets($conn) {
        return Response_net::get_list($conn, 
            "WHERE _type = 'dest' AND response_id = '".$this->id."'");
    }
    function get_source_ports($conn) {
        return Response_port::get_list($conn, 
            "WHERE _type = 'source' AND response_id = '".$this->id."'");
    }
    function get_dest_ports($conn) {
        return Response_port::get_list($conn, 
            "WHERE _type = 'dest' AND response_id = '".$this->id."'");
    }
    function get_plugins($conn) {
        return Response_plugin::get_list($conn,
            "WHERE response_id = '".$this->id."'");
    }
    function get_actions($conn) {
        return Response_action::get_list($conn,
            "WHERE response_id = '".$this->id."'");
    }

    function get_list($conn, $args = "") 
    {
    
    $query = OssimQuery("SELECT * FROM response $args");
    if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Response ($rs->fields["id"],
                                        $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $descr, $source_nets, $source_hosts, $dest_nets,
                    $dest_hosts, $sensors, $source_ports, $dest_ports, 
                    $plugins, $actions)
    {
       
        $sql = "INSERT INTO response (id, descr) VALUES  (NULL, ?)";
        $params = array($descr);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        foreach ($source_hosts as $host)
            Response_host::insert($conn, $host, "source");
        foreach ($dest_hosts as $host)
            Response_host::insert($conn, $host, "dest");
        foreach ($sensors as $sensor)
            Response_host::insert($conn, $sensor, "sensor");
        foreach ($source_nets as $net)
            Response_net::insert($conn, $net, "source");
        foreach ($dest_nets as $net)
            Response_net::insert($conn, $net, "dest");
        foreach ($source_ports as $port)
            Response_port::insert($conn, $port, "source");
        foreach ($dest_ports as $port)
            Response_port::insert($conn, $port, "dest");
        foreach ($plugins as $plugin)
            Response_plugin::insert($conn, $plugin);
        foreach ($actions as $action)
            Response_action::insert($conn, $action);
    }

    function delete($conn, $id)
    {
        $sql  = "DELETE FROM response WHERE id = ?";
        $params = array($id);
        if ($conn->Execute($sql, $params) === false) {
            print "error deleting ($sql): ".$conn->ErrorMsg().'<BR>';
            exit;
        }

        $response_tables = array (
            "response_action",
            "response_host",
            "response_net",
            "response_plugin",
            "response_port"
        );

        foreach ($response_tables as $table) {

            $sql = "DELETE FROM $table WHERE response_id = ?";
            $params = array($id);
            if ($conn->Execute($sql, $params) === false) {
                echo "error deleting ($sql): ".$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

}

?>

