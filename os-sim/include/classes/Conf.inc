<?php

class Conf {

    var $recovery;
    var $threshold;
    var $graph_threshold;
    var $bar_length_left;
    var $bar_length_right;

    function Conf ($recovery, $threshold, $graph_threshold,
                   $bar_length_left, $bar_length_right)
    {
        $this->recovery         = $recovery;
        $this->threshold        = $threshold;
        $this->graph_threshold  = $graph_threshold;
        $this->bar_length_left  = $bar_length_left;
        $this->bar_length_right = $bar_length_right;
    }

    function get_recovery()         { return $this->recovery; }
    function get_threshold()        { return $this->threshold; }
    function get_graph_threshold()  { return $this->graph_threshold; }
    function get_bar_length_left()  { return $this->bar_length_left; }
    function get_bar_length_right() { return $this->bar_length_right; }

    function get_conf($conn)
    {
        $query = "SELECT * FROM conf;";
        if (!$rs = &$conn->Execute("SELECT * FROM conf $where $order")) {
            print $conn->ErrorMsg();
        } else if (!$rs->EOF) {
            $conf = new Conf ($rs->fields["recovery"], 
                              $rs->fields["threshold"],
                              $rs->fields["graph_threshold"],
                              $rs->fields["bar_length_left"],
                              $rs->fields["bar_length_right"]);
            $rs->MoveNext();
        }
        return $conf;
    }


    function insert($conn, $recovery, $threshold, $graph_threshold,
                    $bar_length_left, $bar_length_right)
    {
        $query = "INSERT INTO conf VALUES ('$recovery', '$theshold',
        '$graph_threshold', '$bar_length_left', '$bar_length_right');" ;

        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $recovery, $threshold, $graph_threshold,
                    $bar_length_left, $bar_length_right)
    {
        $query = "UPDATE conf SET recovery='$recovery',
                    graph_threshold='$graph_threshold', 
                    threshold='$threshold', 
                    bar_length_left='$bar_length_left', 
                    bar_length_right='$bar_length_right';";

        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}
?>
