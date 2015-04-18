<?php

class Host_mac{

    var $ip;
    var $mac;
    var $previous;
    var $anom;
    var $date;

    function Host_mac($ip, $mac, $vendor, $previous, $anom, $date)
    {
        $this->ip           = $ip;
        $this->mac          = $mac;
        $this->vendor       = $vendor;
        $this->previous     = $previous;
        $this->anom         = $anom;
        $this->date     = $date;
    }

    function get_ip()           { return $this->ip; }
    function get_mac()          { return $this->mac; }
    function get_vendor()       { return $this->vendor; }
    function get_previous()     { return $this->previous; }
    function get_anom()         { return $this->anom; }
    function get_date()         { return $this->date; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT *, inet_ntoa(ip) FROM host_mac $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new host_mac ( $rs->fields["inet_ntoa(ip)"],
                                            $rs->fields["mac"],
                                            $rs->fields["vendor"],
                                            $rs->fields["previous"],
                                            $rs->fields["anom"],
                                            $rs->fields["date"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function ack($conn, $host, $mac)
    {
        $query = "UPDATE host_mac SET anom = 0, mac = '$mac', 
            previous = '$mac' where ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function ignore($conn, $host, $mac)
    {
        $query = "UPDATE host_mac SET anom = 0, mac = '$mac', 
            previous = '$mac' where ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


    function delete($conn, $host) {
        $query = "DELETE FROM host_mac WHERE ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
