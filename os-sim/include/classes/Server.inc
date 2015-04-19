<?php

require_once 'classes/Net_sensor_reference.inc';
require_once 'classes/Host_sensor_reference.inc';
require_once 'classes/Sensor.inc';
require_once 'classes/Server.inc';
require_once 'classes/Role.inc';
require_once 'classes/Net.inc';
require_once 'classes/Log_action.inc';
require_once ("ossim_db.inc");

class Server {

    var $name;
    var $ip;
    var $port;
    var $descr;

    function Server ($name, $ip, $port, $descr)
    {
        $this->name        = $name;
        $this->ip          = $ip;
        $this->port        = $port;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ip()           { return $this->ip; }
    function get_port()         { return $this->port; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM server $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                if (Session::hostAllowed($conn, $rs->fields["ip"]))
                {
                    $list[] = new Server ($rs->fields["name"],
                                          $rs->fields["ip"],
                                          $rs->fields["port"],
                                          $rs->fields["descr"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_role($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM server_role $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                if (Session::hostAllowed($conn, $rs->fields["ip"]))
                {
                    $list[] = new Role   ($rs->fields["name"],
										                      $rs->fields["correlate"],
                                          $rs->fields["cross_correlate"],
                                          $rs->fields["store"],
                                          $rs->fields["qualify"],
                                          $rs->fields["resend_alarm"],
                                          $rs->fields["resend_event"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function get_all($conn)
    {
        $list = array();
        $sql = "SELECT * FROM server ORDER BY name ASC";
        if (!$rs = $conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            $list[] = new Server ($rs->fields["name"],
                                  $rs->fields["ip"],
                                  $rs->fields["port"],
                                  $rs->fields["descr"]);
            $rs->MoveNext();
        }
        return $list;
    }

    function insert($conn, $name, $ip, $port, $descr, $r_correlate, $r_cross_correlate, $r_store, $r_qualify, $r_resend_alarm, $r_resend_event )
    {
        $sql = "INSERT INTO server (name, ip, port, descr)
                VALUES (?, ?, ?, ?)";
        $params = array($name, $ip, $port, $descr);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
				
        $sql = "INSERT INTO server_role (name, correlate, cross_correlate, store, qualify, resend_alarm, resend_event) 
	              VALUES (?, ?, ?, ?, ?, ?, ?)";
        $params = array($name, $r_correlate, $r_cross_correlate, $r_store, $r_qualify, $r_resend_alarm, $r_resend_event);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        $infolog = array($name,"[".$ip."]");
        Log_action::log(33,$infolog);
    }

    function update($conn, $name, $ip, $port, $descr, $r_correlate, $r_cross_correlate, $r_store, $r_qualify, $r_resend_alarm, $r_resend_event )
    {
        $sql = "UPDATE server SET name = ?, ip = ?, 
                port = ?, descr = ? 
                WHERE name = ?";
        $params = array($name, $ip, $port, $descr, $name);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
				$sql = "UPDATE server_role SET name = ?, correlate = ?, cross_correlate = ?, store = ?, qualify = ?, resend_alarm = ?, resend_event = ?
                WHERE name = ?";
        $params = array($name, $r_correlate, $r_cross_correlate, $r_store, $r_qualify, $r_resend_alarm, $r_resend_event, $name);
        if (!$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        $infolog = array($name,"[".$ip."]");
        Log_action::log(34,$infolog);
 
    }

    function delete($conn, $name)
    {
        $sql = "DELETE FROM server WHERE name = ?";
        if (!$conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
        $sql = "DELETE FROM server_role WHERE name = ?";
        if (!$conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
       $infolog = array($name);
       Log_action::log(35,$infolog);
    }
 



}

?>
