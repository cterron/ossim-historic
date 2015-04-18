<?php

require_once ('Policy_host_reference.inc');
require_once ('Policy_net_reference.inc');
require_once ('Policy_sensor_reference.inc');
require_once ('Policy_port_reference.inc');
require_once ('Policy_sig_reference.inc');

class Policy {

    var $id;
    var $priority;
    var $port_group_name;
    var $sig_group_name;
    var $descr;

    function Policy ($id, $priority, $descr)
    {
        $this->id               = $id;
        $this->priority         = $priority;
        $this->descr            = $descr;
    }

    function get_id()              { return $this->id; }
    function get_priority()        { return $this->priority; }
    function get_descr()           { return $this->descr; }


    function get_hosts($conn, $direction) {
        return Policy_host_reference::get_list($conn, $this->id, $direction);
    }

    function get_nets($conn, $direction) {
        return Policy_net_reference::get_list($conn, $this->id, $direction);
    }

    function get_sensors($conn) {
        return Policy_sensor_reference::get_list($conn, $this->id);
    }

    function get_ports($conn) {
        return Policy_port_reference::get_list($conn, $this->id);
    }

    function get_signatures($conn) {
        return Policy_sig_reference::get_list($conn, $this->id);
    }


    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM policy $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy ($rs->fields["id"],
                                      $rs->fields["priority"],
                                      $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, 
                    $priority, $descr,
                    $host_source_list, $host_dest_list,
                    $net_source_list, $net_dest_list, 
                    $port_list, $sig_list, $sensor_list)
    {
        $priority = intval($priority);

        $query = "INSERT INTO policy VALUES 
          (NULL, $priority, '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        if ($host_source_list)
            Policy_host_reference::insert($conn, $host_source_list, 'source');
        if ($host_dest_list)
            Policy_host_reference::insert($conn, $host_dest_list, 'dest');
        if ($net_source_list)
            Policy_net_reference::insert($conn, $net_source_list, 'source');
        if ($net_dest_list)
            Policy_net_reference::insert($conn, $net_dest_list, 'dest');
        if ($port_list)
            Policy_port_reference::insert($conn, $port_list);
        if ($sig_list)
            Policy_sig_reference::insert($conn, $sig_list);
        if ($sensor_list)
            Policy_sensor_reference::insert($conn, $sensor_list);
    }

    function update($conn, 
                    $id, $priority, $descr,
                    $host_source_list, $host_dest_list,
                    $net_source_list, $net_dest_list,
                    $port_list, $sig_list, $sensor_list)
    {
        $id = intval($id);


        Policy::delete($conn, $id);
        Policy::insert($conn, $priority, $descr,
                       $host_source_list, $host_dest_list,
                       $net_source_list, $net_dest_list,
                       $port_list, $sig_list, $sensor_list);
    }

    function delete($conn, $id) {
        $id = intval($id);

        $query = "DELETE FROM policy WHERE id = $id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        Policy_host_reference::delete($conn, $id);
        Policy_net_reference::delete($conn, $id);
        Policy_port_reference::delete($conn, $id);
        Policy_sig_reference::delete($conn, $id);
        Policy_sensor_reference::delete($conn, $id);
    }
}

?>

