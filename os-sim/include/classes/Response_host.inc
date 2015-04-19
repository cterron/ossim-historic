<?php

require_once("ossim_db.inc");

class Response_host {

    var $response_id;
    var $host;
    var $type;

    function Response_host ($response_id, $host, $type)
    {
        $this->response_id  = $response_id;
        $this->host         = $host;
        $this->type         = $type;
    }

    function get_response_id()  { return $this->response_id;    }
    function get_host()         { return $this->host;           }
    function get_type()         { return $this->type;           }

    function get_list($conn, $args = "") 
    {
        require_once ('classes/Session.inc');
   
        $query = OssimQuery("SELECT * FROM response_host $args");
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                if (Session::hostAllowed($conn, $rs->fields["host"]))
                {
                    $list[] = new Response_host ($rs->fields["response_id"],
                                                 $rs->fields["host"],
                                                 $rs->fields["_type"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $host, $type)
    {
        $sql = "INSERT INTO response_host (response_id, host, _type)
            VALUES  (LAST_INSERT_ID(), ?, ?)";
        $params = array($host, $type);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

