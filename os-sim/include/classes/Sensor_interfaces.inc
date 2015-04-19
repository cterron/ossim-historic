<?php

class Sensor_interfaces {

    var $sensor;
    var $interface;
    var $name;
    var $main;

    function Sensor_interfaces ($sensor, $interface, $name, $main)
    {
        $this->sensor       = $sensor;
        $this->interface    = $interface;
        $this->name         = $name;
        $this->main         = $main;
    }

    function get_sensor()           { return $this->sensor; }
    function get_interface()        { return $this->interface; }
    function get_name()             { return $this->name; }
    function get_main()             { return $this->main; }

    function get_list($conn, $sensor) 
    {
        $sensor = mysql_real_escape_string($sensor);
        if (!$rs = &$conn->Execute("SELECT * FROM sensor_interfaces where sensor = '$sensor' ORDER BY MAIN DESC")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Sensor_interfaces ($rs->fields["sensor"],
                                                $rs->fields["interface"],
                                                $rs->fields["name"],
                                                $rs->fields["main"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert_interfaces($conn, $sensor, $interface, $name, $main)
    {
        $sensor = mysql_real_escape_string($sensor);
        $interface = mysql_real_escape_string($interface);
        $name = mysql_real_escape_string($name);
        $main = intval($main);
        $query = "INSERT INTO sensor_interfaces (sensor, interface, name, main)
            VALUES ('$sensor', '$interface', '$name', $main);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function update_interfaces($conn, $sensor, $interface, $name, $main)
    {
        $sensor = mysql_real_escape_string($sensor);
        $interface = mysql_real_escape_string($interface);
        $name = mysql_real_escape_string($name);
        $main = intval($main);
        $query = "UPDATE sensor_interfaces SET name = '$name', main = $main
        WHERE sensor = '$sensor' and interface = '$interface'";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete_interfaces($conn, $sensor, $interface)
    {
        $sensor = mysql_real_escape_string($sensor);
        $interface = mysql_real_escape_string($interface);
        $query = "DELETE FROM sensor_interfaces WHERE sensor = '$sensor' and interface = '$interface'";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
