<?php

/**
 ** Common functions to DB management
 **/

require_once "ossim_conf.inc";
$conf = new ossim_conf();

$adodb_path = $conf->get_conf("adodb_path");
require_once ("$adodb_path/adodb.inc.php");

class ossim_db {

    /*
     *  Connect and close DB functions
     *  Read DB properties from /etc/ossim.conf
     */
    function connect () { 
        global $conf;

        $base = $conf->get_conf("ossim_base");
        $user = $conf->get_conf("ossim_user");
        $pass = $conf->get_conf("ossim_pass");
        $host = $conf->get_conf("ossim_host");

        $conn = &ADONewConnection('mysql');
        $conn->PConnect($host, $user, $pass, $base);
        return $conn;
    }

    function snort_connect() {
        global $conf;

        $base = $conf->get_conf("snort_base");
        $user = $conf->get_conf("snort_user");
        $pass = $conf->get_conf("snort_pass");
        $host = $conf->get_conf("snort_host");

        $conn = &ADONewConnection('mysql');
        $conn->PConnect($host, $user, $pass, $base);
        return $conn;
    }

    function opennms_connect() {
        global $conf;

        $base = $conf->get_conf("opennms_base");
        $user = $conf->get_conf("opennms_user");
        $pass = $conf->get_conf("opennms_pass");
        $host = $conf->get_conf("opennms_host");

        $conn = &ADONewConnection('pgsql');
        $conn->PConnect($host, $user, $pass, $base);
        return $conn;
    }

    function close($conn) {
        $conn->Close();
    }


    /*
     *  Common functions
     */
    function max_val($conn, $column, $table) {
    
        $query = "SELECT max($column) FROM $table;";
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            return $rs->fields["max($column)"];
        }
    }

    function get_order($column, $order) {
        if ($order == $column) {
            return "$order DESC";
        } else {
            return $column;
        }
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
        require_once "ossim_conf.inc";
        $conf = new ossim_conf();

        $sensor_ip = '';

        /* read from host table */
        $query = "SELECT sensor.ip FROM host_sensor_reference, sensor 
            WHERE host_sensor_reference.host_ip = '$host_ip' AND 
            sensor.name = host_sensor_reference.sensor_name 
            ORDER BY sensor.priority DESC LIMIT 1;";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            list($protocol, $ip, $port)  = 
                split(':', $conf->get_conf("ntop_link"));
            $sensor_ip = "$protocol://" . $rs->fields["ip"] . ":$port";
        } 

        if ($sensor_ip == '') {
        
            /* read from net table */
            $query = "SELECT * FROM net_host_reference nhr, 
                                net_sensor_reference nsr, 
                                sensor s 
                    WHERE s.name = nsr.sensor_name AND 
                          nhr.net_name = nsr.net_name AND 
                          nhr.host_ip = '$host_ip' ORDER BY s.priority DESC;";

            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            } elseif (!$rs->EOF) {
                list($protocol, $ip, $port) = 
                    split(':', $conf->get_conf("ntop_link"));
                $sensor_ip = "$protocol://" . $rs->fields["ip"] . ":$port";
            }
        }
       
        /* read from /etc/ossim.conf */
        if ($sensor_ip == '') {
            $sensor_ip = $conf->get_conf("ntop_link");
        }

        return $sensor_ip;
    }
}

?>

