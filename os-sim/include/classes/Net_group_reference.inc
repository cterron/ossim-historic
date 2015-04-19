<?php

class Net_group_reference {

    var $net_group_name;
    var $net_name;
    
    function Net_group_reference ($net_group_name, $net_name)
    {
        $this->net_group_name      = $net_group_name;    
        $this->net_name  = $net_name;
    }

    function get_net_group_name()      { return $this->net_group_name; }
    function get_net_name()  { return $this->net_name; }
    
    function get_list($conn, $net) 
    {
        $net = stripslashes($net);
        $net = mysql_escape_string($net);
        $query = "SELECT * FROM net_group_reference WHERE net_group_name = '$net'";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Net_group_reference 
                                                (stripslashes($rs->fields["net_group_name"]),
                                                 stripslashes($rs->fields["net_name"]));
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $net_group_name, $nets)
    {
        $net_group_name = stripslashes($net_group_name);
        $net_group_name = mysql_real_escape_string($net_group_name);
        foreach ($nets as $net_name) {
            $net_name = stripslashes($net_name);
            $net_name = mysql_real_escape_string($net_name);
            $query = "INSERT INTO net_group_reference VALUES 
                    ('$net_group_name', '$net_name');";
            if ($conn->Execute($query) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

    function delete($conn, $net_group_name, $net_name = "%") 
    {
        $net_group_name = stripslashes($net_group_name);
        $net_group_name = mysql_real_escape_string($net_group_name);
        $net_name = stripslashes($net_name);
        $net_name = mysql_real_escape_string($net_name);
        $query = "DELETE FROM net_group_reference 
                    WHERE net_group_name = '$net_group_name' AND net_name LIKE '$net_name'";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function in_net_group_reference($conn, $net, $net_name)
    {
        $net = mysql_real_escape_string($net);
        $net_name = mysql_real_escape_string($net_name);
                                                                                
        $query = "SELECT * FROM net_group_reference WHERE net_group_name = '$net' AND net_name='$net_name';";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        }
        return $rs->fields[0];
    }
    function check_orphan($conn)
    {
        $query =  "SELECT * FROM net_group_reference LEFT JOIN net ON net.name = net_group_reference.net_name WHERE descr IS NULL";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Net_group_reference::delete($conn, $rs->fields["net_group_name"], $rs->fields["net_name"]);
                $rs->MoveNext();
            }
        }
    }
}

?>
