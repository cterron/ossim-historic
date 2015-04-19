<?php

class Backlog {

    var $id;
    var $directive_id;
    var $timestamp;
    var $matched;

    function Backlog ($id, $directive_id, $timestamp, $matched)
    {
        $this->id           = $id;
        $this->directive_id = $directive_id;
        $this->timestamp    = $timestamp;
        $this->matched      = $matched;
    }

    function get_id()               { return $this->id;            }
    function get_directive_id()     { return $this->directive_id;  }
    function get_timestamp()        { return $this->timestamp;     }
    function get_matched()          { return $this->matched;       }

    function get_count($conn) 
    {
        $query = "SELECT count(*) FROM backlog;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }

    function get_list($conn, $args = "", $inf = 0, $sup = 25) 
    {
        $i = 0;

        $query = "SELECT * FROM backlog $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if (($i++ >= $inf) && ($inf++ <= $sup)) {
                    $list[] = new Backlog ($rs->fields["id"],
                                           $rs->fields["directive_id"],
                                           $rs->fields["timestamp"],
                                           $rs->fields["matched"]);
                } elseif ($inf > $sup) {
                    break;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $id) {
        if (!strcmp($id, "all")) {
            $query = "DELETE FROM backlog;";
        } else {
            $query = "DELETE FROM backlog WHERE id = '$id';";
        }
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
