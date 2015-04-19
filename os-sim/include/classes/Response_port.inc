<?php

require_once("ossim_db.inc");

class Response_port {

    var $response_id;
    var $port;
    var $type;

    function Response_port ($response_id, $port, $type)
    {
        $this->response_id  = $response_id;
        $this->port         = $port;
        $this->type         = $type;
    }

    function get_response_id()  { return $this->response_id;    }
    function get_port()         { return $this->port;           }
    function get_type()         { return $this->type;           }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM response_port $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Response_port ($rs->fields["response_id"],
                                           $rs->fields["port"],
                                           $rs->fields["_type"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $port, $type)
    {
        $sql = "INSERT INTO response_port (response_id, port, _type)
            VALUES  (LAST_INSERT_ID(), ?, ?)";
        $params = array($port, $type);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

