<?php

class Protocol {

    var $id;
    var $name;
    var $alias;
    var $descr;

    function Protocol ($id, $name, $alias, $descr)
    {
        $this->id          = $id;
        $this->name        = $name;
        $this->alias       = $alias;
        $this->descr       = $descr;
    }

    function get_id()           { return $this->id; }
    function get_name()         { return $this->name; }
    function get_alias()        { return $this->alias; }
    function get_descr()        { return $this->descr; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM protocol $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new protocol ($rs->fields["id"],
                                        $rs->fields["name"],
                                        $rs->fields["alias"],
                                        $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $id, $name, $alias, $descr)
    {
        $query = "INSERT INTO protocol VALUES 
            (NULL, $id, '$name', '$alias', '$descr');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $id, $name, $alias, $descr)
    {
        $query = "UPDATE protocol SET id = $id, name = '$name', 
            alias = '$alias', descr = '$descr' WHERE id = $id;";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $id) {
        $query = "DELETE FROM protocol WHERE id = $id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
