<?php

class RRD_config {

    var $profile;
    var $rrd_attrib;
    var $threshold;
    var $priority;
    var $alpha;
    var $beta;
    var $persistence;
    var $enable;
    var $description;

    function RRD_config($profile, $rrd_attrib, $threshold, $priority, $alpha,
                        $beta, $persistence, $enable, $description)
    {
        $this->profile      = $profile;
        $this->rrd_attrib   = $rrd_attrib;
        $this->threshold    = $threshold;
        $this->priority     = $priority;
        $this->alpha        = $alpha;
        $this->beta         = $beta;
        $this->persistence  = $persistence;
        $this->enable       = $enable;
        $this->description  = $description;
    }

    function get_profile()      { return $this->profile; }
    function get_rrd_attrib()   { return $this->rrd_attrib; }
    function get_threshold()    { return $this->threshold; }
    function get_priority()     { return $this->priority; }
    function get_alpha()        { return $this->alpha; }
    function get_beta()         { return $this->beta; }
    function get_persistence()  { return $this->persistence; }
    function get_enable()       { return $this->enable; }
    function get_description()  { return $this->description; }


    function get_list($conn, $args = "")
    {
        if (!$rs = &$conn->Execute("SELECT * FROM rrd_config $args")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = new RRD_config
                                ($rs->fields["profile"],
                                 $rs->fields["rrd_attrib"],
                                 $rs->fields["threshold"],
                                 $rs->fields["priority"],
                                 $rs->fields["alpha"],
                                 $rs->fields["beta"],
                                 $rs->fields["persistence"],
                                 $rs->fields["enable"],
                                 $rs->fields["description"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_profile_list($conn)
    {
        if (!$rs = &$conn->Execute("SELECT DISTINCT profile FROM rrd_config")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = $rs->fields["profile"];
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert ($conn, $profile, $rrd_attrib,
                     $threshold, $priority, $alpha, $beta,
                     $persistence, $enable, $description = "")
    {
        $query = "INSERT INTO rrd_config (
            profile, rrd_attrib, threshold, priority,
            alpha, beta, persistence, enable, description) VALUES
                ('$profile', '$rrd_attrib', '$threshold', '$priority',
                '$alpha', '$beta', '$persistence', '$enable', '$description');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update ($conn, $profile, $rrd_attrib,
                     $threshold, $priority, $alpha, $beta,
                     $persistence, $enable)
    {
        $query = "UPDATE rrd_config SET
                      threshold     = '$threshold',
                      priority      = '$priority',
                      alpha         = '$alpha',
                      beta          = '$beta',
                      persistence   = '$persistence',
                      enable        = '$enable'
                  WHERE profile = '$profile' AND rrd_attrib = '$rrd_attrib';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $profile) {
        $query = "DELETE FROM rrd_config WHERE profile = '$profile';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

