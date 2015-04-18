<?php

require_once ('classes/Net.inc');
require_once ('classes/Session.inc');

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
        $allowed_nets = Session::getValue();

        if (!$rs = &$conn->Execute("SELECT * FROM net_qualification $where
        $order")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {

                $net_list = Net::get_list($conn,
                    "WHERE name = '" . $rs->fields["net_name"] . "'");
                if ((false !== strpos($allowed_nets, $net_list[0]->get_ips()))
                    or (!$allowed_nets))
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

        $query = "INSERT INTO net_qualification VALUES 
            ('$net_name', $compromise, $attack);";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update($conn, $net_name, $compromise, $attack)
    {
        $compromise = intval($compromise);
        $attack = intval($attack);

        $query = "UPDATE net_qualification SET net_name = '$net_name', 
            compromise = $compromise, attack = $attack WHERE net_name =
            '$net_name';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $net_name) {
        $query = "DELETE FROM net_qualification WHERE net_name = '$net_name';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}

?>
