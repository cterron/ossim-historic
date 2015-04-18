<?php

require_once ('classes/Conf.inc');

class Host_scan {

    var $host_ip;
    var $plugin_id;
    var $plugin_sid;

    function Host_scan ($host_ip, $plugin_id, $plugin_sid)
    {
        $this->host_ip          = $host_ip;
        $this->plugin_id        = $plugin_id;
        $this->plugin_sid       = $plugin_sid;
    }

    function get_host_ip()          { return $this->host_ip;        }
    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }

    function get_list($conn, $args = "") 
    {

        $query = "SELECT *, inet_ntoa(host_ip) FROM host_scan $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_scan ($rs->fields["inet_ntoa(host_ip)"],
                                         $rs->fields["plugin_id"], 
                                         $rs->fields["plugin_sid"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    
    function insert($conn, $host_ip, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $query = "INSERT INTO host_scan VALUES 
            (inet_aton('$host_ip'), $plugin_id, $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function update($conn, $host, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $query = "UPDATE host_scan SET inet_ntoa(host) = '$host', 
            plugin_id = $plugin_id, plugin_sid = $plugin_sid);";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function delete($conn, $host, $plugin_id) {
        $query = "DELETE FROM host_scan WHERE inet_ntoa(host_ip) = '$host' 
                    AND plugin_id = $plugin_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
