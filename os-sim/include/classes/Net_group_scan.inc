<?php

require_once ('classes/Net_group.inc');

class Net_group_scan {

    var $net_group_name;
    var $plugin_id;
    var $plugin_sid;

    function Net_group_scan ($net_group_name, $plugin_id, $plugin_sid)
    {
        $this->net_group_name          = $net_group_name;
        $this->plugin_id        = $plugin_id;
        $this->plugin_sid       = $plugin_sid;
    }

    function get_net_group_name()          { return $this->net_group_name;        }
    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM net_group_scan $args";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_group_scan ($rs->fields["net_group_name"],
                                         $rs->fields["plugin_id"], 
                                         $rs->fields["plugin_sid"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    
    function insert($conn, $net_group_name, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");
        $net_group_name = mysql_real_escape_string($net_group_name);

        $query = "INSERT INTO net_group_scan VALUES 
            ('$net_group_name', $plugin_id, $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function update($conn, $net, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");
        $net_group_name = mysql_real_escape_string($net_group_name);

        $query = "UPDATE net_group_scan SET net = '$net', 
            plugin_id = $plugin_id, plugin_sid = $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function delete($conn, $net, $plugin_id) {
        $net = mysql_real_escape_string($net);
        $query = "DELETE FROM net_group_scan WHERE net_group_name = '$net' 
                    AND plugin_id = $plugin_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function in_net_group_scan($conn, $net, $plugin_id) {
        $net = mysql_real_escape_string($net);
        $query = "SELECT * FROM net_group_scan WHERE net_group_name = '$net' AND plugin_id=$plugin_id;";
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}


?>
