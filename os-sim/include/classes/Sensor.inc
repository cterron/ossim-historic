<?php

class Sensor {

    var $name;
    var $ip;
    var $descr;

    function Sensor ($name, $ip, $descr)
    {
        $this->name        = $name;
        $this->ip          = $ip;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ip()           { return $this->ip; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM sensor $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Sensor ($rs->fields["name"],
                                      $rs->fields["ip"],
                                      $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $ip, $descr)
    {
        $query = "INSERT INTO sensor VALUES 
            ('$name', '$ip', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $name, $ip, $descr)
    {
        $query = "UPDATE sensor SET name = '$name', 
            ip = '$ip', descr = '$descr' WHERE name = '$name';";
        echo "$query\n";
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
