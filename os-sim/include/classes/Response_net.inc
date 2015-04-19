<?php

require_once("ossim_db.inc");

class Response_net {

    var $response_id;
    var $net;
    var $type;

    function Response_net ($response_id, $net, $type)
    {
        $this->response_id  = $response_id;
        $this->net          = $net;
        $this->type         = $type;
    }

    function get_response_id()  { return $this->response_id;    }
    function get_net()          { return $this->net;            }
    function get_type()         { return $this->type;           }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM response_net $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Response_net ($rs->fields["response_id"],
                                          $rs->fields["net"],
                                          $rs->fields["_type"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $net, $type)
    {
        $sql = "INSERT INTO response_net (response_id, net, _type) VALUES  (LAST_INSERT_ID(), ?, ?)";
        $params = array($net, $type);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

