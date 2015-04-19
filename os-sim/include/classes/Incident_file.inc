<?php

class Incident_file {

    var $id;
    var $incident_id;
    var $incident_ticket;
    var $name;
    var $type;
    var $content;

    function Incident_file ($id, $incident_id, $incident_ticket, 
                            $name, $type, $content)
    {
        $this->id               = intval($id);
        $this->incident_id      = intval($incident_id);
        $this->incident_ticket  = intval($incident_ticket);
        $this->name             = $name;
        $this->type             = $type;
        $this->content          = $content;
    }

    function get_id()               { return $this->id;         }
    function get_incident_id()      { return $this->incident_id;}
    function get_incident_ticket()  { return $this->incident_ticket;}
    function get_name()             { return $this->name;       }
    function get_type()             { return $this->type;       }
    function get_content()          { return $this->content;    }


    function get_list ($conn, $args)
    {
        $query = "SELECT * FROM incident_file $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_file (
                                        $rs->fields["id"],
                                        $rs->fields["incident_id"],
                                        $rs->fields["incident_ticket"],
                                        $rs->fields["name"],
                                        $rs->fields["type"],
                                        $rs->fields["content"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert ($conn, $incident_id, $incident_ticket,
                     $name, $type, $content)
    {
        $incident_id     = intval($incident_id);
        $incident_ticket = intval($incident_ticket);

        $query = "INSERT into incident_file (
            incident_id, incident_ticket, name, type, content) VALUES
            ($incident_id, $incident_ticket, '$name', '$type', '$content');";

        if ($conn->Execute($query) === false)
        {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete ($conn, $id, $incident_id, $incident_ticket)
    {
        $id = intval($id);
        $incident_id = intval($incident_id);
        $incident_ticket = intval($incident_ticket);

        $query = "DELETE FROM incident_file 
            WHERE id = $id AND incident_id = $incident_id AND 
                incident_ticket = $incident_ticket";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>
