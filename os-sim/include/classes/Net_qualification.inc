<?php

require_once ('classes/Net.inc');
require_once ('classes/Session.inc');
require_once ("ossim_db.inc");

class Net_qualification {

    var $net_name;
    var $compromise;
    var $attack;

    function Net_qualification ($net_name, $compromise, $attack)
    {
        $this->net_name          = $net_name;
        $this->compromise      = $compromise;
        $this->attack          = $attack;
    }

    function get_net_name()           { return $this->net_name; }
    function get_compromise()       { return $this->compromise; }
    function get_attack()           { return $this->attack; }

    function get_list($conn, $where = "", $order = "") 
    {
        $query = OssimQuery("SELECT * FROM net_qualification $where $order");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF)
            {
                if (Session::netAllowed($conn, $rs->fields["net_name"]))
                {
                    $list[] = new Net_qualification (
                                        $rs->fields["net_name"],
                                        $rs->fields["compromise"],
                                        $rs->fields["attack"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_name, $compromise, $attack)
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $sql = "INSERT INTO net_qualification VALUES (?, ?, ?)"; 
        $params = array($net_name, $compromise, $attack);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $net_name, $compromise, $attack)
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $sql = "UPDATE net_qualification SET net_name = ?, compromise = ?, attack = ? WHERE net_name = ?";
        $params = array($net_name, $compromise, $attack, $net_name);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $net_name) {
        
        $sql = "DELETE FROM net_qualification WHERE net_name = ?";
        $params = array($net_name);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
