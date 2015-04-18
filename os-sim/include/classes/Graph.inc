<?php

require_once ('classes/Link.inc');

class Graph {

    var $id;
    var $ip;

    function Graph ($id, $ip)
    {
        $this->id = $id;
        $this->ip = $ip;
    }

    function get_id()      { return $this->id; }
    function get_ip()      { return $this->ip; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM graph $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Graph ($rs->fields["id"], 
                                     $rs->fields["ip"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_link_list($conn, $graph_id) 
    {
        $query = "SELECT DISTINCT l.source, l.dest, l.occurrences
            FROM link l, graph g WHERE g.id = $graph_id AND
            (g.ip = l.source or g.ip = l.dest) ORDER BY l.occurrences desc;";
            
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Link ($rs->fields[0],
                                    $rs->fields[1],
                                    $rs->fields[2]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_id_list($conn) {
        $query = "SELECT DISTINCT id FROM graph;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = $rs->fields[0];
                $rs->MoveNext();
            }
        }
        return $list;
    }
}


?>
