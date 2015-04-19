<?php

require('classes/PDF.inc');

session_cache_limiter('private'); 

$pathtographs = dirname($_SERVER[REQUEST_URI]);
$proto = "http";
if ($_SERVER[HTTPS] == "on") $proto = "https";
$datapath = "$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]$pathtographs/graphs";

if ($_POST["submit_security"]) {

    $pdf = new PDF("OSSIM Security Report");
    $newpage = false;

    /* rows per table */
    if (!is_numeric($limit = $_POST["limit"]))
        $limit = 10;

    if ($_POST["attacked"] == "on") {
        $pdf->AttackedHosts($limit);
        $pdf->Image( "$datapath/attack_graph.php?hosts=$limit&target=ip_dst", 
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["attacker"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AttackerHosts($limit);
        $pdf->Image( "$datapath/attack_graph.php?hosts=$limit&target=ip_src", 
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["ports"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Ports($limit);
        $pdf->Image( "$datapath/ports_graph.php?hosts=$limit", 
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["eventsbyhost"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Events($limit);
        $pdf->Image( "$datapath/events_received_graph.php?hosts=$limit", 
                     $pdf->GetX(), $pdf->GetY(), "120", "60", "PNG");
        $newpage = true;
    }
    if ($_POST["eventsbyrisk"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->EventsByRisk($limit);
    }
    $pdf->Output();

} elseif ($_POST["submit_metrics"]) {

    $pdf = new PDF("OSSIM Metrics Report");
    $newpage = false;

    if ($_POST["time_day"] == "on") {
        $pdf->Metrics("day", "compromise", "global");
        $pdf->Metrics("day", "compromise", "net");
        $pdf->Metrics("day", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("day", "attack", "global");
        $pdf->Metrics("day", "attack", "net");
        $pdf->Metrics("day", "attack", "host");
        $newpage = true;
    }
    if ($_POST["time_week"] == "on") {
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
    if ($_POST["time_month"] == "on") {
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
    if ($_POST["time_year"] == "on") {
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

} elseif ($_POST["submit_incident"]) {
    
    $reason = $date = $location = $in_charge = "";
    if (isset($_POST["reason"]))        $reason = $_POST["reason"];
    if (isset($_POST["date"]))          $date = $_POST["date"];
    if (isset($_POST["location"]))         $location = $_POST["location"];
    if (isset($_POST["in_charge"]))   $in_charge = $_POST["in_charge"];

    $summary = $metrics_notes = $alarms_notes = $events_notes = "";
    if (isset($_POST["summary"]))
        $summary = $_POST["summary"];
    if (isset($_POST["metrics_notes"]))
        $metrics_notes = $_POST["metrics_notes"];
    if (isset($_POST["alarms_notes"]))
        $alarms_notes = $_POST["alarms_notes"];
    if (isset($_POST["events_notes"]))
        $events_notes = $_POST["events_notes"];
    
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

}  elseif ($_POST["submit_alarms"]) {

    $report_type = "alarm";
    $pdf = new PDF("OSSIM Alarms Report");
    $newpage = false;

    /* rows per table */
    if (!is_numeric($limit = $_POST["limit"]))
        $limit = 10;
    
    if ($_POST["attacked"] == "on") {
        $pdf->AttackedHosts($limit, "alarm");
        $pdf->Image(
        "$datapath/attack_graph.php?hosts=$limit&target=dst_ip&type=alarm",
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["attacker"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AttackerHosts($limit, "alarm");
        $pdf->Image(
        "$datapath/attack_graph.php?hosts=$limit&target=src_ip&type=alarm",
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["ports"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Ports($limit, "alarm");
        $pdf->Image( "$datapath/ports_graph.php?hosts=$limit&type=alarm",
                     $pdf->GetX(), $pdf->GetY(), "110", "70", "PNG");
        $newpage = true;
    }
    if ($_POST["alarmsbyhost"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Events($limit, "alarm");
        $pdf->Image(
        "$datapath/events_received_graph.php?hosts=$limit&type=alarm",
                     $pdf->GetX(), $pdf->GetY(), "120", "60", "PNG");
        $newpage = true;
    }
    if ($_POST["alarmsbyrisk"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->EventsByRisk($limit, "alarm");
    }
    $pdf->Output();
}
?>
