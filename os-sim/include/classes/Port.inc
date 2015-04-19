<?php

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
        if (!$rs = &$conn->Execute("SELECT * FROM port $where $order")) {
            print $conn->ErrorMsg();
        } else {
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

        $query = "INSERT INTO port VALUES 
            ($port_number, '$protocol_name', '$service', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $por_number, $protocol_name, $sevice, $descr)
    {
        settype($port_number, "int");

        $query = "UPDATE port SET port_number = $port_number, protocol_name =
        '$protocol_name', service = '$service', descr = '$descr' WHERE
        port_number = $port_number AND protocol_name = '$protocol_name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $port_number, $protocol_name) {
        settype($port_number, "int");

        $query = "DELETE FROM port WHERE port_number = $port_number AND
        protocol_name = '$protocol_name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function port2service($conn, $port, $protocol = "tcp")
    {
        if (($protocol != "tcp") and ($protocol != "udp"))
            return $port;

        /* first look at ossim.port table */
        $query = "SELECT service FROM port WHERE port_number = $port 
            AND protocol_name = '$protocol'";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF)
                return $rs->fields["service"];
        }

        /* there is no info about $port-$protocol in ossim.port table */
        if ($service = getservbyport($port, $protocol)) {
            $query = "INSERT INTO port 
                (port_number, protocol_name, service, descr) VALUES
                ($port, '$protocol', '$service', '');";
            if ($conn->Execute($query) === false) {
                print 'error inserting port info: '.$conn->ErrorMsg().'<BR>';
            }
            return $service;
        }

        /* getservbyport failed */
        return $port;

    }
}

?>
