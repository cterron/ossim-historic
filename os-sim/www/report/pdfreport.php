<?php

require('classes/PDF.inc');

session_cache_limiter('private'); 

if ($_POST["submit_security"]) {

    $pdf = new PDF("OSSIM Security Report");
    $newpage = False;

    /* rows per table */
    if (!is_numeric($limit = $_POST["limit"]))
        $limit = 10;

    if ($_POST["attacked"] == "on") {
        $pdf->AttackedHosts($limit);
        $newpage = True;
    }
    if ($_POST["attacker"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AttackerHosts($limit);
        $newpage = True;
    }
    if ($_POST["ports"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Ports($limit);
        $newpage = True;
    }
    if ($_POST["alertsbyhost"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->Alerts($limit);
        $newpage = True;
    }
    if ($_POST["alertsbyrisk"] == "on") {
        if ($newpage) $pdf->AddPage();
        $pdf->AlertsByRisk($limit);
    }
    $pdf->Output();

} elseif ($_POST["submit_metrics"]) {

    $pdf = new PDF("OSSIM Metrics Report");
    $newpage = False;

    if ($_POST["time_day"] == "on") {
        $pdf->Metrics("day", "compromise", "global");
        $pdf->Metrics("day", "compromise", "net");
        $pdf->Metrics("day", "compromise", "host");
        $pdf->AddPage();
        $pdf->Metrics("day", "attack", "global");
        $pdf->Metrics("day", "attack", "net");
        $pdf->Metrics("day", "attack", "host");
        $newpage = True;
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
        $newpage = True;
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
        $newpage = True;
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
    
    $show_alarms = $show_metrics = False;
    if ($_POST["alarms"] == "on")
        $show_alarms = True;
    if ($_POST["metrics"] == "on")
        $show_metrics = True;

    $pdf = new PDF("OSSIM Incident Report", "P", "mm", "A4");
    $ids = $pdf->IncidentSummary($show_metrics, $show_alarms);
    foreach ($ids as $incident_id) {
        $pdf->AddPage();
        $pdf->Incident($incident_id);
    }
    $pdf->Output();
}

?>
