<?php

class Host_os {

    var $ip;
    var $os;
    var $previous;
    var $anom;
    var $date;

    function Host_os ($ip, $os, $previous, $anom, $date)
    {
        $this->ip           = $ip;
        $this->os           = $os;
        $this->previous     = $previous;
        $this->anom         = $anom;
        $this->date         = $date;
    }

    function get_ip()           { return $this->ip; }
    function get_os()           { return $this->os; }
    function get_previous()     { return $this->previous; }
    function get_anom()         { return $this->anom; }
    function get_date()         { return $this->date; }

    function get_list($conn, $args = "") 
    {
        $query = "SELECT *, inet_ntoa(ip) FROM host_os $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_os ( $rs->fields["inet_ntoa(ip)"],
                                        $rs->fields["os"],
                                        $rs->fields["previous"],
                                        $rs->fields["anom"],
                                        $rs->fields["date"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function ack($conn, $host, $os)
    {
        $query = "UPDATE host_os SET anom = 0, os = '$os', previous = '$os' 
            where ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function ignore($conn, $host, $os)
    {
        $query = "UPDATE host_os SET anom = 0, os = '$os', previous = '$os'
            where ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error acking: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $host) {
        $query = "DELETE FROM host_os WHERE ip = inet_aton('$host');";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function get_os_pixmap($conn, $host, $pixmap_dir = '../pixmaps/') {

        $query = "SELECT *, inet_ntoa(ip) FROM host_os 
            WHERE ip = inet_aton('$host')";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $os = $rs->fields["os"];
            if (preg_match('/win/i', $os)) {
                return "<img src=\"$pixmap_dir/os/win.png\" alt=\"win\" />";
            } elseif (preg_match('/linux/i', $os)) {
                return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"linux\" />";
            } elseif (preg_match('/bsd/i', $os)) {
                return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"bsd\" />";
            } elseif (preg_match('/mac/i', $os)) {
                return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"mac\" />";
            } elseif (preg_match('/sun|solaris/i', $os)) {
                return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"sunos\" />";
            }
        }
    }

}


?>
