<?php

require_once("ossim_db.inc");

class Category {

    var $id;
    var $name;

    function Category ($id, $name)
    {
        $this->id           = $id;
        $this->name         = $name;
    }

    function get_id()           { return $this->id; }
    function get_name()         { return $this->name; }

    function get_list($conn, $args = "") 
    {
        $query = OssimQuery("SELECT * FROM category $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Category ($rs->fields["id"],
                                      $rs->fields["name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>

