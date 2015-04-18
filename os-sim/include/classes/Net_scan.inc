<?php

require_once ('classes/Conf.inc');
require_once ('classes/Net.inc');

class Net_scan {

    var $net_name;
    var $plugin_id;
    var $plugin_sid;

    function Net_scan ($net_name, $plugin_id, $plugin_sid)
    {
        $this->net_name          = $net_name;
        $this->plugin_id        = $plugin_id;
        $this->plugin_sid       = $plugin_sid;
    }

    function get_net_name()          { return $this->net_name;        }
    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }

    function get_list($conn, $args = "") 
    {

        $query = "SELECT * FROM net_scan $args";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_scan ($rs->fields["net_name"],
                                         $rs->fields["plugin_id"], 
                                         $rs->fields["plugin_sid"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    
    function insert($conn, $net_name, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $query = "INSERT INTO net_scan VALUES 
            ('$net_name', $plugin_id, $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function update($conn, $net, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $query = "UPDATE net_scan SET net = '$net', 
            plugin_id = $plugin_id, plugin_sid = $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function delete($conn, $net, $plugin_id) {
        $query = "DELETE FROM net_scan WHERE net_name = '$net' 
                    AND plugin_id = $plugin_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function in_net_scan($conn, $net, $plugin_id) {
        $query = "SELECT * FROM net_scan WHERE net_name = '$net' AND plugin_id=$plugin_id;";
        echo $query;
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}


?>
