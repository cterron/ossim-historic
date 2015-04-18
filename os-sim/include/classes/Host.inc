<?php

require_once ('classes/Conf.inc');

class Host {

    var $ip;
    var $hostname;
    var $asset;
    var $threshold_c;
    var $threshold_a;
    var $alert;
    var $persistence;
    var $nat;
    var $descr;

    function Host ($ip, $hostname, $asset, $threshold_c, $threshold_a,
                   $alert, $persistence, $nat, $descr)
    {
        $this->ip          = $ip;
        $this->hostname    = $hostname;
        $this->asset       = $asset;
        $this->threshold_c = $threshold_c;
        $this->threshold_a = $threshold_a;
        $this->alert       = $alert;
        $this->persistence = $persistence;
        $this->nat         = $nat;
        $this->descr       = $descr;
    }

    function get_ip()           { return $this->ip; }
    function get_hostname()     { return $this->hostname; }
    function get_asset()        { return $this->asset; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_alert()        { return $this->alert; }
    function get_persistence()  { return $this->persistence; }
    function get_nat()          { return $this->nat; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $where = "", $order = "") 
    {
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
                                    $rs->fields["alert"],
                                    $rs->fields["persistence"],
                                    $rs->fields["nat"],
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $hostname, $asset, $threshold_c, 
                    $threshold_a, $alert, $persistence, $nat, $descr)
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");

        $query = "INSERT INTO host VALUES 
            ('$ip', '$hostname', $asset, $threshold_c,
             $threshold_a, '$alert', '$persistence', '$nat', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $ip, $hostname, $asset, $threshold_c, 
                    $threshold_a, $alert, $persistence, $nat, $descr)
    {
        settype($asset, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        settype($alert, "int");
        settype($persistence, "int");

        $query = "UPDATE host SET hostname = '$hostname', 
            ip = '$ip', asset = $asset, 
            threshold_c = $threshold_c, threshold_a = $threshold_a, 
            alert = $alert, persistence = $persistence, nat = '$nat',
            descr = '$descr' WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $ip) {
        $query = "DELETE FROM host WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function ip2hostname($conn, $ip) {
        $query = "SELECT * FROM host WHERE ip = '$ip';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["hostname"]; 
        else
            return $ip;
    }

    function ipthresh_c($conn, $ip) {
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
        if (!$rs = &$conn->Execute("SELECT * FROM host WHERE ip = '$ip'")) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

}


?>
