<?php

require_once ('classes/SecurityReport.inc');

/* hosts to show */
if (!$limit = $_GET["hosts"]) {
    $limit = 10;
}

/* target must be ip_src or ip_dst */
if (!$target = $_GET["target"]) exit;
    

if (!$type = $_GET["type"]) {
    $type = "event";
} 

if (!$type == "event") {
    if($target == "ip_dst") $target = "dst_ip";
    if($target == "ip_src") $target = "src_ip";
}



$security_report = new SecurityReport();

if (!strcmp($target, "ip_src") || !strcmp($target, "src_ip")) {
    $title = "TOP ATTACKER";
    $color = "navy";
    $color2 = "lightsteelblue";
    $titlecolor = "darkblue";
} elseif (!strcmp($target, "ip_dst") || !strcmp($target, "dst_ip")) {
    $title = "TOP ATTACKED";
    $color = "darkred";
    $color2 = "lightred";
    $titlecolor = "darkred";
}

$list = $security_report->AttackHost($security_report->ossim_conn,
                                     $target, $limit, $type);
foreach ($list as $l) {
    $datax[] = Host::ip2hostname($security_report->ossim_conn, $l[0]);
    $datay[] = $l[1];
}

require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
$jpgraph = $conf->get_conf("jpgraph_path");

include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_bar.php");

// Setup the graph.
$graph = new Graph(400,250,"auto");    
$graph->img->SetMargin(60,20,30,100);
$graph->SetScale("textlin");
$graph->SetMarginColor("white");
$graph->SetShadow();

// Set up the title for the graph
$graph->title->Set("$title");
$graph->title->SetFont(FF_FONT1,FS_BOLD,18);
$graph->title->SetColor("$titlecolor");

// Setup font for axis
$graph->xaxis->SetFont(FF_FONT1,FS_NORMAL,8);
$graph->yaxis->SetFont(FF_FONT1,FS_NORMAL,11);

// Show 0 label on Y-axis (default is not to show)
$graph->yscale->ticks->SupressZeroLabel(false);

// Setup X-axis labels
$graph->xaxis->SetTickLabels($datax);
$graph->xaxis->SetLabelAngle(90);

// Create the bar pot
$bplot = new BarPlot($datay);
$bplot->SetWidth(0.6);

// Setup color for gradient fill style
$bplot->SetFillGradient("$color",$color2,GRAD_MIDVER);

// Set color for the frame of each bar
$bplot->SetColor("$color");
$graph->Add($bplot);

// Finally send the graph to the browser
$graph->Stroke();

?>

