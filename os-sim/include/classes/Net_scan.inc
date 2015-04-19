<?php

require_once ('classes/Net.inc');
require_once ("ossim_db.inc");

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

        $query = OssimQuery("SELECT * FROM net_scan $args");

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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

        $sql = "INSERT INTO net_scan VALUES (?, ?, ?)";
        $params = array($net_name, $plugin_id, $plugin_sid);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function update($conn, $net, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $sql = "UPDATE net_scan SET net = ?, plugin_id = ?, plugin_sid = ?";
        $params = array($net, $plugin_id, $plugin_sid);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function delete($conn, $net, $plugin_id) {
        $sql = "DELETE FROM net_scan WHERE net_name = ? AND plugin_id = ?";
        $params = array($net, $plugin_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function in_net_scan($conn, $net, $plugin_id) {
        $sql = "SELECT * FROM net_scan WHERE net_name = ? AND plugin_id = ?";
        $params = array($net, $plugin_id);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}


?>
