<?php

class Net_host_reference {

    var $net_name;
    var $host_ip;
    
    function Net_host_reference ($net_name, $host_ip)
    {
        $this->net_name  = $net_name;
        $this->host_ip   = $host_ip;    
    }

    function get_net_name()   { return $this->net_name; }
    function get_host_ip()  { return $this->host_ip; }
    
    function get_list($conn, $where = "", $order = "") 
    {
        $query = "SELECT * FROM net_host_reference $where $order";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_host_reference ($rs->fields["net_name"],
                                                  $rs->fields["host_ip"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_name, $ip_list)
    {
        foreach ($ip_list as $ip) {
            $query = "INSERT INTO net_host_reference VALUES 
                      ('$net_name', '$ip');";
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

/*
    function update($conn, $net_name, $host_ip)
    {
        $query = "UPDATE net_host_reference SET host_ip = '$host_ip', 
            net_name = '$net_name' WHERE net_name = '$net_name'";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
*/
    function delete($conn, $net_name) 
    {
        $query = "DELETE FROM net_host_reference WHERE net_name = '$net_name'";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
