<?php

require_once 'Port_group_reference.inc';

class Port_group {

    var $name;
    var $descr;

    function Port_group ($name, $descr)
    {
        $this->name        = $name;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ip()           { return $this->ip; }
    function get_descr()        { return $this->descr; }

    function get_reference_ports($conn, $port_group_name) {
        return Port_group_reference::get_list
            ($conn, "WHERE port_group_name = '$port_group_name'");
    }

    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM port_group $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Port_group ($rs->fields["name"],
                                          $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $port_list, $descr)
    {
        $query = "INSERT INTO port_group VALUES 
            ('$name', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Port_group_reference::insert($conn, $name, $port_list);
    }

    function update($conn, $name, $port_list, $descr)
    {
        Port_group::delete($conn, $name);
        Port_group::insert($conn, $name, $port_list, $descr);
    }

    function delete($conn, $name) {
        $query = "DELETE FROM port_group WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Port_group_reference::delete($conn, $name);
    }
}

?>
