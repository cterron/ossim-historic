<?php

require_once 'classes/Net_qualification.inc';
require_once 'classes/Net_vulnerability.inc';
require_once 'classes/Net_sensor_reference.inc';
require_once 'classes/Control_panel_net.inc';
require_once 'ossim_conf.inc';


class Net {

    var $name;
    var $ips;
    var $priority;
    var $threshold_c;
    var $threshold_a;
    var $rrd_profile;
    var $alert;
    var $persistence;
    var $descr;

    function Net ($name, $ips, $priority, $threshold_c, $threshold_a,
                  $rrd_profile, $alert, $persistence, $descr)
    {
        $this->name        = $name;
        $this->ips         = $ips;
        $this->priority    = $priority;
        $this->threshold_c = $threshold_c;
        $this->threshold_a = $threshold_a;
        $this->rrd_profile = $rrd_profile;
        $this->alert       = $alert;
        $this->persistence = $persistence;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ips()          { return $this->ips; }
    function get_priority()     { return $this->priority; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_rrd_profile()  { return $this->rrd_profile; }
    function get_alert()        { return $this->alert; }
    function get_persistence()  { return $this->persistence; }
    function get_descr()        { return $this->descr; }


    function get_sensors($conn) {
        return Net_sensor_reference::get_list($conn, $this->name);
    }


    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM net $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net  ($rs->fields["name"],
                                    $rs->fields["ips"],
                                    $rs->fields["priority"],
                                    $rs->fields["threshold_c"],
                                    $rs->fields["threshold_a"],
                                    $rs->fields["rrd_profile"],
                                    $rs->fields["alert"],
                                    $rs->fields["persistence"],
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $ips, $priority, $threshold_c, $threshold_a, 
                    $rrd_profile, $alert, $persistence, $sensors, $descr)
    {
        settype($priority, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");
        $name = mysql_escape_string($name);
        $rrd_profile = htmlentities($rrd_profile);
        $persistence = htmlentities($descr);

        $name = str_replace(" ", "_", $name);
        $ips = str_replace(" ", "", $ips);

        $query = "INSERT INTO net (
            name, ips, priority, threshold_c, threshold_a, 
            rrd_profile, alert, persistence, descr) VALUES
            ('$name', '$ips', $priority, $threshold_c, $threshold_a, 
             '$rrd_profile', '$alert', '$persistence', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        Net_qualification::insert($conn, $name, 1, 1);
        Net_vulnerability::insert($conn, $name, 1);
        Net_sensor_reference::insert($conn, $name, $sensors);
    }

    function update($conn, $name, $ips, $priority, $threshold_c, $threshold_a, 
                    $rrd_profile, $alert, $persistence, $sensors, $descr)
    {
        Net::delete($conn, $name);
        Net::insert($conn, $name, $ips, $priority,
                    $threshold_c, $threshold_a, $rrd_profile,
                    $alert, $persistence, $sensors, $descr);
    }

    function delete($conn, $name) {
        $name = mysql_real_escape_string($name);
        $query = "DELETE FROM net WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Control_panel_net::delete($conn, $name);
        Net_qualification::delete($conn, $name);
        Net_vulnerability::delete($conn, $name);
        Net_sensor_reference::delete($conn, $name);
    }

    function netthresh_c($conn, $name) {
        $name = mysql_real_escape_string($name);
        $query = "SELECT * FROM net WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_c"]; 
        else {
            /* net is not in db, return default threshold */
            $conf = new ossim_conf();
            return $conf->get_conf("threshold");
        }
    }

    function netthresh_a($conn, $name) {
        $name = mysql_real_escape_string($name);
        $query = "SELECT * FROM net WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_a"]; 
        else {
            /* net is not in db, return default threshold */
            $conf = new ossim_conf();
            return $conf->get_conf("threshold");
        }
    }

    /*
     * PRE: 
     *   ip in ipv4 format (X.X.X.X)
     *   nets in net/mask format (X.X.X.X/X,X.X.X.X/X,...)
     */
    function isIpInNet($ip, $nets)
    {

        $net_list = explode(",", $nets);
        foreach ($net_list as $n) 
        {
            list($net, $mask) = split("/", $n);
            
            $val1 = ip2long($ip);
            $val2 = ip2long($net);

            if (($val1 >> (32 - $mask)) == ($val2 >> (32 - $mask)))
                return True;
        }

        return False;
    }

    function enable_nessus($conn, $net_name)
    {
        $name = mysql_real_escape_string($name);
        $query = "INSERT INTO net_scan (net_name, plugin_id, plugin_sid) 
            VALUES ('$net_name', 3001, 0)";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function disable_nessus($conn, $net_name)
    {
        $name = mysql_real_escape_string($name);
        $query = "DELETE FROM net_scan 
            WHERE net_name = '$net_name' AND plugin_id = 3001";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function isIpInAnyNet($conn, $ip)
    {

        $net_list = Net::get_list($conn);
        foreach ($net_list as $n) 
        {
            list($net, $mask) = split("/", $n);
            
            $val1 = ip2long($ip);
            $val2 = ip2long($net);

            if (($val1 >> (32 - $mask)) == ($val2 >> (32 - $mask)))
                return True;
        }

        return False;
    }



}

?>
