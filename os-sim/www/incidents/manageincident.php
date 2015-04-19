<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
Session::logcheck("MenuIncidents", "IncidentsIncidents");
require_once 'ossim_db.inc';
require_once 'classes/Incident.inc';
require_once 'classes/Incident_ticket.inc';
require_once 'classes/Incident_tag.inc';


function die_error($msg = null, $append = null)
{
    if ($msg) ossim_set_error($msg);
    echo ossim_error();
    echo '<br/><a href="javascript: history.go(-1)">'._("Back").'</a>';
    echo $append;
    exit;
}

$db = new ossim_db();
$conn = $db->connect();

$id = GET('incident_id');
$action = GET('action'); 

//
// Subscriptions management
//
if ($action == 'subscrip') {
    if (POST('login')) {
        if (!ossim_valid($id, OSS_DIGIT)) {
            die_error("Wrong ID");
        }
        if (ossim_valid(POST('login'), OSS_LETTER)) {
            if (POST('subscribe')) {
                Incident::insert_subscription($conn, $id, $_POST['login']);
            } elseif (POST('unsubscribe')) {
                Incident::delete_subscriptions($conn, $id, $_POST['login']);
            }
        }
    }
    header("Location: incident.php?id=$id"); exit;
}

//
// Ticket new
//
if ($action == 'newticket') {
    if (!ossim_valid($id, OSS_DIGIT)) die_error("Wrong ID");
    $vals = array(
        'status',
        'priority',
        'transferred',
        'tag',
        'description',
        'action',
        'transferred',
        );
    foreach ($vals as $var) {
        $$var = POST("$var");
    }
    if (isset($_FILES['attachment']) && $_FILES['attachment']['tmp_name']) {
        $attachment = $_FILES['attachment'];
        $attachment['content'] = file_get_contents($attachment['tmp_name']);
    } else {
        $attachment = null;
    }
    
    $user = Session::get_me($conn);
    if (!$user->get_email()) {
        die_error(_("Users with no valid email are not allowed to insert tickets"));
    }
    $login = $user->get_login();
    
    $tags = POST('tags') ? POST('tags') : array();
    
    Incident_ticket::insert(
                     $conn, $id, $status, $priority, 
                     $login, $description, $action, 
                     $transferred, $tags, $attachment);
    // Error should be only at the mail() function in Incident_ticket::mail_susbcription()
    if (ossim_error()) {
        die_error(null, "<br/><a href=\"incident.php?id=$id\">".
                        _("Continue").'</a>');
    }
    header("Location: incident.php?id=$id"); exit;
}
//
// Ticket deletetion
//
if ($action == 'delticket') {
    if (!GET('ticket_id')) {
        die("Invalid Ticket ID");
    }
    Incident_ticket::delete($conn, GET('ticket_id'));
    header("Location: incident.php?id=$id"); exit;
}

//
// Incident deletetion
//

if ($action == 'delincident') {
    Incident::delete($conn, $id);
    header("Location: ./"); exit;
}
//
// Incident edit
//
if ($action == 'editincident') {
    /* update alarm|event incident */
    if (GET('ref') == 'Alarm' or GET('ref') == 'Event')
    {
        $method = GET('ref') == 'Alarm' ? 'update_alarm' : 'update_event';
        $vars = array(
            'incident_id', 'title', 'type', 'submitter', 'priority', 'src_ips', 'dst_ips', 'src_ports', 'dst_ports', 'event_start', 'event_end'
        );
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        
        Incident::$method($conn, $incident_id, $title, $type, $submitter, $priority,
                           $src_ips, $dst_ips, $src_ports, $dst_ports, $event_start, $event_end);
    }
    /* update metric incident */
    elseif (GET('ref') == 'Metric')
    {
        $vars = array(
            'incident_id', 'title', 'type', 'submitter', 'priority', 'target', 'metric_type', 'metric_value', 'event_start', 'event_end'
        );
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        Incident::update_metric($conn, $incident_id, $title, $type, $submitter, $priority,
                            $target, $metric_type, $metric_value, $event_start, $event_end);
    }
    elseif (GET('ref') == 'Anomaly')
    {
        if (GET('anom_type') == 'mac')
        {
            $vars = array('incident_id','title', 'type', 'submitter', 'priority', 'a_sen', 'a_date', 'a_mac', 'a_mac_o', 'anom_ip', 'a_vend', 'a_vend_o');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_date, $a_mac_o, $a_vend_o);
            $anom_data_new  = array($a_sen, $a_date, $a_mac, $a_vend);
            Incident::update_anomaly($conn, $incident_id, $title, $type, $submitter, $priority, 'mac', $anom_ip, $anom_data_orig, $anom_data_new); 
        } elseif (GET('anom_type') == 'service') {
            $vars = array('incident_id','title', 'type', 'submitter', 'priority', 'a_sen', 'a_date', 'a_port', 'a_prot_o', 'a_prot', 'anom_ip', 'a_ver', 'a_ver_o');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_port, $a_date, $a_prot_o, $a_ver_o);
            $anom_data_new  = array($a_sen, $a_port, $a_date, $a_prot, $a_ver);
            Incident::update_anomaly($conn, $incident_id, $title, $type, $submitter, $priority, 'service', $anom_ip, $anom_data_orig, $anom_data_new); 
        } elseif (GET('anom_type') == 'os') {
            $vars = array('incident_id','title', 'type', 'submitter', 'priority', 'a_sen', 'a_date', 'a_os', 'a_os_o', 'anom_ip');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_date, $a_os_o);
            $anom_data_new  = array($a_sen, $a_date, $a_os);
            Incident::update_anomaly($conn, $incident_id, $title, $type, $submitter, $priority, 'os', $anom_ip, $anom_data_orig, $anom_data_new); 
        } /*elseif os*/
    } /*elseif anomaly*/
  elseif (GET('ref') == 'Vulnerability')
    {
        $vars = array('incident_id', 'title', 'type', 'submitter', 'priority', 'ip', 'port', 'nessus_id', 'risk', 'description');
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        Incident::update_vulnerability($conn, $incident_id, $title, $type, $submitter, $priority, $ip, $port, $nessus_id, $risk, $description);
    } /*elseif vulnerability*/
    if (ossim_error()) die_error();
    header("Location: incident.php?id=$incident_id"); exit;
}

//
// Incident new
//
if ($action == 'newincident') {
    /* insert new alarm|event incident */
    if (GET('ref') == 'Alarm' or GET('ref') == 'Event')
    {
        $method = GET('ref') == 'Alarm' ? 'insert_alarm' : 'insert_event';
        $vars = array(
            'title', 'type', 'submitter', 'priority', 'src_ips', 'dst_ips', 'src_ports', 'dst_ports', 'event_start', 'event_end'
        );
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        
        $incident_id = Incident::$method($conn, $title, $type, $submitter, $priority, 
                          $src_ips, $dst_ips, $src_ports, $dst_ports, $event_start, $event_end);
    }
    /* insert new metric incident */
    elseif (GET('ref') == 'Metric')
    {
        $vars = array(
            'title', 'type', 'submitter', 'priority', 'target', 'metric_type', 'metric_value', 'event_start', 'event_end'
        );
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        $incident_id = Incident::insert_metric($conn, $title, $type, $submitter, $priority, 
                       $target, $metric_type, $metric_value, $event_start, $event_end);
    }
    elseif (GET('ref') == 'Anomaly')
    {
        if (GET('anom_type') == 'mac')
        {
            $vars = array('title', 'type', 'submitter', 'priority', 'a_sen', 'a_date_o',
            'a_date', 'a_mac', 'a_mac_o', 'anom_ip', 'a_vend', 'a_vend_o');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_date, $a_mac_o, $a_vend_o);
            $anom_data_new  = array($a_sen, $a_date, $a_mac, $a_vend);
            $incident_id = Incident::insert_anomaly($conn, $title, $type, $submitter, $priority, 'mac', $anom_ip, $anom_data_orig, $anom_data_new); 
        }  elseif (GET('anom_type') == 'service') {
            $vars = array('title', 'type', 'submitter', 'priority', 'a_sen', 'a_date', 'a_port', 'a_prot_o', 'a_prot', 'anom_ip', 'a_ver', 'a_ver_o');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_date, $a_port, $a_prot_o, $a_ver_o);
            $anom_data_new  = array($a_sen, $a_date, $a_port, $a_prot, $a_ver);
            $incident_id = Incident::insert_anomaly($conn, $title, $type, $submitter, $priority, 'service', $anom_ip, $anom_data_orig, $anom_data_new); 
        } elseif (GET('anom_type') == 'os') {
            $vars = array('title', 'type', 'submitter', 'priority', 'a_sen', 'a_date', 'a_os', 'a_os_o', 'anom_ip');
            foreach ($vars as $v) {
                $$v = GET("$v");
            }
            $anom_data_orig = array($a_sen, $a_date, $a_os_o);
            $anom_data_new  = array($a_sen, $a_date, $a_os);
            $incident_id = Incident::insert_anomaly($conn, $title, $type, $submitter, $priority, 'os', $anom_ip, $anom_data_orig, $anom_data_new); 
        } /*elseif os*/

    } /*elseif anomaly*/
   /* insert new vulnerability incident */
    elseif (GET('ref') == 'Vulnerability')
    {
        $vars = array('title', 'type', 'submitter', 'priority', 'ip', 'port', 'nessus_id', 'risk', 'description');
        foreach ($vars as $v) {
            $$v = GET("$v");
        }
        $incident_id = Incident::insert_vulnerability($conn, $title, $type, $submitter, $priority, $ip, $port, $nessus_id, $risk, $description);
    }

    
    if (ossim_error()) {
        die_error();
    }
    header("Location: incident.php?id=$incident_id"); exit;
}
