<?php

//require_once 'classes/Signature.inc';
require_once 'classes/Signature_group_reference.inc';
require_once 'classes/Log_action.inc';
require_once ("ossim_db.inc");

class Signature_group {

    var $name;
    var $descr;

    function Signature_group ($name, $descr)
    {
        $this->name        = $name;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_descr()        { return $this->descr; }

    function get_reference_signatures($conn, $sig_group_name) {
        return Signature_group_reference::get_list
            ($conn, "WHERE sig_group_name = '$sig_group_name'");
    }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM signature_group $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Signature_group  ($rs->fields["name"],
                                                $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $signature_list, $descr)
    {
        $sql = "INSERT INTO signature_group VALUES  (?, ?)";
        $params = array($name, $descr);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::insert($conn, $name, $signature_list);
    
        $infolog = array($name);
        Log_action::log(36,$infolog);
    }

    function update($conn, $name, $signature_list, $descr)
    {
        $sql = "UPDATE signature_group SET name = ?, descr = ? WHERE name = ?";
        $params = array($name, $descr, $name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::update($conn, $name, $signature_list); 
    
        $infolog = array($name);
        Log_action::log(37,$infolog);
    }

    function delete($conn, $name) {
        $sql = "DELETE FROM signature_group WHERE name = ?";
        $params = array($name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::delete($conn, $name); 
        
        $infolog = array($name);
        Log_action::log(38,$infolog);
    }
}

?>
