<?php

class Plugin {

    var $id;
    var $type;
    var $name;
    var $description;

    function Plugin ($id, $type, $name, $description)
    {
        $this->id           = $id;
        $this->type         = $type;
        $this->name         = $name;
        $this->description  = $description;
    }

    function get_id()           { return $this->id; }
    function get_type()         { return $this->type; }
    function get_name()         { return $this->name; }
    function get_description()  { return $this->description; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM plugin $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Plugin ($rs->fields["id"],
                                      $rs->fields["type"],
                                      $rs->fields["name"],
                                      $rs->fields["description"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
