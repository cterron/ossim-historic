<?php

require_once ("ossim_db.inc");

class Signature {

    var $name;

    function Signature ($name)
    {
        $this->name        = $name;
    }

    function get_name()         { return $this->name; }

    function get_list($conn, $where = "") 
    {
        $query = OssimQuery("SELECT * FROM signature $where");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Signature ($rs->fields["name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name)
    {
        $sql = "INSERT INTO signature VALUES (?)";
        $params = array($name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $name, $ips, $priority, $threshold_c, 
                    $threshold_a, $descr)
    {
        $sql = "UPDATE signature SET name = ? WHERE name = ?";
        $params = array($name, $name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $name) {
        $sql = "DELETE FROM signature WHERE name = ?'";
        $params = array($name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
