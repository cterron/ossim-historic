<?php

class Host_qualification {

    var $host_ip;
    var $compromise;
    var $attack;
    
    function Host_qualification ($host_ip, $compromise, $attack)
    {
        $this->host_ip          = $host_ip;
        $this->compromise       = $compromise;
        $this->attack           = $attack;
    }   
  
    function get_host_ip()           { return $this->host_ip; }
    function get_compromise()        { return $this->compromise; }
    function get_attack()            { return $this->attack; }

    function get_ip_attack($conn, $ip){
        if (!$rs = &$conn->Execute("SELECT * FROM host_qualification WHERE host_ip = '$ip'")){
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF)
                return $rs->fields["attack"];
            else {
                /* there is no attack level for this host */
                return 0;
            }
        }
    }

    function get_ip_compromise($conn, $ip){
        if (!$rs = &$conn->Execute("SELECT * FROM host_qualification WHERE host_ip = '$ip'")){
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF)
                return $rs->fields["compromise"];
            else {
                /* there is no compromise level for this host */
                return 0;
            }
        }
    }

    function __get_global($conn, $type)
    {
        if (($type != 'compromise') and ($type != 'attack'))
            return -1;

        $query = "SELECT sum($type) FROM host_qualification;";
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            if (!$rs->EOF) {

                if (NULL == ($result = $rs->fields["sum($type)"])) {
                
                    /* no hosts -> sum == null */
                    return 0;
                }
                return $result;                

            } else {
                return 0;
            }
        }
    }
    
    function get_global_compromise($conn) 
    {
        return Host_qualification::__get_global($conn, "compromise");
    }

    function get_global_attack($conn) 
    {
        return Host_qualification::__get_global($conn, "attack");
    }


    function get_list($conn, $where = "", $order = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM host_qualification $where
        $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Host_qualification ($rs->fields["host_ip"],
                                                  $rs->fields["compromise"],
                                                  $rs->fields["attack"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_ip, $compromise, $attack) 
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $query = "INSERT INTO host_qualification VALUES 
            ('$host_ip', $compromise, $attack);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $host_ip, $compromise, $attack)
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $query = "UPDATE host_qualification SET host_ip = '$host_ip', 
            compromise = $compromise, attack = $attack WHERE host_ip =
            '$host_ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $host_ip) {
        $query = "DELETE FROM host_qualification WHERE host_ip = '$host_ip';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
