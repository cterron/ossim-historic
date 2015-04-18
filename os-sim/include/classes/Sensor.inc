<?php

class Sensor {

    var $name;
    var $ip;
    var $priority;
    var $port;
    var $connect;
    var $descr;

    function Sensor ($name, $ip, $priority, $port, $connect, $descr)
    {
        $this->name        = $name;
        $this->ip          = $ip;
        $this->priority    = $priority;
        $this->port        = $port;
        $this->connect     = $connect;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ip()           { return $this->ip; }
    function get_priority()     { return $this->priority; }
    function get_port()         { return $this->port; }
    function get_connect()      { return $this->connect; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM sensor $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Sensor ($rs->fields["name"],
                                      $rs->fields["ip"],
                                      $rs->fields["priority"],
                                      $rs->fields["port"],
                                      $rs->fields["connect"],
                                      $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $ip, $priority, $port, $descr)
    {
        $query = "INSERT INTO sensor (name, ip, priority, port, descr)
            VALUES ('$name', '$ip', '$priority', '$port', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $name, $ip, $priority, $port, $descr)
    {
        $query = "UPDATE sensor SET name = '$name', ip = '$ip', 
            priority = '$priority', port = '$port', descr = '$descr' 
            WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $name) {
        $query = "DELETE FROM sensor WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
