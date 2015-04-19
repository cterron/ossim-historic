<?php

require_once("ossim_db.inc");

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
        $query = OssimQuery("SELECT * FROM protocol $where $order");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
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
        settype($id, "int");

        $sql = "INSERT INTO protocol VALUES (NULL, ?, ?, ?, ?)";
        $params = array($id, $name, $alias, $descr);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $id, $name, $alias, $descr)
    {
        settype($id, "int");

        $sql = "UPDATE protocol SET id = ?, name = ?, alias = ?, descr = ? WHERE id = ?";
        $params = array($id, $name, $alias, $descr, $id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $id) {
        settype($id, "int");

        $sql = "DELETE FROM protocol WHERE id = ?";
        $params = array($id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
