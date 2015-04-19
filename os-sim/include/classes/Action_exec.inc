<?php

require_once("ossim_db.inc");

class Action_exec {

    var $action_id;
    var $command;

    function Action_exec ($action_id, $command)
    {
        $this->action_id    = $action_id;
        $this->command      = mysql_escape_string($command);
    }

    function get_action_id()    { return $this->action_id; }
    function get_command()      { return $this->command; }


    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM action_exec $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Action_exec ($rs->fields["action_id"],
                                           $rs->fields["command"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $command)
    {
        $sql = "INSERT INTO action_exec (action_id, command) VALUES (LAST_INSERT_ID(), ?)";
        $params = array($command);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $id, $command)
    {
        $sql = "UPDATE action_exec SET command = ? WHERE action_id = ?";
        $params = array($command, $id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
