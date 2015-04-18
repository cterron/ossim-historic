<?php

class Signature {

    var $name;

    function Signature ($name)
    {
        $this->name        = $name;
    }

    function get_name()         { return $this->name; }

    function get_list($conn, $where = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM signature $where")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Signature ($rs->fields["name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $name)
    {
        $query = "INSERT INTO signature VALUES ('$name');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $name, $ips, $priority, $threshold_c, 
                    $threshold_a, $descr)
    {
        $query = "UPDATE signature SET name = '$name' WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $name) {
        $query = "DELETE FROM signature WHERE name = '$name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
