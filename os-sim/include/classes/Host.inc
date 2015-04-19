<?php
require_once ('ossim_conf.inc');
require_once ('classes/Host_sensor_reference.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Host_mac.inc');
require_once ('classes/Net.inc');
require_once ('classes/Session.inc');
require_once ('classes/Log_action.inc');
require_once ("ossim_db.inc");

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

    function get_os($conn)
    {
        if ($os = Host_os::get_ip_data($conn,$this->ip))
        {
            return $os["os"];
        }
        return "Unknown";
    }

    function get_mac_address($conn)
    {
        if ($mac = Host_mac::get_ip_data($conn, $this->ip))
        {
            return $mac["mac"];
        }
        return "";
    }

    function get_mac_vendor($conn)
    {
        if ($mac = Host_mac::get_ip_data($conn, $this->ip))
        {
            return $mac["vendor"];
        }
        return "";
    }

    function get_sensors($conn) {
        return Host_sensor_reference::get_list($conn, $this->ip);
    }

    function get_list($conn, $where = "", $order = "") 
    {
        mysql_escape_string($where);
        mysql_escape_string($order);

        $query = OssimQuery("SELECT * FROM host $where $order");
        $list = array();
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                if (Session::hostAllowed($conn, $rs->fields["ip"]))
                {
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
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $hostname, $asset, $threshold_c, $threshold_a,
                    $rrd_profile, $alert, $persistence, $nat, $sensors,
                    $descr, $os = "Unknown", $mac = "", $mac_vendor = "")
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");

        $sql = "INSERT INTO host (ip, hostname, asset, threshold_c, threshold_a, rrd_profile, 
                alert, persistence, nat, descr) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        
        $params = array($ip, $hostname, $asset, $threshold_c, $threshold_a, 
                        $rrd_profile, $alert, $persistence, $nat, $descr);
       
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting host: '.$conn->ErrorMsg().'<BR>';
            exit;
        } else {
            Host_sensor_reference::insert($conn, $ip, $sensors);
        }
        
        require_once 'classes/Sensor.inc';
        $sensor_list = Sensor::get_list($conn);
        if (is_array($sensor_list)) {
            foreach ($sensor_list as $aux) {
                $db_sensor_list[$aux->get_name()] = $aux->get_ip();
            }
        }

        foreach ($sensors as $sensor) {

	    if($os != "Unknown"){
            $sql = "INSERT INTO host_os (ip, os, date, anom, sensor)
                    VALUES (inet_aton(?), ?, CURRENT_TIMESTAMP, '0', inet_aton(?))";
            $params = array($ip, $os, $db_sensor_list[$sensor]);
            $conn->Execute($sql, $params);
	    }
            
	    if($mac != ""){ 
            $sql = "INSERT INTO host_mac (ip, mac, date, vendor, anom, sensor) 
                    VALUES (inet_aton(?), ?, CURRENT_TIMESTAMP, ?, '0', inet_aton(?))";
            
            $params = array($ip, $mac, $mac_vendor, $db_sensor_list[$sensor]);
            $conn->Execute($sql, $params);
            }
        }

        $infolog = array($hostname,"[".$ip."]");
        Log_action::log(24,$infolog);
   
    }

    function update($conn, $ip, $hostname, $asset, $threshold_c, $threshold_a, 
                    $rrd_profile, $alert, $persistence, $nat, $sensors,
                    $descr, $os = "Unknown", $mac = "", $mac_vendor = "")
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");

        $sql = "UPDATE host SET hostname = ?, ip = ?, asset = ?, threshold_c = ?,
                threshold_a = ?, rrd_profile = ?, alert = ?, persistence = ?, nat = ?,
                descr = ? WHERE ip = ?";
        $params = array($hostname, $ip, $asset, $threshold_c, $threshold_a,
                        $rrd_profile, $alert, $persistence, $nat, $descr, $ip);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* update sensor info */
        Host_sensor_reference::delete($conn, $ip);
        Host_sensor_reference::insert($conn, $ip, $sensors);
        

 
        require_once 'classes/Sensor.inc';
        $sensor_list = Sensor::get_list($conn);
        if (is_array($sensor_list)) {
            foreach ($sensor_list as $aux) {
                $db_sensor_list[$aux->get_name()] = $aux->get_ip();
            }
        }

        foreach ($sensors as $sensor) {
            if ($os != "Unknown") {
                $sql = "INSERT INTO host_os (ip, os, date, anom, sensor) VALUES 
                        (inet_aton(?), ?, CURRENT_TIMESTAMP, '0', inet_aton(?))";
                $params = array($ip, $os, $db_sensor_list[$sensor]);
                $conn->Execute($sql, $params);
            }
            if ($mac != "") {
                $sql = "INSERT INTO host_mac (ip, mac, date, vendor, anom, sensor) VALUES
                        (inet_aton(?), ?, CURRENT_TIMESTAMP, ?, '0', inet_aton(?))";
                $params = array($ip, $mac, $mac_vendor, $db_sensor_list[$sensor]);
                $conn->Execute($sql, $params);
            }
        }

    $infolog = array($hostname,"[".$ip."]");
    Log_action::log(26,$infolog);
    }

    function delete($conn, $ip) {
        
        $sql = "DELETE FROM host WHERE ip = ?";
        $params = array($ip);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        } else {
        /* WORK IN PROGRESS	
	    Host_sensor_reference::delete($conn, $ip);
            Host_os::delete($conn, $ip);
            Host_mac::delete($conn, $ip,NULL);
	*/
        }
    $infolog = array($ip);
    Log_action::log(25,$infolog);
        
    }

    function ip2hostname($conn, $ip, $is_sensor = false, $force_no_dns = false) {

	if($is_sensor == true){
        	$sql = "SELECT * FROM sensor WHERE ip = ?";
        	$params = array($ip);
        	if (!$rs = $conn->Execute($sql, $params)) {
            	print 'error: '.$conn->ErrorMsg().'<BR>';
            	exit;
        	}
        	if (!$rs->EOF) {
            	return $rs->fields["name"]; 
        	}
	}

        $sql = "SELECT * FROM host WHERE ip = ?";
        $params = array($ip);
        if (!$rs = $conn->Execute($sql, $params)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        if (!$rs->EOF) {
            return $rs->fields["hostname"]; 
        } else {
    
            require_once ('ossim_conf.inc');
            $config = $GLOBALS["CONF"];
            $use_resolv = $config->get_conf("use_resolv");
            if ($use_resolv and preg_match("/\d+\.\d+\.\d+\.\d+/", $ip) and $force_no_dns == false)
                return gethostbyaddr($ip);
            else 
                return $ip;
        }
    }

    function ipthresh_c($conn, $ip) {
        $sql = "SELECT * FROM host WHERE ip = ?";
        $params = array($ip);
        if (!$rs = $conn->Execute($sql, $params)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_c"]; 
        else {
            /* ip is not in db, return default threshold */
            $conf = $GLOBALS["CONF"];
            return $conf->get_conf("threshold");
        }
    }

    function ipthresh_a($conn, $ip) {
        $sql = "SELECT * FROM host WHERE ip = ?";
        $params = array($ip);
        if (!$rs = $conn->Execute($sql, $params)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_a"]; 
        else {
            /* ip is not in db, return default threshold */
            $conf = $GLOBALS["CONF"];
            return $conf->get_conf("threshold");
        }
    }

    function in_host($conn, $ip) {
        $sql = "SELECT * FROM host WHERE ip = ?";
        $params = array($ip);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }


    function get_related_sensors($conn, $ip)
    {
        $sensors = array();
       
        /* read from HOST table */
        $sql = "SELECT sensor.ip AS sensor FROM sensor, host_sensor_reference
                WHERE host_sensor_reference.host_ip = ? AND
                sensor.name = host_sensor_reference.sensor_name";
        $params = array($ip);
        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            $sensors[] = $rs->fields["sensor"];
        }

        /* read from NET table */
        if ($net_list = Net::get_list($conn))
        {
            foreach ($net_list as $net)
            {
                $net_name = $net->get_name();
                $ips = $net->get_ips();

                if (Net::isIpInNet($ip, $ips))
                {
                    $sql = "SELECT sensor.ip AS sensor 
                        FROM sensor, net_sensor_reference
                        WHERE name = sensor_name AND net_name = ?";
                    $params = array($net_name);
                    if (!$rs = &$conn->Execute($sql, $params)) {
                        print $conn->ErrorMsg();
                    } elseif (!$rs->EOF) {
                        $sensors[] = $rs->fields["sensor"];
                    }
                }
            }
        }

        return $sensors;
    }

}


?>
