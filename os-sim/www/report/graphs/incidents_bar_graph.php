<?php
require_once 'classes/Session.inc';
require_once 'classes/Incident.inc';
require_once 'ossim_db.inc';

Session::logcheck("MenuIncidents", "IncidentsReport");

require_once 'classes/Security.inc';

$by = GET('by');

ossim_valid($by, OSS_ALPHA, OSS_SPACE, OSS_SCORE, 'illegal:'._("Target"));

if (ossim_error()) {
    die(ossim_error());
}

$conf = $GLOBALS["CONF"];
$jpgraph = $conf->get_conf("jpgraph_path");

include "$jpgraph/jpgraph.php";
include "$jpgraph/jpgraph_bar.php";

$db = new ossim_db();
$conn = $db->connect();

if ($by == "monthly_by_status") {
    
    $year_ago_time = mktime(0, 0, 0, date('m')-12, date('d'), date('Y'));
    $year_ago_date = date('Y-m-d H:i:s', $year_ago_time);    
    
    for ($i = 12; $i >= 1; $i--) {
        $time = mktime(0, 0, 0, date('m')-$i, date('d'), date('Y'));
        $data[date('M-y', $time)] = 0;
    }
    
    $sql = "SELECT count(status) as num_incidents, status, " .
           "date_format(last_update, '%Y-%m') as month, " .
           "date_format(last_update, '%b-%y') as label " .
           "FROM incident " .
           "WHERE status='Closed' AND last_update >= ? " .
           "GROUP BY month";
    $params = array($year_ago_date);

    if (!$rs = $conn->Execute($sql, $params)) die ($conn->ErrorMsg());
    
    while (!$rs->EOF) {
        $num_inc = $rs->fields['num_incidents'];
        $month   = $rs->fields['label'];
        $data[$month] = $num_inc;
        $rs->MoveNext();
    }
    $labelx = array_keys($data);
    $datay   = array_values($data);
    $title = '';
    $titley = _("Month").'-'._("Year");
    $titlex = _("Num. Incidents");
    $width = 700;
    
} elseif ($by == "resolution_time") {
    $list = Incident::search($conn, array('status' => 'Closed'));
    $ttl_groups[1] = 0;
    $ttl_groups[2] = 0;
    $ttl_groups[3] = 0;
    $ttl_groups[4] = 0;
    $ttl_groups[5] = 0;
    $ttl_groups[6] = 0;

    $total_days = 0;
    $day_count;
    
    foreach ($list as $incident) {
        $ttl_secs = $incident->get_life_time('s');
        $days = round($ttl_secs/60/60/24);
        $total_days += $days;
        $day_count++;
        if ($days < 1) $days = 1;
        if ($days > 6) $days = 6;
        @$ttl_groups[$days]++;
    }
    $datay = array_values($ttl_groups);
    $labelx = array('1 '._("day"), '2 '._("days"), '3 '._("days"),
                    '4 '._("days"), '5 '._("days"), '6 '._("or more"));
    $title = '';
    if($day_count < 1) $day_count = 1;
    $titley = _("Duration in days.") . " " . _("Average:") . " " .  $total_days/$day_count;
    $titlex = _("Num. Incidents");
    $width = 500;
} else {
    die("Invalid by");
}

$background = "white";
$color = "navy";
$color2 = "lightsteelblue";


// Setup graph
$graph = new Graph($width, 250, "auto");
$graph->SetScale("textlin");
$graph->SetMarginColor($background);
$graph->img->SetMargin(40,30,20,40);
$graph->SetShadow();

// Setup graph title
$graph->title->Set($title);
$graph->title->SetFont(FF_FONT1,FS_BOLD);

$bplot = new BarPlot($datay);
$bplot->SetWidth(0.6);
$bplot->SetFillGradient($color,$color2,GRAD_MIDVER);
$bplot->SetColor($color);
$graph->Add($bplot);

$graph->xaxis->SetTickLabels($labelx);
//$graph->xaxis->SetLabelAngle(40); // only with TTF fonts

$graph->title->Set($title);
$graph->xaxis->title->Set($titley);
$graph->yaxis->title->Set($titlex);


$graph->Stroke();
?>
