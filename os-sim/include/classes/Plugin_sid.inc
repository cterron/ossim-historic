<?php

class Plugin_sid {

    var $plugin_id;
    var $sid;
    var $category_id;
    var $class_id;
    var $name;
    var $reliability;
    var $priority;

    function Plugin_sid ($plugin_id, $sid, $category_id, $class_id, 
                         $name, $reliability, $priority)
    {
        $this->plugin_id    = $plugin_id;
        $this->sid          = $sid;
        $this->category_id  = $category_id;
        $this->class_id     = $class_id;
        $this->name         = $name;
        $this->reliability  = $reliability;
        $this->priority     = $priority;
    }

    function get_plugin_id()    { return $this->plugin_id; }
    function get_sid()          { return $this->sid; }
    function get_category_id()  { return $this->category_id; }
    function get_class_id()     { return $this->class_id; }
    function get_name()         { return $this->name; }
    function get_reliability()  { return $this->reliability; }
    function get_priority()     { return $this->priority; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM plugin_sid $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Plugin_sid ($rs->fields["plugin_id"],
                                          $rs->fields["sid"],
                                          $rs->fields["category_id"],
                                          $rs->fields["class_id"],
                                          $rs->fields["name"],
                                          $rs->fields["reliability"],
                                          $rs->fields["priority"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function update($conn, $id, $sid, $priority, $reliability) 
    {
        settype($priority, "int");
        settype($reliability, "int");

        $query = "UPDATE plugin_sid SET priority = $priority, 
                    reliability = $reliability  WHERE plugin_id = $id AND 
                    sid = $sid;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
    }
}

?>

