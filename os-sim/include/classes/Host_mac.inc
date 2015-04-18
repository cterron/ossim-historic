<?php

class Host_mac{

    var $ip;
    var $mac;
    var $previous;
    var $anom;
    var $mac_time;

    function Host_mac($ip, $mac, $previous, $anom, $mac_time)
    {
        $this->ip           = $ip;
        $this->mac          = $mac;
        $this->previous     = $previous;
        $this->anom         = $anom;
        $this->mac_time     = $mac_time;
    }

    function get_ip()             { return $this->ip; }
    function get_mac()            { return $this->mac; }
    function get_previous()     { return $this->previous; }
    function get_anom()            { return $this->anom; }
    function get_mac_time()            { return $this->mac_time; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT * FROM host_mac $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new host_mac ( $rs->fields["ip"],
                                            $rs->fields["mac"],
                                            $rs->fields["previous"],
                                            $rs->fields["anom"],
                                            $rs->fields["mac_time"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function ack($conn, $host, $mac)
    {
        $query = "UPDATE host_mac SET anom = 0, mac = '$mac', previous = '$mac' where ip = '$host';";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function ignore($conn, $host, $mac)
    {
        $query = "UPDATE host_mac SET anom = 0, mac = '$mac', previous = '$mac' where ip = '$host';";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function delete($conn, $host) {
        $query = "DELETE FROM host_mac WHERE ip = '$host';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
