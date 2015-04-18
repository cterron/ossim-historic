<?php

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
        if (!$rs = &$conn->Execute("SELECT * FROM policy_ports_reference $where $order")) {
            print $conn->ErrorMsg();
        } else {
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
        $query = "INSERT INTO policy_ports_reference VALUES 
            ($policy_id, $port_number, '$port_protocol');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $policy_id, $port_number, $port_protocol)
    {
        $policy_id = intval($policy_id);
        $port_number = intval($port_number);

        $query = "UPDATE policy_ports_reference SET policy_id = $policy_id, 
            port_number = $port_number, port_protocol = '$port_protocol' WHERE policy_id =
            $policy_id AND port_number = $port_number AND port_protocol = '$protocol';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $policy_id, $port_number, $port_protocol) {
        $policy_id = intval($policy_id);
        $port_number = intval($port_number);

        $query = "DELETE FROM policy_ports_reference WHERE policy_id =
        $policy_id AND port_number = $port_number AND port_protocol =
        '$port_protocol';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
