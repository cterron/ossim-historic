<?php

require_once 'classes/Net_group_reference.inc';
require_once 'ossim_conf.inc';


class Net_group {

    var $name;
    var $threshold_c;
    var $threshold_a;
    var $rrd_profile;
    var $descr;

    function Net_group ($name, $threshold_c, $threshold_a, $rrd_profile, $descr)
    {
        $this->name        = stripslashes($name);
        $this->threshold_c = $threshold_c;
        $this->threshold_a = $threshold_a;
        $this->rrd_profile = $rrd_profile;
        $this->descr       = $descr;
    }

    function get_name()         { return $this->name; }
    function get_threshold_c()  { return $this->threshold_c; }
    function get_threshold_a()  { return $this->threshold_a; }
    function get_rrd_profile()  { return $this->rrd_profile; }
    function get_descr()        { return $this->descr; }


    function get_networks($conn, $name = "") {
        if($name != ""){
        return Net_group_reference::get_list($conn, $name);
        } else {
        return Net_group_reference::get_list($conn, $this->name);
        }
    }


    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM net_group $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_group  (stripslashes($rs->fields["name"]),
                                    $rs->fields["threshold_c"],
                                    $rs->fields["threshold_a"],
                                    stripslashes($rs->fields["rrd_profile"]),
                                    stripslashes($rs->fields["descr"]));
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_compromise($conn, $name){
        $compromise = 0;
        $networks = Net_group::get_networks($conn, $name);
        $name = stripslashes($name);
        $name = mysql_real_escape_string($name);

        foreach($networks as $network){
            $name = $network->get_net_name();
            $net_list = Net_qualification::get_list($conn, "WHERE net_name = '$name'");
            foreach($net_list as $net){
                $compromise += $net->get_compromise();
            }
        }

        return $compromise;
    }

    function get_attack($conn, $name){
        $attack = 0;
        $networks = Net_group::get_networks($conn, $name);
        $name = stripslashes($name);
        $name = mysql_real_escape_string($name);

        foreach($networks as $network){
            $name = $network->get_net_name();
            $net_list = Net_qualification::get_list($conn, "WHERE net_name = '$name'");
            foreach($net_list as $net){
                $attack += $net->get_attack();
            }
        }

        return $attack;
    
    }



    function insert($conn, $name, $threshold_c, $threshold_a, $rrd_profile, $nets, $descr)
    {
        settype($threshold_c, "int");
        settype($threshold_a, "int");
        $name =stripslashes($name);
        $name = mysql_escape_string($name);
        $descr = stripslashes($descr);
        $descr = mysql_escape_string($descr);
        $rrd_profile = htmlentities($rrd_profile);

        $name = str_replace(" ", "_", $name);

        $query = "INSERT INTO net_group ( name, threshold_c, threshold_a, rrd_profile, descr) 
        VALUES ('$name', $threshold_c, $threshold_a, '$rrd_profile', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
        }
        $name = stripslashes($name);
        $descr = stripslashes($name);
        Net_group_reference::insert($conn, $name, $nets);
    }

    function update($conn, $name, $threshold_c, $threshold_a, $rrd_profile, $nets, $descr)
    {
        Net_group::delete($conn, $name);
        Net_group::insert($conn, $name, $threshold_c, $threshold_a, $rrd_profile, $nets, $descr);
    }

    function delete($conn, $name) {
        $name = stripslashes($name);
        $name = mysql_real_escape_string($name);
        $query = "DELETE FROM net_group WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        $name = stripslashes($name);
        Net_group_reference::delete($conn, $name);
    }

    function netthresh_c($conn, $name) {
        $name = stripslashes($name);
        $name = mysql_real_escape_string($name);
        $query = "SELECT * FROM net_group WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_c"]; 
        else {
            /* network group is not in db, return default threshold */
            $conf = new ossim_conf();
            return $conf->get_conf("threshold");
        }
    }

    function netthresh_a($conn, $name) {
        $name = stripslashes($name);
        $name = mysql_real_escape_string($name);
        $query = "SELECT * FROM net_group WHERE name = '$name';";
        if (!$rs = $conn->Execute($query)) {
            print 'error: '.$conn->ErrorMsg().'<BR>';
            exit;
        } 
        if (!$rs->EOF)
            return $rs->fields["threshold_a"]; 
        else {
            /* network group is not in db, return default threshold */
            $conf = new ossim_conf();
            return $conf->get_conf("threshold");
        }
    }

    function isNetInGroup($conn, $group, $net)
    {
        $group = stripslashes($group);
        $group = mysql_real_escape_string($group);
        $net = stripslashes($net);
        $net = mysql_real_escape_string($net);

        $query = "SELECT * from net_group_reference where net_group_name =
        '" . $group . "' AND net_name = '". $net ."'";

        if ($rs = &$conn->Execute($query) && (!$rs->EOF)){
            return True;
        } 

        return False;
    }

    function enable_nessus($conn, $net_group_name)
    {
        $net_group_name = stripslashes($net_group_name);
        $net_group_name = mysql_real_escape_string($net_group_name);
        $query = "INSERT INTO net_group_scan (net_group_name, plugin_id, plugin_sid) 
            VALUES ('$net_group_name', 3001, 0)";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    function disable_nessus($conn, $net_group_name)
    {
        $net_group_name = stripslashes($net_group_name);
        $net_group_name = mysql_real_escape_string($net_group_name);
        $query = "DELETE FROM net_group_scan 
            WHERE net_group_name = '$net_group_name' AND plugin_id = 3001";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function isNetInAnyGroup($conn, $net)
    {
        $group = stripslashes($group);
        $group = mysql_real_escape_string($group);
        $net = stripslashes($net);
        $net = mysql_real_escape_string($net);

        $query = "SELECT * from net_group_reference where net_name = '". $net ."'";

        if ($rs = &$conn->Execute($query) && (!$rs->EOF)){
            return True;
        } 

        return False;
    }

    function check_orphan($conn)
    {
        $query =  "SELECT * FROOM net_sensor_reference LEFT JOIN sensor ON sensor.name = net_sensor_reference.sensor_name WHERE name IS NULL";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Net_sensor_rerefence::delete($conn, $rs->fields["alert_id"]);
                $rs->MoveNext();
            }
        }
    }



}

?>
