<?php

class Link {

    var $source;
    var $dest;
    var $occurrences;

    function Link ($source, $dest, $occurrences)
    {
        $this->source      = $source;
        $this->dest        = $dest;
        $this->occurrences = $occurrences;
    }

    function get_source()       { return $this->source; }
    function get_dest()         { return $this->dest; }
    function get_occurrences()  { return $this->occurrences; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM link $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Link ($rs->fields["source"], 
                                    $rs->fields["dest"],
                                    $rs->fields["occurrences"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

}


?>
