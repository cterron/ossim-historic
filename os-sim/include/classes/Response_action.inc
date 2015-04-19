<?php

require_once("ossim_db.inc");

class Response_action {

    var $response_id;
    var $action_id;

    function Response_action ($response_id, $action_id)
    {
        $this->response_id  = $response_id;
        $this->action_id    = $action_id;
    }

    function get_response_id()  { return $this->response_id;    }
    function get_action_id()    { return $this->action_id;      }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM response_action $args");    
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Response_action ($rs->fields["response_id"],
                                               $rs->fields["action_id"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $action_id)
    {
        $sql = "INSERT INTO response_action (response_id, action_id)
            VALUES  (LAST_INSERT_ID(), ?)";
        $params = array($action_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

