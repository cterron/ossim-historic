<?php

require_once 'Port_group_reference.inc';
require_once ('classes/Log_action.inc');
require_once ("ossim_db.inc");

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
        $query = OssimQuery("SELECT * FROM port_group $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        $sql = "INSERT INTO port_group VALUES (?, ?)";
        $params = array($name, $descr);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        Port_group_reference::insert($conn, $name, $port_list);

        $infolog = array($name,"(".$descr.")");
        Log_action::log(30,$infolog);
    }

    function update($conn, $name, $port_list, $descr)
    {
        Port_group::delete($conn, $name);
        Port_group::insert($conn, $name, $port_list, $descr);
        
        $infolog = array($name,"(".$descr.")");
        Log_action::log(31,$infolog);
 
    }

    function delete($conn, $name) {
        
        $sql = "DELETE FROM port_group WHERE name = ?";
        $params = array($name);
        
        if ($conn->Execute($sql, $params) === false) {
       
        print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Port_group_reference::delete($conn, $name);
 
        $infolog = array($name);
        Log_action::log(32,$infolog);}
    }

?>
