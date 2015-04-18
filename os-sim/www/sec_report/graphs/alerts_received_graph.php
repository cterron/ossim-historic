<?php

require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');

require_once ('../sec_util.php');

/* hosts to show */
if (!$NUM_HOSTS = $_GET["hosts"]) {
    $NUM_HOSTS = 10;
}

/* snort db connect */
$snort_db = new ossim_db();
$snort_conn = $snort_db->snort_connect();

/* ossim framework conf */
$conf = new ossim_conf();
$jpgraph = $conf->get_conf("jpgraph_path");

$query = "SELECT count(sig_name) AS occurrences, sig_name
    FROM acid_event GROUP BY sig_name
    ORDER BY occurrences DESC LIMIT $NUM_HOSTS;";

if (!$rs = &$snort_conn->CacheExecute($query)) {
    print $snort_conn->ErrorMsg();
} else {

    while (!$rs->EOF) 
    {
        $legend [] = $rs->fields["sig_name"];
        $data [] = $rs->fields["occurrences"];
        
        $rs->MoveNext();
    }
}
$snort_db->close($snort_conn);
 

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
