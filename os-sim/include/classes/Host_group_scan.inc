<?php

require_once ('classes/Host_group.inc');
require_once ("ossim_db.inc");


class Host_group_scan {

    var $host_group_name;
    var $plugin_id;
    var $plugin_sid;

    function Host_group_scan ($host_group_name, $plugin_id, $plugin_sid)
    {
        $this->host_group_name          = $host_group_name;
        $this->plugin_id        = $plugin_id;
        $this->plugin_sid       = $plugin_sid;
    }

    function get_host_group_name()          { return $this->host_group_name;        }
    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }
    function get_name()              { return $this->host_group_name;        } 

    function get_list($conn, $args = "")
    {
        $query = OssimQuery("SELECT * FROM host_group_scan $args");
        $list = array();
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_group_scan ($rs->fields["host_group_name"],
                                         $rs->fields["plugin_id"],
                                         $rs->fields["plugin_sid"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_group_name, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $sql = "INSERT INTO host_group_scan VALUES (?, ?, ?)";

        $params = array($host_group_name, $plugin_id, $plugin_sid);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $host, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $sql = "UPDATE host_group_scan SET host = ?, plugin_id = ?, plugin_sid = ?";
        $params = array($host, $plugin_id, $plugin_sid);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $host, $plugin_id) {
        $sql = "DELETE FROM host_group_scan WHERE host_group_name = ? AND plugin_id = ?";
        $params = array($host, $plugin_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_host_group_scan($conn, $host, $plugin_id) {
        $sql = "SELECT * FROM host_group_scan WHERE host_group_name = ? AND plugin_id = ?";
        $params = array($host, $plugin_id);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}


?>
