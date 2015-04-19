<?php

require_once 'classes/Net_sensor_reference.inc';
require_once 'classes/Host_sensor_reference.inc';
require_once 'classes/Sensor.inc';
require_once 'classes/Net.inc';
require_once 'classes/Log_action.inc';
require_once ("ossim_db.inc");

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
        $query = OssimQuery("SELECT * FROM sensor $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                if (Session::hostAllowed($conn, $rs->fields["ip"]))
                {
                    $list[] = new Sensor ($rs->fields["name"],
                                          $rs->fields["ip"],
                                          $rs->fields["priority"],
                                          $rs->fields["port"],
                                          $rs->fields["connect"],
                                          $rs->fields["descr"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_all($conn)
    {
        $list = array();
        $sql = "SELECT * FROM sensor ORDER BY name ASC";
        if (!$rs = $conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            $list[] = new Sensor ($rs->fields["name"],
                                  $rs->fields["ip"],
                                  $rs->fields["priority"],
                                  $rs->fields["port"],
                                  $rs->fields["connect"],
                                  $rs->fields["descr"]);
            $rs->MoveNext();
        }
        return $list;
    }

    function insert($conn, $name, $ip, $priority, $port, $descr)
    {
        $sql = "INSERT INTO sensor (name, ip, priority, port, connect, descr)
                VALUES (?, ?, ?, ?, 0, ?)";
        $params = array($name, $ip, $priority, $port, $descr);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        $sql = "INSERT INTO sensor_stats (name) VALUES (?)";
        if (!$conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
        $infolog = array($name,"[".$ip."]");
        Log_action::log(33,$infolog);
    }

    function update($conn, $name, $ip, $priority, $port, $descr)
    {
        $sql = "UPDATE sensor SET name = ?, ip = ?, 
                priority = ?, port = ?, descr = ? 
                WHERE name = ?";
        $params = array($name, $ip, $priority, $port, $descr, $name);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        $sql = "UPDATE sensor_stats SET name = ? WHERE name = ?";
        if (!$conn->Execute($sql, array($name, $name))) {
            die($conn->ErrorMsg());
        }
        $infolog = array($name,"[".$ip."]");
        Log_action::log(34,$infolog);
 
    }

    function delete($conn, $name)
    {
        $sql = "DELETE FROM sensor WHERE name = ?";
        if (!$conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
        $sql = "DELETE FROM sensor_stats WHERE name = ?";
        if (!$conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
        Net_sensor_reference::check_orphan($conn);
        Host_sensor_reference::check_orphan($conn);
       
       $infolog = array($name);
       Log_action::log(35,$infolog);
    }

    /*
     * Given a host ip, return its sensor ip asociated.
     *
     * Look at:
     *  1st) host table
     *  2nd) net table
     *  3rd) /etc/ossim.conf
     */
    function get_sensor_link($conn, $host_ip)
    {
        require_once "classes/Net.inc";
        require_once "ossim_conf.inc";
        $conf = $GLOBALS["CONF"];

        $sensor_ip = '';

        /* read from host table */
        $query = OssimQuery("SELECT sensor.ip FROM host_sensor_reference, sensor 
            WHERE host_sensor_reference.host_ip = '$host_ip' AND 
            sensor.name = host_sensor_reference.sensor_name 
            ORDER BY sensor.priority DESC LIMIT 1");

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            list($protocol, $ip, $port)  = 
                split(':', $conf->get_conf("ntop_link"));
            $sensor_ip = "$protocol://" . $rs->fields["ip"] . ":$port";
            return $sensor_ip;
        } 


        /* read from network table */
        if ($net_list = Net::get_list($conn))
        {
            foreach ($net_list as $net)
            {
                $net_name = $net->get_name();
                $ips = $net->get_ips();
    
                if (Net::isIpInNet($host_ip, $ips)) 
                {
                    $query = OssimQuery("SELECT ip FROM sensor, net_sensor_reference 
                        WHERE name = sensor_name AND net_name = '$net_name'");

                    if (!$rs = &$conn->Execute($query)) {
                        print $conn->ErrorMsg();
                    } 
                    
                    elseif (!$rs->EOF) 
                    {
                        list($protocol, $ip, $port) = 
                            split(':', $conf->get_conf("ntop_link"));
                        $sensor_ip = "$protocol://" . 
                            $rs->fields["ip"] . ":$port";

                        return $sensor_ip;
                    }
                }
            }
        }

        /* read from /etc/ossim.conf */
        if ($sensor_ip == '') {
            $sensor_ip = $conf->get_conf("ntop_link");
        }

        return $sensor_ip;
    }

    /*
     * Given a host ip, return its main sensor's interface.
     *
     * Look at:
     *  1st) host table
     *  2nd) net table
     *  3rd) /etc/ossim.conf
     */
    function get_sensor_interface($conn, $host_ip)
    {
        require_once "classes/Net.inc";
        require_once "ossim_conf.inc";
        $conf = $GLOBALS["CONF"];

        $sensor_interface_name = '';

        /* read from host table */
        $query = OssimQuery("SELECT sensor_interfaces.interface FROM
        host_sensor_reference, sensor, sensor_interfaces
            WHERE host_sensor_reference.host_ip = '$host_ip' AND 
            sensor.name = host_sensor_reference.sensor_name AND
            sensor_interfaces.main = 1 ORDER BY sensor.priority DESC LIMIT 1");

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            $sensor_interface_name = $rs->fields["interface"];
            return $sensor_interface_name;
        } 

        /* read from network table */
        if ($net_list = Net::get_list($conn))
        {
            foreach ($net_list as $net)
            {
                $net_name = $net->get_name();
                $ips = $net->get_ips();
    
                if (Net::isIpInNet($host_ip, $ips)) 
                {
                    $query = OssimQuery("SELECT sensor_interfaces.interface FROM sensor,
                    net_sensor_reference, sensor_interfaces WHERE
                    sensor_interfaces.name = sensor.name AND net_name =
                    '$net_name' AND sensor_interfaces.main = 1");

                    if (!$rs = &$conn->Execute($query)) {
                        print $conn->ErrorMsg();
                    } 
                    
                    elseif (!$rs->EOF) 
                    {
                        $sensor_interface_name = $rs->fields["interface"];
                        return $sensor_interface_name;
                    }
                }
            }
        }

        /* read from /etc/ossim.conf */
        if ($sensor_interface_name == '') {
            $sensor_interface_name = $conf->get_conf("ossim_interface");
        }

        return $sensor_interface_name;
    }
 



}

?>
