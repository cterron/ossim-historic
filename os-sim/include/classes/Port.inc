<?php

require_once("ossim_db.inc");

class Port {

    var $port_number;
    var $protocol_name;
    var $service;
    var $descr;

    function Port ($port_number, $protocol_name, $service, $descr)
    {
        $this->port_number      = $port_number;
        $this->protocol_name    = $protocol_name;
        $this->service          = $service;
        $this->descr            = $descr;
    }

    function get_port_number()       { return $this->port_number; }
    function get_protocol_name()     { return $this->protocol_name; }
    function get_service()           { return $this->service; }
    function get_descr()             { return $this->descr; }

    function get_list($conn, $where = "", $order = "") 
    {
        $query = OssimQuery("SELECT * FROM port $where $order");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Port ($rs->fields["port_number"],
                                    $rs->fields["protocol_name"],
                                    $rs->fields["service"],
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $port_number, $protocol_name, $service, $descr)
    {
        settype($port_number, "int");

        $sql = "INSERT INTO port VALUES (?, ?, ?, ?)";
        $params = array($port_number, $protocol_name, $service, $descr);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $port_number, $protocol_name, $service, $descr)
    {
        settype($port_number, "int");

        $sql = "UPDATE port SET port_number = ?, protocol_name = ?, service = ?,
                descr = ? WHERE port_number = ? AND protocol_name = ?";
        $params = array($port_number, $protocol_name, $service, $descr, $port_number, $protocol_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $port_number, $protocol_name) {
        settype($port_number, "int");

        $sql = "DELETE FROM port WHERE port_number = ? AND protocol_name = ?";
        $params = array($port_number, $protocol_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function port2service($conn, $port, $protocol = "tcp")
    {
        if (($protocol != "tcp") and ($protocol != "udp"))
            return $port;

        /* first look at ossim.port table */
        $sql = "SELECT service FROM port WHERE port_number = ? AND protocol_name = ?";
        $params = array($port, $protocol);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF)
                return $rs->fields["service"];
        }

        /* there is no info about $port-$protocol in ossim.port table */
        if ($service = getservbyport($port, $protocol)) {
            $sql = "INSERT INTO port (port_number, protocol_name, service, descr) VALUES (?, ?, ?, '')";
            $params = array($port, $protocol, $service);
            if ($conn->Execute($sql, $params) === false) {
                print 'error inserting port info: '.$conn->ErrorMsg().'<BR>';
            }
            return $service;
        }

        /* getservbyport failed */
        return $port;

    }
}

?>
