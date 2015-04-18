<?php

require_once 'classes/Net_host_reference.inc';
require_once 'classes/Net_qualification.inc';
require_once 'classes/Control_panel_net.inc';
require_once 'ossim_conf.inc';


class Net {

    var $name;
    var $ips;
    var $priority;
    var $threshold_c;
    var $threshold_a;
    var $descr;
    
    function Net ($name, $ips, $priority, $threshold_c, $threshold_a,
        $descr)
    {
        $this->name        = $name;
        $this->ips         = $ips;
        $this->priority    = $priority;
        $this->threshold_c = $threshold_c;
        $this->threshold_a = $threshold_a;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_ips()          { return $this->ips; }
    function get_priority()     { return $this->priority; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_descr()        { return $this->descr; }

    function get_reference_ips($conn, $net_name) {
        return Net_host_reference::get_list($conn, "WHERE name = '$net_name'");
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
                                    $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name, $ips, $priority, $threshold_c, 
                    $threshold_a, $descr)
    {
        settype($priority, "int");
        settype($threshold_c, "int");
        settype($threshold_a, "int");

	$name = ereg_replace(" ", "_", $name);
	
        $query = "INSERT INTO net VALUES 
            ('$name', '$ips', $priority, $threshold_c,
             $threshold_a, '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
	$conf = new ossim_conf();
	$nmap = $conf->get_conf("nmap_path");
        $ips_sane = escapeshellcmd($ips);
        $ip_string = shell_exec("$nmap -n -sL $ips_sane | grep Host | cut -f 2 -d \" \" ");   
        $ip_list = explode("\n", $ip_string);
        $lala = array_pop($ip_list);
        
        Net_host_reference::insert($conn, $name, $ip_list);
        Net_qualification::insert($conn, $name, 1, 1);
    }

    function update($conn, $name, $ips, $priority, $threshold_c, 
                    $threshold_a, $descr)
    {
        Net::delete($conn, $name);
        Net::insert($conn, $name, $ips, $priority, 
                    $threshold_c, $threshold_a, $descr);
    }

    function delete($conn, $name) {
        $query = "DELETE FROM net WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Net_host_reference::delete($conn, $name);
        Control_panel_net::delete($conn, $name);
        Net_qualification::delete($conn, $name);
    }

    function netthresh_c($conn, $name) {
        $query = "SELECT * FROM net WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_c"]; 
        else {
            /* net is not in db, return default threshold */
            $conf = Conf::get_conf($conn);
            return $conf->get_threshold();
        }
    }

    function netthresh_a($conn, $name) {
        $query = "SELECT * FROM net WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_a"]; 
        else {
            /* net is not in db, return default threshold */
            $conf = Conf::get_conf($conn);
            return $conf->get_threshold();
        }
    }

}

?>
