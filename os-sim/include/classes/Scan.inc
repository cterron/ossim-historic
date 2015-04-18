<?php

require_once ('classes/Conf.inc');

class Scan {

    var $ip;
    var $active;

    function Scan ($ip, $active)
    {
        $this->ip          = $ip;
        $this->active          = $active;
    }

    function get_ip()           { return $this->ip; }
    function get_active()       { return $this->active; }

    function get_list($conn) 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM scan")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Scan ($rs->fields["ip"], 
                                    $rs->fields["active"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $ip, $active)
    {
        $active = intval($active);

        $query = "INSERT INTO scan VALUES  ('$ip', '$active');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function active($conn, $ip)
    {
        $query = "UPDATE scan SET active = 1 WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function disactive($conn, $ip)
    {
        $query = "UPDATE scan SET active = 0 WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_scan($conn, $ip) {
        if (!$rs = &$conn->Execute("SELECT * FROM scan WHERE ip = '$ip'")) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }

    function is_active($conn, $ip) {
        if (!$rs = &$conn->Execute("SELECT active FROM scan WHERE ip = '$ip'"))
        {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    
    function delete($conn, $ip) {
        $query = "delete from scan where ip = '$ip';";
        if ($conn->execute($query) === false) {
            print 'error deleting: '.$conn->errormsg().'<br>';
            exit;
        }
    }

    function delete_all($conn) {
        $query = "delete from scan;";
        if ($conn->execute($query) === false) {
            print 'error deleting: '.$conn->errormsg().'<br>';
            exit;
        }
    }

}


?>
