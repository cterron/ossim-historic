<?php

require_once ('classes/Action_email.inc');
require_once ('classes/Action_exec.inc');
require_once ("ossim_db.inc");

class Action {

    var $id;
    var $action_type;
    var $descr;

    function Action ($id, $action_type, $descr)
    {
        $this->id          = $id;
        $this->action_type = $action_type;
        $this->descr       = htmlentities($descr);
    }

    function get_id()           { return $this->id;          }
    function get_action_type()  { return $this->action_type; }
    function get_descr()        { return $this->descr;       }

    function get_action_by_id($conn, $id)
    {
        $sql = "SELECT descr FROM action WHERE id = ?";
        $params = array($id);

        if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF) {
                $action = new Action ($rs->fields["id"],
                                      $rs->fields["action_type"],
                                      $rs->fields["descr"]);
            }
        }
        return $action;
 
    }

    /* get an action object of email or exec classes */
    function get_action($conn)
    {
        $id = $this->id;
        $type = $this->action_type;
        
        if ($type == "email")
        {
            if (is_array($action_list = 
                Action_email::get_list($conn, " WHERE action_id = '$id' ")))
            {
                return $action_list[0];
            }
            return null;
        }
        elseif ($type == "exec")
        {
            if (is_array($action_list = 
                Action_exec::get_list($conn, " WHERE action_id = '$id' ")))
            {
                return $action_list[0];
            }
            return null;
        } 
        else 
        {
            return null;
        }
    }


    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM action $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Action ($rs->fields["id"],
                                      $rs->fields["action_type"],
                                      $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $action_type, $descr)
    {
        $sql = "INSERT INTO action (id, action_type, descr) VALUES (NULL, ?, ?)";
        $params = array($action_type, $descr);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function insertEmail($conn, $action_type, $descr,
                         $from, $to, $subject, $message)
    {
        Action::insert($conn, $action_type, $descr);
        Action_email::insert($conn, $from, $to, $subject, $message);
    }

    function insertExec($conn, $action_type, $descr, $command)
    {
        Action::insert($conn, $action_type, $descr);
        Action_exec::insert($conn, $command);
    }

    function update($conn, $id, $action_type, $descr)
    {
        $sql = "UPDATE action SET action_type = ?, descr = ? WHERE id = ?";
        $params = array($action_type, $descr, $id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function updateEmail($conn, $id, $action_type, $descr,
                         $from, $to, $subject, $message)
    {
        Action::update($conn, $id, $action_type, $descr);
        Action_email::update($conn, $id, $from, $to, $subject, $message);
    }

    function updateExec($conn, $id, $action_type, $descr, $command)
    {
        Action::update($conn, $id, $action_type, $descr);
        Action_exec::update($conn, $id, $command);
    }


    function delete($conn, $id)
    {
	$Action = Action::get_action_by_id($conn, $id);
        $type = $Action->action_type;
        
        $sql  = "DELETE FROM action WHERE id = ?";
        $params = array($id);
        if ($conn->Execute($sql, $params) === false) {
            print "error deleting ($sql): ".$conn->ErrorMsg().'<BR>';
            exit;
        }

        if ($type != "") {
            $sql = "DELETE FROM action_$type WHERE action_id = ?";
            $params = array($id);
            if ($conn->Execute($sql, $params) === false) {
                echo "error deleting ($sql): ".$conn->ErrorMsg().'<BR>';
                exit;
            }
        }

        $sql = "DELETE FROM response_action WHERE action_id = ?";
        $params = array($id);
        if ($conn->Execute($sql, $params) === false) {
            echo "error deleting ($sql): ".$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>

