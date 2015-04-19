<?php

class Response_plugin {

    var $response_id;
    var $plugin_id;

    function Response_plugin ($response_id, $plugin_id)
    {
        $this->response_id  = $response_id;
        $this->plugin_id    = $plugin_id;
    }

    function get_response_id()  { return $this->response_id;    }
    function get_plugin_id()    { return $this->plugin_id;      }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM response_plugin $args");
    
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Response_plugin ($rs->fields["response_id"],
                                             $rs->fields["plugin_id"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function insert($conn, $plugin_id)
    {
        $sql = "INSERT INTO response_plugin (response_id, plugin_id)
            VALUES  (LAST_INSERT_ID(), ?)";
        $params = array($plugin_id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>

