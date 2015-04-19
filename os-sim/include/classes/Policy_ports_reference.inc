<?php

require_once("ossim_db.inc");

/*
 * @deprecated class/file
 */
class Policy_ports_reference {

    var $policy_id;
    var $port_number;
    var $port_protocol;
    
    function Policy_ports_reference ($policy_id, $port_number, $port_protocol)
    {
        $this->policy_id        = $policy_id;
        $this->port_number      = $port_number;
        $this->port_protocol    = $port_protocol;    
    }

    function get_policy_id()         { return $this->policy_id; }
    function get_port_number()       { return $this->port_number; }
    function get_port_protocol()     { return $this->port_protocol; }
    
    function get_list($conn, $where = "", $order = "") 
    {
        $query = OssimQuery("SELECT * FROM policy_ports_reference $where
        $order");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Policy_ports_reference ($rs->fields["policy_id"], 
                                                      $rs->fields["port_number"],
                                                      $rs->fields["port_protocol"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $policy_id, $port_number, $port_protocol)
    {
        $policy_id = intval($policy_id);
        $port_number = intval($port_number);
        $sql = "INSERT INTO policy_ports_reference VALUES (?, ?, ?)";
        $params = array($policy_id, $port_number, $port_protocol);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $policy_id, $port_number, $port_protocol)
    {
        $policy_id = intval($policy_id);
        $port_number = intval($port_number);

        $sql = "UPDATE policy_ports_reference SET policy_id = ?, port_number  = ?,
                port_protocol = ? WHERE policy_id = ? AND port_number = ? AND port_protocol = ?";
        $params = array($policy_id, $port_number, $port_protocol, $policy_id, $port_number, $port_protocol);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $policy_id, $port_number, $port_protocol) {
        $policy_id = intval($policy_id);
        $port_number = intval($port_number);

        $sql = "DELETE FROM policy_ports_reference WHERE policy_id = ? AND port_number = ? AND port_protocol = ?";
        $params = array($policy_id, $port_number, $port_protocol);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
