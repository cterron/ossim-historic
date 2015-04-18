<?php

class Host_os{

    var $ip;
    var $os;
    var $previous;
    var $anom;
    var $os_time;

    function Host_os ($ip, $os, $previous, $anom, $os_time)
    {
        $this->ip           = $ip;
        $this->os           = $os;
        $this->previous     = $previous;
        $this->anom         = $anom;
        $this->os_time      = $os_time;
    }

    function get_ip()             { return $this->ip; }
    function get_os()            { return $this->os; }
    function get_previous()     { return $this->previous; }
    function get_anom()            { return $this->anom; }
    function get_os_time()            { return $this->os_time; }

    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM host_os $where $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_os ( $rs->fields["ip"],
                                            $rs->fields["os"],
                                            $rs->fields["previous"],
                                            $rs->fields["anom"],
                                            $rs->fields["os_time"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function ack($conn, $host, $os)
    {
        $query = "UPDATE host_os SET anom = 0, os = '$os', previous = '$os' where ip = '$host';";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $host) {
        $query = "DELETE FROM host_os WHERE ip = '$host';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
