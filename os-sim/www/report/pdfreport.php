<?php
require_once 'classes/Security.inc';
require_once 'classes/PDF.inc';

session_cache_limiter('private'); 

$pathtographs = dirname($_SERVER['REQUEST_URI']);

$proto = "http";
if (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == "on") $proto = "https";

$datapath = "$proto://".$_SERVER['SERVER_ADDR'].":".$_SERVER['SERVER_PORT']."$pathtographs/graphs";


function clean_tmp_files()
{
    foreach($GLOBALS['tmp_files'] as $file) {
        unlink($file);
    }
}

register_shutdown_function('clean_tmp_files');

function create_image($url, $args = array())
{
    foreach ($args as $k => $v) {
        $_GET[$k] = $v;
    }
    ob_start();
    include $url;
    $cont = ob_get_clean();
    $tmp_name = tempnam('/tmp', 'ossim_');
    $GLOBALS['tmp_files'][] = $tmp_name;
    $fd = fopen($tmp_name, 'w');
    fputs($fd, $cont);
    fclose($fd);
    return $tmp_name;
}

if (POST('submit_security')) {

    $pdf = new PDF("OSSIM Security Report");
    $newpage = false;

    /* rows per table */
    if (!is_numeric($limit = POST('limit')))
        $limit = 10;

    if (POST('attacked') == "on") {
        $pdf->AttackedHosts($limit);
        $args = array('limit' => $limit, 'target' => 'ip_dst');
        $image = create_image('./graphs/attack_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('attacker') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AttackerHosts($limit);
        $args = array('limit' => $limit, 'target' => 'ip_src');
        $image = create_image('./graphs/attack_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('eventsbyhost') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Events($limit);
        $args = array('hosts' => $limit);
        $image = create_image('./graphs/events_received_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "120", "60", "PNG");
        $newpage = true;
    }
    if (POST('ports') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Ports($limit);
        $args = array('ports' => $limit);
        $image = create_image('./graphs/ports_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('eventsbyrisk') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->EventsByRisk($limit);
    }
    $pdf->Output();

} elseif (POST('submit_metrics')) {

    $pdf = new PDF("OSSIM Metrics Report");
    $newpage = false;

    if (POST('time_day') == "on") {
        $pdf->Metrics("day", "compromise", "global");
        $pdf->Metrics("day", "compromise", "net");
        $pdf->Metrics("day", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("day", "attack", "global");
        $pdf->Metrics("day", "attack", "net");
        $pdf->Metrics("day", "attack", "host");
        $newpage = true;
    }
    if (POST('time_week') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Metrics("week", "compromise", "global");
        $pdf->Metrics("week", "compromise", "net");
        $pdf->Metrics("week", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("week", "attack", "global");
        $pdf->Metrics("week", "attack", "net");
        $pdf->Metrics("week", "attack", "host");
        $newpage = true;
    }
    if (POST('time_month') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Metrics("month", "compromise", "global");
        $pdf->Metrics("month", "compromise", "net");
        $pdf->Metrics("month", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("month", "attack", "global");
        $pdf->Metrics("month", "attack", "net");
        $pdf->Metrics("month", "attack", "host");
        $newpage = true;
    }
    if (POST('time_year') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Metrics("year", "compromise", "global");
        $pdf->Metrics("year", "compromise", "net");
        $pdf->Metrics("year", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("year", "attack", "global");
        $pdf->Metrics("year", "attack", "net");
        $pdf->Metrics("year", "attack", "host");
    }
    
    $pdf->Output();

} elseif (POST('submit_incident')) {
    
    $reason = $date = $location = $in_charge = "";
    $reason = POST('reason');
    $date   = POST('date');
    $location  = POST('location');
    $in_charge = POST('in_charge');

    $summary       = POST('summary');
    $metrics_notes = POST('metrics_notes');
    $alarms_notes  = POST('alarms_notes');
    $events_notes  = POST('events_notes');
    
    $pdf = new PDF("OSSIM Incident Report", "P", "mm", "A4");

    $pdf->IncidentGeneralData($reason, $date, $location, $in_charge, $summary);

    /* metrics */
    $pdf->IncidentSummary(gettext("1. METRICS"), "Metric", $metrics_notes);
    $ids = $pdf->get_metric_ids();
    foreach ($ids as $incident_id) {
        $pdf->Incident($incident_id);
    }

    /* alarms */
    $pdf->IncidentSummary(gettext("2. ALARMS"), "Alarm", $alarms_notes);
    $ids = $pdf->get_alarm_ids();
    foreach ($ids as $incident_id) {
        $pdf->Incident($incident_id);
    }

    /* events */
    $pdf->IncidentSummary(gettext("3. ALERTS"), "Event", $events_notes);
    $ids = $pdf->get_event_ids();
    foreach ($ids as $incident_id) {
        $pdf->Incident($incident_id);
    }

    $pdf->Output();

}  elseif (POST('submit_alarms')) {

    $report_type = "alarm";
    $pdf = new PDF("OSSIM Alarms Report");
    $newpage = false;

    /* rows per table */
    if (!is_numeric($limit = POST('limit')))
        $limit = 10;
    
    if (POST('attacked') == "on") {
        $pdf->AttackedHosts($limit, "alarm");
        $args = array('hosts' => $limit, 'target' => 'dst_ip', 'type' => 'alarm');
        $image = create_image('./graphs/attack_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('attacker') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AttackerHosts($limit, "alarm");
        $args = array('hosts' => $limit, 'target' => 'src_ip', 'type' => 'alarm');
        $image = create_image('./graphs/attack_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('ports') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Ports($limit, "alarm");
        $args = array('ports' => $limit, 'type' => 'alarm');
        $image = create_image('./graphs/ports_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if (POST('alarmsbyhost') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Events($limit, "alarm");
        $args = array('hosts' => $limit, 'type' => 'alarm');
        $image = create_image('./graphs/events_received_graph.php', $args);
        $pdf->Image($image, $pdf->GetX(), $pdf->GetY(), "120", "60", "PNG");
        $newpage = true;
    }
    if (POST('alarmsbyrisk') == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->EventsByRisk($limit, "alarm");
    }
    $pdf->Output();
}
?>
