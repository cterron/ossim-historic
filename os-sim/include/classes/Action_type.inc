<?php

require_once ("ossim_db.inc");

class Action_type {

    var $type;
    var $descr;

    function Action_type ($type, $descr)
    {
        $this->type     = $type;
        $this->descr    = htmlentities($descr);
    }

    function get_type()         { return $this->type; }
    function get_descr()        { return $this->descr; }


    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM action_type $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Action_type ($rs->fields["_type"],
                                           $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
