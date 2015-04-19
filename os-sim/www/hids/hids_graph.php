<?php
 

require_once ('classes/Host_ids.inc');
require_once ('classes/Security.inc');


$limit = GET('hosts');

ossim_valid($limit, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("limit"));

if (ossim_error()) {
    die(ossim_error());
}
                    
/* hosts to show */
if (empty($limit)) {
    $limit = 10;
}

$hids = new Host_ids("","","","","","","","","","");
$list = $hids->Events($limit);
$data = $legend = array();
foreach ($list as $l) {
    $legend[] = $l[0];
    $data[]   = $l[1];
}
 
$conf = $GLOBALS["CONF"];
$jpgraph = $conf->get_conf("jpgraph_path");

include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_pie.php");

// Setup graph
$graph = new PieGraph(400,240,"auto");
$graph->SetShadow();

// Setup graph title
$graph->title->Set("HIDS Events");
$graph->title->SetFont(FF_FONT1,FS_BOLD);

// Create pie plot
$p1 = new PiePlot($data);
//$p1->SetFont(FF_VERDANA,FS_BOLD);
//$p1->SetFontColor("darkred");
$p1->SetSize(0.2);
$p1->SetCenter(0.35);
$p1->SetLegends($legend);
//$p1->SetStartAngle(M_PI/8);
//$p1->ExplodeSlice(0);

$graph->Add($p1);

$graph->Stroke(); 

?>
