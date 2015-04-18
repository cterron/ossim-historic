<?php

require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');

require_once ('../sec_util.php');

/* hosts to show */
if (!$NUM_HOSTS = $_GET["hosts"]) {
    $NUM_HOSTS = 10;
}

/* target must be ip_src or ip_dst */
if (!$target = $_GET["target"]) exit;

/* ossim framework conf */
$conf = new ossim_conf();
$jpgraph = $conf->get_conf("jpgraph_path");

/* snort db connect */
$snort_db = new ossim_db();
$snort_conn = $snort_db->snort_connect();

$query = "SELECT count($target) AS occurrences, inet_ntoa($target) 
    FROM acid_event GROUP BY $target
    ORDER BY occurrences DESC LIMIT $NUM_HOSTS;";

if (!$rs = &$snort_conn->CacheExecute($query)) {
    print $snort_conn->ErrorMsg();
} else {

    if (!strcmp($target, "ip_src")) {
        $title = "TOP ATTACKER";
        $color = "navy";
        $color2 = "lightsteelblue";
        $titlecolor = "darkblue";
    } elseif (!strcmp($target, "ip_dst")) {
        $title = "TOP ATTACKED";
        $color = "darkred";
        $color2 = "lightred";
        $titlecolor = "darkred";
    }
    $background = "#f1f1f1";

    while (!$rs->EOF) {

        $ip = $rs->fields["inet_ntoa($target)"];
        $occurrences = $rs->fields["occurrences"];

        $datax [] = ip2hostname($ip);
        $datay [] = $occurrences;

        $rs->MoveNext();
    }
}
$snort_db->close($snort_conn);



include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_bar.php");

// Setup the graph.
$graph = new Graph(400,250,"auto");    
$graph->img->SetMargin(60,20,30,100);
$graph->SetScale("textlin");
$graph->SetMarginColor("$background");
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

