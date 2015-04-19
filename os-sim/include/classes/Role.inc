<?php
require_once 'classes/Role.inc';
require_once("ossim_db.inc");

class Role {

    var $name;
    var $correlate;
    var $cross_correlate;
    var $store;
    var $qualify;
    var $resend_alarm;
    var $resend_event;
    
    function Role ($name, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event)
    {
        $this->name = $name;
        $this->correlate = $correlate;
        $this->cross_correlate = $cross_correlate;
        $this->store = $store;
        $this->qualify = $qualify;
        $this->resend_alarm = $resend_alarm;
        $this->resend_event = $resend_event;
    }

    function get_name()            { return $this->name; }
    function get_correlate()       { return $this->correlate; }
    function get_cross_correlate() { return $this->cross_correlate; }
    function get_store()           { return $this->store; }
    function get_qualify()         { return $this->qualify; }
    function get_resend_alarm()    { return $this->resend_alarm; }
    function get_resend_event()    { return $this->resend_event; }
    
    function get_list($conn, $name) 
    {
        $list = array();
        $query = OssimQuery("SELECT * FROM server_role 
                    WHERE name = '$name'");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Role 
                                    ($rs->fields["name"],
                                     $rs->fields["correlate"],
                                     $rs->fields["cross_correlate"],
                                     $rs->fields["store"],
                                     $rs->fields["qualify"],
                                     $rs->fields["resend_alarm"],
                                     $rs->fields["resend_event"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $name)
    {
			$correlate = intval($correlate);
			$cross_correlate = intval($cross_correlate);
			$store = intval($store);
			$qualify = intval($qualify);
			$resend_alarm = intval($resend_alarm);
			$resend_event = intval($resend_event);

      $sql = "INSERT INTO server_role VALUES (?, ?, ?, ?, ?, ?, ?)"; 
            $params = array($name, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
    }
    

    function update($conn, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $name)
    {
      $correlate = intval($correlate);
      $cross_correlate = intval($cross_correlate);
      $store = intval($store);
      $qualify = intval($qualify);
      $resend_alarm = intval($resend_alarm);
      $resend_event = intval($resend_event);

      $sql = "UPDATE server_role SET name = ?, correlate  = ?,
                cross_correlate = ?, store = ?, qualify = ?, resend_alarm = ?, resend_event =? WHERE name = ?";
        $params = array($name, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function delete($conn, $name) {
        $name = intval($name);

        $sql = "DELETE FROM server_role WHERE name = ?";
        $params = array($name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
