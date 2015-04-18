<?php

class Port_group_reference {

    var $port_group_name;
    var $port_number;
    var $protocol_name;

    function Port_group_reference ($port_group_name, $port_number,
                                   $protocol_name)
    {
        $this->port_group_name  = $port_group_name;
        $this->port_number      = $port_number;
        $this->protocol_name    = $protocol_name;
    }
    
    function get_group_name()       { return $this->port_group_name; }
    function get_port_number()      { return $this->port_number; }
    function get_protocol_name()    { return $this->protocol_name; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM port_group_reference 
                                   $where $order"))
        {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Port_group_reference 
                                   ($rs->fields["port_group_name"],
                                    $rs->fields["port_number"],
                                    $rs->fields["protocol_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $port_group_name, $port_list)
    {
        foreach ($port_list as $port)
        {
            /* 
             * port_list contains an array like this
             * (80-tcp, 80-udp, 8080-tcp, 8080-udp)
             */
            $port_array = explode ('-', $port);
            $port_number = $port_array[0];
            $protocol_name = $port_array[1];

            $query = "INSERT INTO port_group_reference VALUES 
                ('$port_group_name', $port_number, '$protocol_name');";
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $port_group_name) {
        $query = "DELETE FROM port_group_reference WHERE 
                    port_group_name = '$port_group_name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_port_group_reference($conn, $port_group_name, $port_number,
                                     $protocol_name)
    {
        $query = "SELECT * FROM port_group_reference WHERE
            port_group_name = '$port_group_name' AND 
            port_number = $port_number AND protocol_name = '$protocol_name';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}

?>

