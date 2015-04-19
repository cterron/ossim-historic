<?php

require_once("ossim_db.inc");

class Action_email {

    var $action_id;
    var $from;
    var $to;
    var $subject;
    var $message;

    function Action_email ($action_id, $from, $to, $subject, $message)
    {
        $this->action_id    = $action_id;
        $this->from         = mysql_escape_string($from);
        $this->to           = mysql_escape_string($to);
        $this->subject      = mysql_escape_string($subject);
        $this->message      = mysql_escape_string($message);
    }

    function get_action_id()    { return $this->action_id; }
    function get_from()         { return $this->from; }
    function get_to()           { return $this->to; }
    function get_subject()      { return $this->subject; }
    function get_message()      { return $this->message; }



    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM action_email $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Action_email ($rs->fields["action_id"],
                                            $rs->fields["_from"],
                                            $rs->fields["_to"],
                                            $rs->fields["subject"],
                                            $rs->fields["message"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $from, $to, $subject, $message)
    {
        $sql = "INSERT INTO action_email (action_id, _from, _to, subject, message)
                VALUES (LAST_INSERT_ID(), ?, ?, ?, ?)";
        $params = array($from, $to, $subject, $message);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $id, $from, $to, $subject, $message)
    {
        $sql = "UPDATE action_email SET _from = ?, _to = ?, subject = ?, message = ? 
                WHERE action_id = ?";
        $params = array($from, $to, $subject, $message, $id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>
