<?php

class Host {

    var $ip;
    var $hostname;
    var $asset;
    var $threshold_c;
    var $threshold_a;
    var $active;
    var $descr;

    function Host ($ip, $hostname, $asset, $threshold_c, $threshold_a,
                   $descr)
    {
        $this->ip          = $ip;
        $this->hostname    = $hostname;
        $this->asset    = $asset;
        $this->threshold_c = $threshold_c;
        $this->threshold_a = $threshold_a;
        $this->descr       = $descr;
    }

    function get_ip()           { return $this->ip; }
    function get_hostname()     { return $this->hostname; }
    function get_asset()     { return $this->asset; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM host $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host ($rs->fields["ip"], 
                                    $rs->fields["hostname"],
                                    $rs->fields["asset"],
                                    $rs->fields["threshold_c"],
                                    $rs->fields["threshold_a"],
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $hostname, $asset, $threshold_c, 
                    $threshold_a, $descr)
    {
        $query = "INSERT INTO host VALUES 
            ('$ip', '$hostname', $asset, $threshold_c,
             $threshold_a, '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $ip, $hostname, $asset, $threshold_c, 
                    $threshold_a, $descr)
    {
        $query = "UPDATE host SET hostname = '$hostname', 
            ip = '$ip', asset = $asset, 
            threshold_c = $threshold_c, threshold_a = $threshold_a, 
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
        if ($conn->Execute($query) === false)  {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if ($rs)
           return $rs->fields["hostname"]; 
        else
            return $ip;
    }

}


?>
