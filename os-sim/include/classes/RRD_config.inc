<?php

class RRD_config {

    var $ip;
    var $rrd_attrib;
    var $threshold;
    var $priority;
    var $alpha;
    var $beta;
    var $persistence;
    var $description;

    function RRD_config($ip, $rrd_attrib, $threshold, $priority, $alpha,
                        $beta, $persistence, $description)
    {
        $this->ip           = $ip;
        $this->rrd_attrib   = $rrd_attrib;
        $this->threshold    = $threshold;
        $this->priority     = $priority;
        $this->alpha        = $alpha;
        $this->beta         = $beta;
        $this->persistence  = $persistence;
        $this->description  = $description;
    }

    function get_ip()           { return $this->ip; }
    function get_rrd_attrib()   { return $this->rrd_attrib; }
    function get_threshold()    { return $this->threshold; }
    function get_priority()     { return $this->priority; }
    function get_alpha()        { return $this->alpha; }
    function get_beta()         { return $this->beta; }
    function get_persistence()  { return $this->persistence; }
    function get_description()  { return $this->description; }


    function get_list($conn, $args = "") 
    {
        if (!$rs = &$conn->Execute("SELECT *, inet_ntoa(ip) FROM rrd_config $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = new RRD_config 
                                ($rs->fields["inet_ntoa(ip)"],
                                 $rs->fields["rrd_attrib"],
                                 $rs->fields["threshold"],
                                 $rs->fields["priority"],
                                 $rs->fields["alpha"],
                                 $rs->fields["beta"],
                                 $rs->fields["persistence"],
                                 $rs->fields["description"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_ip_list($conn)
    {
        if (!$rs = &$conn->Execute("SELECT DISTINCT inet_ntoa(ip) FROM rrd_config")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = $rs->fields["inet_ntoa(ip)"];
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert ($conn, $ip, $rrd_attrib, 
                     $threshold, $priority, $alpha, $beta, 
                     $persistence, $description = "")
    {
        $query = "INSERT INTO rrd_config VALUES 
                    (inet_aton('$ip'), '$rrd_attrib', '$threshold', '$priority', 
                     '$alpha', '$beta', '$persistence', '$description');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update ($conn, $ip, $rrd_attrib, 
                     $threshold, $priority, $alpha, $beta, 
                     $persistence)
    {
        $query = "UPDATE rrd_config SET 
                      threshold     = '$threshold', 
                      priority      = '$priority',
                      alpha         = '$alpha',
                      beta          = '$beta',
                      persistence   = '$persistence'
                  WHERE ip = inet_aton('$ip') AND rrd_attrib = '$rrd_attrib';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $ip) {
        $query = "DELETE FROM rrd_config WHERE ip = inet_aton('$ip');";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

