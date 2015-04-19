<?php

class Incident_ticket {

    var $id;
    var $incident_id;
    var $date;
    var $status;
    var $priority;
    var $user;
    var $description;
    var $action;
    var $in_charge;
    var $transferred;
    var $copy;

    function Incident_ticket ($id, $incident_id, $date, $status, $priority,
                              $user, $description, $action, 
                              $in_charge, $transferred, $copy)
    {
        $this->id           = intval($id);
        $this->incident_id  = intval($incident_id);
        $this->date         = $date;
        $this->status       = htmlentities($status);
        $this->priority     = intval($priority);
        $this->user         = htmlentities($user);
        $this->description  = htmlentities($description);
        $this->action       = htmlentities($action);
        $this->in_charge    = htmlentities($in_charge);
        $this->transferred  = htmlentities($transferred);
        $this->copy         = $copy;
    }

    function get_id()           { return $this->id;         }
    function get_incident_id()  { return $this->incident_id;}
    function get_status()       { return $this->status;      }
    function get_priority()     { return $this->priority;   }
    function get_user()         { return $this->user;       }
    function get_description()  { return $this->description;}
    function get_action()       { return $this->action;     }
    function get_in_charge()    { return $this->in_charge;  }
    function get_transferred()  { return $this->transferred;}
    function get_copy()         { return $this->copy;       }

    function get_attachment($conn)
    {
        require_once ('classes/Incident_file.inc');

        $attachment = Incident_file::get_list ($conn, 
            " WHERE incident_id = " . $this->incident_id . 
            " AND incident_ticket = " . $this->id);

        if ($attachment)
            return $attachment[0];
        else
            return False;
    }

    function get_date()
    {
        return (substr($this->date, 0, 4) . '-' .
                substr($this->date, 4, 2) . '-' .
                substr($this->date, 6, 2) . ' ' .
                substr($this->date, 8, 2) . ':' .
                substr($this->date, 10, 2) . ':' .
                substr($this->date, 12, 2));
    }

    function get_list ($conn, $args)
    {
        $query = "SELECT * FROM incident_ticket $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident_ticket (
                                        $rs->fields["id"],
                                        $rs->fields["incident_id"],
                                        $rs->fields["date"],
                                        $rs->fields["status"],
                                        $rs->fields["priority"],
                                        $rs->fields["users"],
                                        $rs->fields["description"],
                                        $rs->fields["action"],
                                        $rs->fields["in_charge"],
                                        $rs->fields["transferred"],
                                        $rs->fields["copy"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert ($conn, $incident_id, $status, $priority, 
                     $user, $description, $action, 
                     $in_charge, $transferred, $copy, $attachment)
    {
    
        $incident_id    = intval($incident_id);
        $priority       = intval($priority);
        $description    = ereg_replace("<[^>]*>", "", $description);
        $status         = ereg_replace("<[^>]*>", "", $status);
        $user           = ereg_replace("<[^>]*>", "", $user);
        $action         = ereg_replace("<[^>]*>", "", $action);
        $in_charge      = ereg_replace("<[^>]*>", "", $in_charge);
        $transferred    = ereg_replace("<[^>]*>", "", $transferred);

        $query = "INSERT into incident_ticket (
            incident_id, status, priority, users, description, action, 
            in_charge, transferred, copy) VALUES
            ($incident_id, '$status', '$priority', 
             '$user', '$description', '$action',
             '$in_charge', '$transferred', '$copy');";
        $query2 = "UPDATE incident SET priority = $priority 
            WHERE id = $incident_id";
        if (($conn->Execute($query) === false) or 
            ($conn->Execute($query2) === false))
        {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        if ($attachment["content"])
        {
            require_once ('classes/Incident_file.inc');
            
            /* get last inserted id */
            $query = "SELECT LAST_INSERT_ID() as id FROM incident_ticket";
            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            }
            $last_ticket_id =  $rs->fields["id"];

            Incident_file::insert ($conn,
                                   $incident_id,
                                   $last_ticket_id,
                                   $attachment["name"],
                                   $attachment["type"],
                                   $attachment["content"]);
        }
    }
    
    function update_priority ($conn, $id, $incident_id, $priority)
    {
        /* update priority */
        $id = intval($id);
        $incident_id = intval($incident_id);
        $priority = intval($priority);

        $query = "UPDATE incident_ticket 
            SET priority = $priority, date = date WHERE id = $id";
        if ($conn->Execute($query) === false) 
        {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* get last id */
        $query = "SELECT id, incident_id FROM incident_ticket 
            WHERE incident_id = $incident_id ORDER BY id DESC LIMIT 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        $max_id = $rs->fields["id"];
        $incident_id = $rs->fields["incident_id"];

        /* if it's the last id, update global priority */
        if ($id == $max_id) 
        {
            $query = "UPDATE incident 
                SET priority = $priority WHERE id = $incident_id";
            if ($conn->Execute($query) === false) 
            {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }


    function delete ($conn, $id, $incident_id)
    {
        $id = intval($id);
        $incident_id = intval($incident_id);

        $query = "DELETE FROM incident_ticket 
            WHERE id = $id AND incident_id = $incident_id";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        $query = "DELETE FROM incident_file 
            WHERE incident_ticket = $id AND incident_id = $incident_id";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function change_status ($conn, $id)
    {
        $id = intval($id);

        $query = "SELECT status FROM incident_ticket WHERE id = $id;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        $status = $rs->fields["status"];

        if ($status == 'Open') {
            $query2 = "UPDATE incident_ticket SET status = 'Closed'
                WHERE id = $id";
        } elseif ($status == 'Closed') {
            $query2 = "UPDATE incident_ticket SET status = 'Open'
                WHERE id = $id";
        }

        if (isset($query2))
        {
            if ($conn->Execute($query2) === false) {
                print 'error inserting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }
}

?>
