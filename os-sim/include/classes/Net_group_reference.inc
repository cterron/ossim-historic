<?php

require_once ("ossim_db.inc");

class Net_group_reference {

    var $net_group_name;
    var $net_name;
    
    function Net_group_reference ($net_group_name, $net_name)
    {
        $this->net_group_name      = $net_group_name;    
        $this->net_name  = $net_name;
    }

    function get_net_group_name()      { return $this->net_group_name; }
    function get_net_name()  { return $this->net_name; }
    
    function get_list($conn, $net) 
    {
        $list = array();
        $sql = "SELECT * FROM net_group_reference WHERE net_group_name = ?";
        if (!$rs = &$conn->Execute($sql, array($net))) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_group_reference 
                                                (stripslashes($rs->fields["net_group_name"]),
                                                 stripslashes($rs->fields["net_name"]));
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_group_name, $nets)
    {
        foreach ($nets as $net_name) {
            
            $sql = "INSERT INTO net_group_reference VALUES (?, ?)";
            $params = array($net_group_name, $net_name);
            
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $net_group_name, $net_name = "%") 
    {
        $sql = "DELETE FROM net_group_reference WHERE net_group_name = ? AND net_name LIKE ?";
        $params = array($net_group_name, $net_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_net_group_reference($conn, $net, $net_name)
    {
        $sql = "SELECT * FROM net_group_reference WHERE net_group_name = ? AND net_name = ?";
        $params = array($net, $net_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    
    function check_orphan($conn)
    {
        $sql = "SELECT * FROM net_group_reference LEFT JOIN net ON net.name = net_group_reference.net_name WHERE descr IS NULL";

        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Net_group_reference::delete($conn, $rs->fields["net_group_name"], $rs->fields["net_name"]);
                $rs->MoveNext();
            }
        }
    }
}

?>
