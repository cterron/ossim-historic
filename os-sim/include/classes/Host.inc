<?php

require_once ('classes/Conf.inc');
require_once ('classes/Host_sensor_reference.inc');

class Host {

    var $ip;
    var $hostname;
    var $asset;
    var $threshold_c;
    var $threshold_a;
    var $rrd_profile;
    var $alert;
    var $persistence;
    var $nat;
    var $descr;

    function Host ($ip, $hostname, $asset, $threshold_c, $threshold_a,
                   $rrd_profile, $alert, $persistence, $nat, $descr)
    {
        $this->ip          = htmlentities($ip);
        $this->hostname    = htmlentities($hostname);
        $this->asset       = intval($asset);
        $this->threshold_c = intval($threshold_c);
        $this->threshold_a = intval($threshold_a);
        $this->rrd_profile = htmlentities($rrd_profile);
        $this->alert       = intval($alert);
        $this->persistence = intval($persistence);
        $this->nat         = htmlentities($nat);
        $this->descr       = htmlentities($descr);
    }

    function get_ip()           { return $this->ip; }
    function get_hostname()     { return $this->hostname; }
    function get_asset()        { return $this->asset; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_rrd_profile()  { return $this->rrd_profile; }
    function get_alert()        { return $this->alert; }
    function get_persistence()  { return $this->persistence; }
    function get_nat()          { return $this->nat; }
    function get_descr()        { return $this->descr; }

    function get_sensors($conn) {
        return Host_sensor_reference::get_list($conn, $this->ip);
    }

    function get_list($conn, $where = "", $order = "") 
    {
        mysql_escape_string($where);
        mysql_escape_string($order);

        $query = "SELECT * FROM host $where $order";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host ($rs->fields["ip"],
                                    $rs->fields["hostname"],
                                    $rs->fields["asset"],
                                    $rs->fields["threshold_c"],
                                    $rs->fields["threshold_a"],
                                    $rs->fields["rrd_profile"],
                                    $rs->fields["alert"],
                                    $rs->fields["persistence"],
                                    $rs->fields["nat"],
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $hostname, $asset, $threshold_c, $threshold_a, 
                    $rrd_profile, $alert, $persistence, $nat, $sensors, $descr)
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");
        $ip = htmlentities($ip);
        $hostname = htmlentities($hostname);
        $rrd_profile = htmlentities($rrd_profile);
        $alert = htmlentities($alert);
        $nat = htmlentities($nat);
        $descr = htmlentities($descr);

        $query = "INSERT INTO host (
            ip, hostname, asset, threshold_c, threshold_a, rrd_profile, 
            alert, persistence, nat, descr) VALUES 
            ('$ip', '$hostname', $asset, $threshold_c,
             $threshold_a, '$rrd_profile', '$alert', '$persistence', 
             '$nat', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        } else {
            Host_sensor_reference::insert($conn, $ip, $sensors);
        }
    }

    function update($conn, $ip, $hostname, $asset, $threshold_c, $threshold_a, 
                    $rrd_profile, $alert, $persistence, $nat, $sensors, $descr)
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");
        $ip = htmlentities($ip);
        $hostname = htmlentities($hostname);
        $rrd_profile = htmlentities($rrd_profile);
        $alert = htmlentities($alert);
        $nat = htmlentities($nat);
        $descr = htmlentities($descr);

        $query = "UPDATE host SET hostname = '$hostname', 
            ip = '$ip', asset = $asset, 
            threshold_c = $threshold_c, threshold_a = $threshold_a,
            rrd_profile = '$rrd_profile',
            alert = $alert, persistence = $persistence, nat = '$nat',
            descr = '$descr' WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        } else {
            Host_sensor_reference::delete($conn, $ip);
            Host_sensor_reference::insert($conn, $ip, $sensors);
        }
    }

    function delete($conn, $ip) {
        $ip = mysql_escape_string($ip);
        $query = "DELETE FROM host WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        } else {
            Host_sensor_reference::delete($conn, $ip);
        }
    }

    function ip2hostname($conn, $ip) {
        $ip = mysql_escape_string($ip);
        $query = "SELECT * FROM host WHERE ip = '$ip';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        if (!$rs->EOF) {
            return $rs->fields["hostname"]; 
        } else {
            if(preg_match("/\d+\.\d+\.\d+\.\d+/", $ip)){
            return gethostbyaddr($ip);
            } else {
            return $ip;
            }
        }
    }

    function ipthresh_c($conn, $ip) {
        $ip = mysql_escape_string($ip);
        $query = "SELECT * FROM host WHERE ip = '$ip';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_c"]; 
        else {
            /* ip is not in db, return default threshold */
            $conf = Conf::get_conf($conn);
            return $conf->get_threshold();
        }
    }

    function ipthresh_a($conn, $ip) {
        $ip = mysql_escape_string($ip);
        $query = "SELECT * FROM host WHERE ip = '$ip';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_a"]; 
        else {
            /* ip is not in db, return default threshold */
            $conf = Conf::get_conf($conn);
            return $conf->get_threshold();
        }
    }

    function in_host($conn, $ip) {
        $ip = mysql_escape_string($ip);
        if (!$rs = &$conn->Execute("SELECT * FROM host WHERE ip = '$ip'")) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

}


?>
