<?php

class Host_sensor_reference {

    var $host_ip;
    var $sensor_name;
    
    function Host_sensor_reference ($host_ip, $sensor_name)
    {
        $this->host_ip      = $host_ip;    
        $this->sensor_name  = $sensor_name;
    }

    function get_host_ip()      { return $this->host_ip; }
    function get_sensor_name()  { return $this->sensor_name; }
    
    function get_list($conn, $ip) 
    {
        $query = "SELECT * FROM host_sensor_reference WHERE host_ip = '$ip'";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_sensor_reference 
                                                ($rs->fields["host_ip"],
                                                 $rs->fields["sensor_name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_ip, $sensors)
    {
        foreach ($sensors as $sensor_name) {
            $query = "INSERT INTO host_sensor_reference VALUES 
                    ('$host_ip', '$sensor_name');";
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

/*
    function update($conn, $sensor_name, $host_ip)
    {
        $query = "UPDATE host_sensor_reference SET host_ip = '$host_ip', 
            sensor_name = '$sensor_name' WHERE sensor_name = '$sensor_name'";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/
    function delete($conn, $host_ip) 
    {
        $query = "DELETE FROM host_sensor_reference 
                    WHERE host_ip = '$host_ip'";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_host_sensor_reference($conn, $ip, $sensor_name)
    {
                                                                                
        $query = "SELECT * FROM host_sensor_reference
                    WHERE host_ip = '$ip' AND sensor_name='$sensor_name';";
        echo $query;
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
}

?>
