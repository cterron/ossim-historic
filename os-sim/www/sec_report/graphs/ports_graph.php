<?php

require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');

/* hosts to show */
if (!$PORTS = $_GET["ports"]) {
    $PORTS = 10;
}

/* ossim framework conf */
$conf = new ossim_conf();
$jpgraph = $conf->get_conf("jpgraph_path");

/* snort db connect */
$snort_db = new ossim_db();
$snort_conn = $snort_db->snort_connect();

$query = "SELECT count(layer4_dport) AS occurrences, layer4_dport 
    FROM acid_event GROUP BY layer4_dport
    ORDER BY occurrences DESC LIMIT $PORTS;";

if (!$rs = &$snort_conn->CacheExecute($query)) {
    print $snort_conn->ErrorMsg();
} else {

    while (!$rs->EOF) {

        $datay [] = $rs->fields["occurrences"];
        $datax [] = $rs->fields["layer4_dport"];
        $rs->MoveNext();
    }
}
$snort_db->close($snort_conn);

include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_bar.php");

$titlecolor = "darkorange";
$color = "darkorange";
$color2 = "lightyellow";
$background = "#f1f1f1";
$title = "DESTINATION PORTS";


// Setup the graph.
$graph = new Graph(400,250,"auto");    
$graph->img->SetMargin(60,20,30,70);
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
$bplot->SetFillGradient($color,$color2,GRAD_MIDVER);

// Set color for the frame of each bar
$bplot->SetColor($color);
$graph->Add($bplot);

// Finally send the graph to the browser
$graph->Stroke();
?>
