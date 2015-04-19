<?php

require_once ('classes/Log_action.inc');
require_once ("ossim_db.inc");

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
        $query = OssimQuery("SELECT * FROM rrd_config $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        $sql = "SELECT DISTINCT profile FROM rrd_config";
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        $profile = htmlentities($profile);
        $rrd_attrib = htmlentities($rrd_attrib);
        $description = htmlentities($description);
        $enable = htmlentities($enable);
        $threshold = intval($threshold);
        $priority = intval($priority);
        $alpha = intval($alpha);
        $beta = intval($beta);
        $persistence = intval($persistence);

        $sql = "INSERT INTO rrd_config (profile, rrd_attrib, threshold, priority, alpha, beta, persistence, enable, description)
                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
        $params = array($profile, $rrd_attrib, $threshold, $priority, $alpha, $beta, $persistence, $enable, $description);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Log_action::log(9,NULL);
    }

    function update ($conn, $profile, $rrd_attrib,
                     $threshold, $priority, $alpha, $beta,
                     $persistence, $enable)
    {
        $sql = "UPDATE rrd_config SET
                      threshold     = ?,
                      priority      = ?,
                      alpha         = ?,
                      beta          = ?,
                      persistence   = ?,
                      enable        = ?
                  WHERE profile = ? AND rrd_attrib = ?";
        $params = array($threshold, $priority, $alpha, $beta, $persistence, $enable, $profile, $rrd_attrib);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $profile) {
        $sql = "DELETE FROM rrd_config WHERE profile = ?";
        $params = array($profile);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>
