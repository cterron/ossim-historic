<?php
require_once ("ossim_db.inc");

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
        $query = "SELECT * FROM sensor_interfaces where sensor=? ORDER BY main DESC";
        if (!$rs = &$conn->Execute($query, array($sensor))) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        $main = intval($main);
        $sql = "INSERT INTO sensor_interfaces (sensor, interface, name, main) VALUES (?, ?, ?, ?)";
        $params = array($sensor, $interface, $name, $main);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function update_interfaces($conn, $sensor, $interface, $name, $main)
    {
        $main = intval($main);
        $sql = "UPDATE sensor_interfaces SET name = ?, main = ? WHERE sensor = ? and interface = ?";
        $params = array($name, $main, $sensor, $interface);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete_interfaces($conn, $sensor, $interface)
    {
        $sql = "DELETE FROM sensor_interfaces WHERE sensor = ? and interface = ?";
        $params = array($sensor, $interface);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
