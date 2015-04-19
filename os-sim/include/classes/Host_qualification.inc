<?php

require_once ('classes/Net.inc');
require_once ('classes/Session.inc');
require_once ("ossim_db.inc");

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
        $sql = "SELECT * FROM host_qualification WHERE host_ip = ?";
        $params = array($ip);
        if (!$rs = &$conn->Execute($sql, $params)){
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
        $sql = OssimQuery("SELECT * FROM host_qualification WHERE host_ip = ?");
        $params = array($ip);
    if (!$rs = &$conn->Execute($sql, $params)){
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

        $global = 0;

        $sql = "SELECT $type, host_ip FROM host_qualification";
        
        if (!$rs = &$conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if (Session::hostAllowed($conn, $rs->fields["host_ip"]))
                {
                    $global += $rs->fields[$type];
                }
                $rs->MoveNext();
            }
        }
        return $global;
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
        $query = OssimQuery("SELECT * FROM host_qualification $where
        $order");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                if (Session::hostAllowed($conn, $rs->fields["host_ip"]))
                {
                    $list[] = new Host_qualification (
                                                $rs->fields["host_ip"],
                                                $rs->fields["compromise"],
                                                $rs->fields["attack"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $host_ip, $compromise, $attack) 
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $sql = "INSERT INTO host_qualification VALUES (?, ?, ?)";
        $params = array($host_ip, $compromise, $attack);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $host_ip, $compromise, $attack)
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $sql = "UPDATE host_qualification SET host_ip = ?, compromise = ?, attack = ? WHERE host_ip = ?";
        $params = array($host_ip, $compromise, $attack, $host_ip);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $host_ip) {
        $sql = "DELETE FROM host_qualification WHERE host_ip = ?";
        $params = array($host_ip);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
