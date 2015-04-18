<?php

//require_once 'classes/Signature.inc';
require_once 'classes/Signature_group_reference.inc';

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
        if (!$rs = &$conn->Execute("SELECT * FROM signature_group $args")) {
            print $conn->ErrorMsg();
        } else {
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
        $query = "INSERT INTO signature_group VALUES 
            ('$name', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::insert($conn, $name, $signature_list);
    }

    function update($conn, $name, $signature_list, $descr)
    {
        $query = "UPDATE signature_group SET name = '$name',  
            descr = '$descr' WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::update($conn, $name, $signature_list); 
    }

    function delete($conn, $name) {
        $query = "DELETE FROM signature_group WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Signature_group_reference::delete($conn, $name); 
    }
}

?>
