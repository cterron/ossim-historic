<?php

require_once ('classes/Incident.inc');
require_once ("ossim_db.inc");

class Incident_type {
    
    var $id;
    var $descr;

    function Incident_type($id, $descr)
    {
        $this->id           = $id;
        $this->descr        = $descr;
    }

    function get_id()           { return $this->id;         }
    function get_descr()        { return $this->descr;      }


    function get_list($conn, $args = "")
    {
        $list = array();
        $query = OssimQuery("SELECT * FROM incident_type $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_type (
                                        $rs->fields["id"],
                                        $rs->fields["descr"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert($conn, $id, $descr)
    {
        $sql = "INSERT into incident_type ( id, descr) VALUES   (?, ?)";
        $params = array($id, $descr);
        
        if ($conn->Execute($sql, $params) == false) {
                print 'Error inserting: ' .$conn->ErrorMsg().'<BR>';
                exit;
        }
    }
    
    function update($conn, $id, $descr)
    {
        $sql = "UPDATE incident_type SET descr= ? WHERE id = ?";
        
        $params = array($descr, $id);
        
        if ($conn->Execute($sql, $params) == false) {
                print 'Error updating: ' .$conn->ErrorMsg().'<BR>';
                exit;
        }
    }
   

    function delete($conn, $id)
    {
        $sql = "DELETE FROM incident_type WHERE id = ?";
        $params = array($id);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        $sql = "UPDATE incident SET type_id = 'Generic' WHERE type_id = ?"; 
        $params = array($id);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

    }
}

?>
