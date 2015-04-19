<?php

require_once 'classes/Policy_host_reference.inc';
require_once 'classes/Policy_net_reference.inc';
require_once 'classes/Policy_sensor_reference.inc';
require_once 'classes/Policy_port_reference.inc';
require_once 'classes/Policy_time.inc';
require_once 'ossim_db.inc';

class Policy {

    var $id;
    var $priority;
    var $port_group_name;
    var $descr;
    var $store;

    function Policy ($id, $priority, $descr, $store)
    {
        $this->id          = $id;
        $this->priority    = $priority;
        $this->descr       = htmlentities($descr);
        $this->store       = $store;
    }

    function get_id()              { return $this->id; }
    function get_priority()        { return $this->priority; }
    function get_descr()           { return $this->descr; }
    function get_store()           { return $this->store; }


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

    /*
     * @deprecated
     */
    function get_signatures($conn) {
        return Policy_sig_reference::get_list($conn, $this->id);
    }

    function get_plugingroups($conn, $policy_id, $get_all_groups = false)
    {
        $list = array();
        if (!$get_all_groups) {
            $where = "WHERE policy_plugin_group_reference.policy_id = ?";
            $params = array($policy_id);
        } else {
            $where = "";
            $params = array();
        }
        $sql = "SELECT
                    plugin_group_descr.group_id,
                    plugin_group_descr.name,
                    policy_plugin_group_reference.policy_id
                FROM
                    plugin_group_descr
                LEFT JOIN policy_plugin_group_reference ON
                    plugin_group_descr.group_id = policy_plugin_group_reference.group_id
                $where
                ORDER BY
                    plugin_group_descr.name";
        if (!$rs = $conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            $list[] = array(
                            'id'   => $rs->fields['group_id'],
                            'name' => $rs->fields['name'],
                            'policy_id'  => $rs->fields['policy_id']
                            );
            $rs->MoveNext();
        }
        return $list;
    }

    function get_time($conn) {
        return Policy_time::get_list($conn, $this->id);
    }


    function get_list($conn, $args = "") 
    {
        $list = array();
        $query = OssimQuery("SELECT * FROM policy $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Policy ($rs->fields["id"],
                                      $rs->fields["priority"],
                                      $rs->fields["descr"],
                                      $rs->fields["store"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, 
                    $priority, $begin_hour, $end_hour, $begin_day, $end_day, 
                    $descr, $host_source_list, $host_dest_list,
                    $net_source_list, $net_dest_list, 
                    $port_list, $plug_groups, $sensor_list, $store, $id = null)
    {
        $descr = htmlentities($descr);
        $priority = intval($priority);
        $store = intval($store);
        if (!$id) {
            $id = $conn->GenID('policy_seq');
        }
        
        $sql = "INSERT INTO policy VALUES (?, ?, ?, ?)";
        $params = array($id, $priority, $descr, $store);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        
        Policy_host_reference::insert($conn, $host_source_list, 'source', $id);
        Policy_host_reference::insert($conn, $host_dest_list, 'dest', $id);
        Policy_net_reference::insert($conn, $net_source_list, 'source', $id);
        Policy_net_reference::insert($conn, $net_dest_list, 'dest', $id);
        Policy_port_reference::insert($conn, $port_list, $id);
            
        $conn->Execute("DELETE FROM policy_plugin_group_reference WHERE policy_id=$id");
        $sql = "INSERT INTO policy_plugin_group_reference
                (policy_id, group_id) VALUES (?, ?)";
        foreach ($plug_groups as $group_id) {
            $conn->Execute($sql, array($id, $group_id));
        }
        
        Policy_sensor_reference::insert($conn, $sensor_list, $id);
        Policy_time::insert($conn, 
                            $begin_hour, $end_hour, $begin_day, $end_day, $id);
    }

    function update($conn, 
                    $id, $priority, $begin_hour, $end_hour, 
                    $begin_day, $end_day, $descr,
                    $host_source_list, $host_dest_list,
                    $net_source_list, $net_dest_list,
                    $port_list, $plug_groups, $sensor_list, $store)
    {
        $descr = htmlentities($descr);
        $id = intval($id);
        $store = intval($store);

        Policy::delete($conn, $id);
        Policy::insert($conn, $priority, $begin_hour, $end_hour, 
                       $begin_day, $end_day, $descr,
                       $host_source_list, $host_dest_list,
                       $net_source_list, $net_dest_list,
                       $port_list, $plug_groups, $sensor_list, $store, $id);
    }

    function delete($conn, $id)
    {
        $id = intval($id);

        Policy_host_reference::delete($conn, $id);
        Policy_net_reference::delete($conn, $id);
        Policy_port_reference::delete($conn, $id);
        Policy_sensor_reference::delete($conn, $id);
        Policy_time::delete($conn, $id);
        
        
        $sql = "DELETE FROM policy_plugin_group_reference WHERE policy_id = ?";
        $params = array($id);
        
        $conn->Execute($sql, $params);
        
        $sql = "DELETE FROM policy WHERE id = ?";
        $conn->Execute($sql, $params);

    }
}

?>

