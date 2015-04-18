<?php

class Incident {

    var $id;
    var $title;
    var $date;
    var $ref;
    var $priority;

    function Incident ($id, $title, $date, $ref, $priority)
    {
        $this->id       = intval($id);
        $this->title    = htmlentities($title);
        $this->date     = htmlentities($date);
        $this->ref      = htmlentities($ref);
        $this->priority = intval($priority);
    }

    function get_id()       { return intval($this->id);         }
    function get_title()    { return $this->title;      }
    function get_ref()      { return $this->ref;        }

    function get_priority($conn)
    {
        $query = "SELECT priority FROM incident_ticket
            WHERE incident_id = " . intval($this->id) . " ORDER BY id DESC LIMIT 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }

        if ($rs->EOF)
            return intval($this->priority);
        else
            return intval($rs->fields["priority"]);
    }

    
    function get_status($conn)
    {
        $query = "SELECT status FROM incident_ticket
            WHERE incident_id = " . intval($this->id) . " ORDER BY id DESC LIMIT 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        if ($rs->EOF)
            return "Unknown";
        else
            return $rs->fields["status"];
    }

    function get_in_charge($conn)
    {
        $query = "SELECT in_charge, transferred FROM incident_ticket
            WHERE incident_id = " . $this->id . " ORDER BY id DESC LIMIT 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }

        if ($rs->EOF) {
            return "Unknown";
        } else {
            $in_charge   = $rs->fields["in_charge"];
            $transferred = $rs->fields["transferred"];
            if ($transferred)
                return $transferred;
            else
                return $in_charge;
        }
    }

    function get_last_modification($conn)
    {
        $query = "SELECT date FROM incident_ticket
            WHERE incident_id = " . $this->id . " ORDER BY id DESC LIMIT 1";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        
        if ($rs->EOF) {
            return "Unknown";
        } else {
            return (substr($rs->fields["date"], 0, 4) . '-' .
                substr($rs->fields["date"], 4, 2) . '-' .
                substr($rs->fields["date"], 6, 2) . ' ' .
                substr($rs->fields["date"], 8, 2) . ':' .
                substr($rs->fields["date"], 10, 2) . ':' .
                substr($rs->fields["date"], 12, 2));
        }
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

    function get_ticket()
    {
        $id = ($this->id > 10)? $this->id : "0" . $this->id;
        return strtoupper(substr($this->ref, 0, 3)) . $id;
    }

    function get_priority_bgcolor($priority)
    {
        if ($priority > 7)
            return "red";
        elseif ($priority > 4)
            return "orange";
        elseif ($priority > 1)
            return "green";
    }

    function get_priority_fgcolor($priority)
    {
        if ($priority > 7)
            return "white";
        elseif ($priority > 4)
            return "black";
        elseif ($priority > 1)
            return "white";
    }

    function colorize_status($status)
    {
        $status = htmlentities($status);
        if ($status == 'Open')
            echo "<font color=\"red\"><b>Open</b></font>";
        elseif ($status == 'Closed')
            echo "<b>Closed</b>";
        else
            echo "<b>$status</b>";
    }

    function get_list ($conn, $args = "")
    {
        $args = mysql_escape_string($args);
        $query = "SELECT * FROM incident $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Incident ($rs->fields["id"],
                                        $rs->fields["title"],
                                        $rs->fields["date"],
                                        $rs->fields["ref"],
                                        $rs->fields["priority"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_tickets ($conn)
    {
        require_once ('classes/Incident_ticket.inc');
        return Incident_ticket::get_list($conn, 
            "WHERE incident_id = " . $this->id . " ORDER BY id");
    }

    function get_alarms ($conn)
    {
        require_once ('classes/Incident_alarm.inc');
        return Incident_alarm::get_list($conn, 
            "WHERE incident_id = " . $this->id . " ORDER BY id");
    }

    function get_metrics ($conn)
    {
        require_once ('classes/Incident_metric.inc');
        return Incident_metric::get_list($conn, 
            "WHERE incident_id = " . $this->id . " ORDER BY id");
    }

    function print_td_priority($priority, $bgcolor, $fgcolor)
    {
    $priority = intval($priority);
    $bgcolor = htmlentities($bgcolor);
    $fgcolor = htmlentities($fgcolor);
        $priority_td = "
        <td>
          <table align=\"center\" bgcolor=\"$bgcolor\" fgcolor=\"$fgcolor\">
            <tr>
              <td";
        if ($bgcolor)
            $priority_td .= " bgcolor=\"$bgcolor\" fgcolor=\"$fgcolor\"";
        $priority_td .= "><b>&nbsp;";
        if ($fgcolor)
            $priority_td .= "<font color=\"$fgcolor\">$priority</font>";
        else
            $priority_td .= $priority;
        $priority_td .= "&nbsp;</b>
              </td>
            </tr>
          </table>
        </td>";
        echo $priority_td;
    }

    function insert_alarm($conn, $title, $priority, 
                          $src_ips, $dst_ips, $src_ports, $dst_ports)
    {
        $title = htmlentities($title);
        $priority = intval($priority);
        require_once ('classes/Session.inc');
        $user = Session::get_session_user();

        /* insert incident */
        $query = "INSERT INTO incident (title, ref, priority)
            VALUES ('$title', 'Alarm', $priority)";
        if ($conn->Execute($query) === false)
        {
            print 'error inserting incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* get last inserted id */
        $query = "SELECT LAST_INSERT_ID() as id FROM incident";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        $last_id =  $rs->fields["id"];

        $query2 = "INSERT INTO incident_ticket 
            (incident_id, priority, users, description, action, in_charge)
            VALUES ($last_id, $priority, '$user', '$title', '', '$user')";
        $query3 = "INSERT INTO incident_alarm
            (incident_id, src_ips, dst_ips, src_ports, dst_ports)
            VALUES ($last_id, '$src_ips', '$dst_ips', '$src_ports', '$dst_ports');";

        if ( ($conn->Execute($query2) === false) or
             ($conn->Execute($query3) === false) )
        {
            print 'error inserting incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        return $last_id;
    }

    function insert_metric($conn, $title, $priority, 
                           $target, $metric_type, $metric_value)
    {
        $title = htmlentities($title);
        $target = htmlentities($target);
        $priority = intval($priority);
        $metric_type = htmlentities($metric_type);
        $metric_value = intval($metric_value);
        require_once ('classes/Session.inc');
        $user = Session::get_session_user();

        /* insert incident */
        $query = "INSERT INTO incident (title, ref, priority)
            VALUES ('$title', 'Metric', $priority)";
        if ($conn->Execute($query) === false)
        {
            print 'error inserting incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        
        /* get last inserted id */
        $query = "SELECT LAST_INSERT_ID() as id FROM incident";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit;
        }
        $last_id =  $rs->fields["id"];

        /* insert references with last id*/
        $query2 = "INSERT INTO incident_ticket 
            (incident_id, priority, users, description, action, in_charge)
            VALUES ($last_id, $priority, '$user', '$title', '', '$user')";
        $query3 = "INSERT INTO incident_metric
            (incident_id, target, metric_type, metric_value)
            VALUES ($last_id, '$target', '$metric_type', '$metric_value');";

        if ( ($conn->Execute($query2) === false) or
             ($conn->Execute($query3) === false) )
        {
            print 'error inserting incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        return $last_id;
    }

    function update_alarm ($conn, $incident_id, $title,
                           $src_ips, $dst_ips, $src_ports, $dst_ports)
    {
        $incident_id = intval($incident_id);
        $title = htmlentities($title);

        $query1 = "UPDATE incident SET title = '$title' 
            WHERE id = $incident_id";
        $query2 = "UPDATE incident_alarm 
            SET src_ips = '$src_ips', dst_ips = '$dst_ips',
                src_ports = '$src_ports', dst_ports = '$dst_ports'
            WHERE incident_id = $incident_id";

        if ( ($conn->Execute($query1) === false) or
             ($conn->Execute($query2) === false) )
        {
            print 'error updating incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update_metric ($conn, $incident_id, $title, $target)
    {
        $incident_id = intval($incident_id);
        $title = htmlentities($title);
        $target = htmlentities($target);

        $query1 = "UPDATE incident SET title = '$title' 
            WHERE id = $incident_id";
        $query2 = "UPDATE incident_metric 
            SET target = '$target'
            WHERE incident_id = $incident_id";

        if ( ($conn->Execute($query1) === false) or
             ($conn->Execute($query2) === false) )
        {
            print 'error updating incident: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete ($conn, $id)
    {
        $id = intval($id);
        $query1 = "DELETE FROM incident WHERE id = $id";
        $query2 = "DELETE FROM incident_ticket WHERE incident_id = $id";
        $query3 = "DELETE FROM incident_alarm  WHERE incident_id = $id";
        $query4 = "DELETE FROM incident_metric WHERE incident_id = $id";
        if ( ($conn->Execute($query1) === false) or
             ($conn->Execute($query2) === false) or 
             ($conn->Execute($query3) === false) or 
             ($conn->Execute($query4) === false)) 
        {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }
}


?>
