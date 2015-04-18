<?php

class Graph_qualification {

    var $graph_id;
    var $compromise;
    var $attack;
    
    function Graph_qualification ($graph_id, $compromise, $attack)
    {
        $this->graph_id          = $graph_id;
        $this->compromise       = $compromise;
        $this->attack           = $attack;
    }   
  
    function get_graph_id()           { return $this->graph_id; }
    function get_compromise()        { return $this->compromise; }
    function get_attack()            { return $this->attack; }
    
    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM graph_qualification $where
        $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new graph_qualification ($rs->fields["graph_id"],
                                                  $rs->fields["compromise"],
                                                  $rs->fields["attack"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $graph_id, $compromise, $attack)
        {
        $query = "INSERT INTO graph_qualification VALUES 
            ($graph_id, $compromise, $attack);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $graph_id, $compromise, $attack)
    {
        $query = "UPDATE graph_qualification SET graph_id = $graph_id, 
            compromise = $compromise, attack = $attack WHERE graph_id =
            $graph_id;";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $graph_id) {
        $query = "DELETE FROM graph_qualification WHERE graph_id = $graph_id;";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
