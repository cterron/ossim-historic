<?php
 

require_once ('classes/SecurityReport.inc');

/* hosts to show */
if (!$limit = $_GET["hosts"]) {
    $limit = 10;
}

$security_report = new SecurityReport();
$list = $security_report->Alerts($limit);
foreach ($list as $l) {
    $legend[] = SecurityReport::Truncate($l[0],60);
    $data[]   = $l[1];
}
 
$conf = new ossim_conf();
$jpgraph = $conf->get_conf("jpgraph_path");

include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_pie.php");

// Setup graph
$graph = new PieGraph(600,300,"auto");
$graph->SetShadow();

// Setup graph title
$graph->title->Set("ALERTS RECEIVED");
$graph->title->SetFont(FF_FONT1,FS_BOLD);

// Create pie plot
$p1 = new PiePlot($data);
//$p1->SetFont(FF_VERDANA,FS_BOLD);
//$p1->SetFontColor("darkred");
$p1->SetSize(0.2);
$p1->SetCenter(0.15);
$p1->SetLegends($legend);
//$p1->SetStartAngle(M_PI/8);
//$p1->ExplodeSlice(0);

$graph->Add($p1);

$graph->Stroke(); 

?>
