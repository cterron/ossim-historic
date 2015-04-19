<?php

require_once("ossim_db.inc");

class Signature_group_reference {

    var $sig_name;
    var $sig_group_name;
    
    function Signature_group_reference ($sig_name, $sig_group_name)
    {
        $this->sig_name  = $sig_name;
        $this->sig_group_name   = $sig_group_name;    
    }

    function get_sig_name()   { return $this->sig_name; }
    function get_sig_group_name()  { return $this->sig_group_name; }
    
    function get_list($conn, $where = "", $order = "") 
    {
        $query = OssimQuery("SELECT * FROM signature_group_reference $where $order");
        if (!$rs = &$conn->Execute($query)) 
        {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Signature_group_reference
                                        ($rs->fields["sig_name"],
                                         $rs->fields["sig_group_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $sig_name, $signature_list)
    {
        foreach ($signature_list as $ip) {
            $sql = "INSERT INTO signature_group_reference VALUES (?, ?)";
            $params = array($sig_name, $ip);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function update($conn, $sig_group_name, $signature_list)
    {
        Signature_group_reference::delete($conn, 
                                          $sig_group_name);
        Signature_group_reference::insert($conn, 
                                          $sig_group_name, $signature_list);
        
    }

    function delete($conn, $sig_group_name) 
    {
        $sql = "DELETE FROM signature_group_reference WHERE sig_group_name = ?";
        $params = array($sig_group_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_signature_group_reference($conn, $sig_group_name, $sig_name) 
    {
        $sql = "SELECT * FROM signature_group_reference WHERE sig_group_name = ? and sig_name = ?";
        $params = array($sig_group_name, $sig_name);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}

?>

