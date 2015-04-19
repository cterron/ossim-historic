<?php
require_once('classes/Log_action.inc');
require_once ("ossim_db.inc");

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

        $query = OssimQuery("SELECT *, inet_ntoa(host_ip) FROM host_scan $args");

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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

        $sql = "INSERT INTO host_scan VALUES (inet_aton(?), ?, ?)";
        $params = array($host_ip, $plugin_id, $plugin_sid);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        $infolog = array($host_ip . ":". $plugin_id);
        Log_action::log(10,$infolog);
    }
    
    function update($conn, $host, $plugin_id, $plugin_sid = 0)
    {
        settype($plugin_id, "int");
        settype($plugin_sid, "int");

        $sql = "UPDATE host_scan SET inet_ntoa(host) = ?, plugin_id = ?,  plugin_sid = ?";
        $params = array($host, $plugin_id, $plugin_sid);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function delete($conn, $host, $plugin_id) {
        $sql = "DELETE FROM host_scan WHERE inet_ntoa(host_ip) = ? AND plugin_id = ?";
        $params = array($host, $plugin_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function in_host_scan($conn, $host, $plugin_id) {
        $sql = "SELECT * FROM host_scan WHERE inet_ntoa(host_ip) = ? AND plugin_id = ?";
        $params = array($host, $plugin_id);
        
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}


?>
