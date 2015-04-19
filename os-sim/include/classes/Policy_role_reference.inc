<?php

require_once("ossim_db.inc");

class Policy_role_reference {

    var $policy_id;
    var $correlate;
    var $cross_correlate;
    var $store;
    var $qualify;
    var $resend_alarm;
    var $resend_event;
    
    function Policy_role_reference ($policy_id, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event)
    {
        $this->policy_id = $policy_id;
        $this->correlate = $correlate;
        $this->cross_correlate = $cross_correlate;
        $this->store = $store;
        $this->qualify = $qualify;
        $this->resend_alarm = $resend_alarm;
        $this->resend_event = $resend_event;
    }

    function get_policy_id()       { return $this->policy_id; }
    function get_correlate()       { return $this->correlate; }
    function get_cross_correlate() { return $this->cross_correlate; }
    function get_store()           { return $this->store; }
    function get_qualify()         { return $this->qualify; }
    function get_resend_alarm()    { return $this->resend_alarm; }
    function get_resend_event()    { return $this->resend_event; }
    
    function get_list($conn, $policy_id) 
    {
        $list = array();
        $policy_id = intval($policy_id);
        $query = OssimQuery("SELECT * FROM policy_role_reference 
                    WHERE policy_id = $policy_id");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Policy_role_reference 
                                    ($rs->fields["policy_id"],
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

    function insert($conn, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $policy_id)
    {
			$policy_id = intval($policy_id);
			$correlate = intval($correlate);
			$cross_correlate = intval($cross_correlate);
			$store = intval($store);
			$qualify = intval($qualify);
			$resend_alarm = intval($resend_alarm);
			$resend_event = intval($resend_event);

      $sql = "INSERT INTO policy_role_reference VALUES (?, ?, ?, ?, ?, ?, ?)"; 
            $params = array($policy_id, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event);
            if (!$conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
    }
    

    function update($conn, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $policy_id)
    {
      $policy_id = intval($policy_id);
      $correlate = intval($correlate);
      $cross_correlate = intval($cross_correlate);
      $store = intval($store);
      $qualify = intval($qualify);
      $resend_alarm = intval($resend_alarm);
      $resend_event = intval($resend_event);

      $sql = "UPDATE policy_role_reference SET policy_id = ?, correlate  = ?,
                cross_correlate = ?, store = ?, qualify = ?, resend_alarm = ?, resend_event =? WHERE policy_id = ?";
        $params = array($policy_id, $correlate, $cross_correlate, $store, $qualify, $resend_alarm, $resend_event, $policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function delete($conn, $policy_id) {
        $policy_id = intval($policy_id);

        $sql = "DELETE FROM policy_role_reference WHERE policy_id = ?";
        $params = array($policy_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
