<?php

class Classification {

    var $id;
    var $name;
    var $description;
    var $priority;

    function Classification ($id, $name, $description, $priority)
    {
        $this->id           = $id;
        $this->name         = $name;
        $this->description  = $description;
        $this->priority     = $priority;
    }

    function get_id()           { return $this->id; }
    function get_name()         { return $this->name; }
    function get_description()  { return $this->description; }
    function get_priority()     { return $this->priority; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM classification $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Classification ($rs->fields["id"],
                                      $rs->fields["name"],
                                      $rs->fields["description"],
                                      $rs->fields["priority"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
}

?>
