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

include ("$jpgraph/jpgraph.php");
include ("$jpgraph/jpgraph_pie.php");

$db = new ossim_db();
$conn = $db->connect();

if ($by == "user") {
    $list = Incident::incidents_by_user($conn);
    $title = _("INCIDENT BY USER");
} elseif ($by == "type") {
    $list = Incident::incidents_by_type($conn);
    $title = _("INCIDENT BY TYPE");
} elseif ($by == "type_descr") {
    $list = Incident::incidents_by_type_descr($conn);
    $title = _("INCIDENT BY TYPE DESCRIPTION");

} elseif ($by == "status") {
    $title = _("INCIDENT BY STATUS");
    $list = Incident::incidents_by_status($conn);
}

foreach ($list as $l) {
        $legend[] = $l[0];
        $data[]   = $l[1];
}

$db->close($conn);

if ($by == "type_descr")
{
  // Setup graph
  $graph = new PieGraph(800,450,"auto");
  //$graph = new PieGraph(500,250,"auto");
  $graph->SetShadow();

  // Setup graph title
  $graph->title->Set($title);
  $graph->title->SetFont(FF_FONT1,FS_BOLD);

  $graph->legend->Pos(0.01,0.9,'left','bottom'); 
  //$graph->legend->AbsPos(200,100,'left','top'); 
  //$graph->legend->SetColumns(1); 


  $graph->legend->SetMarkAbsSize(10);
  // Create pie plot
  $p1 = new PiePlot($data);
  //$p1->SetFont(FF_VERDANA,FS_BOLD);
  //$p1->SetFontColor("darkred");
  $p1->SetSize(0.2);
  //$p1->SetCenter(0.30);
  $p1->SetCenter(0.45, 0.3);
  $p1->SetLegends($legend);
  //$graph->legend->Pos(0.5,0.0.5,'left','bottom'); 
  //$p1->SetLabelType(PIE_VALUE_ABS); 
  //$p1->SetStartAngle(M_PI/8);
  //$p1->ExplodeSlice(0);
}
else
{
  // Setup graph
  $graph = new PieGraph(500,250,"auto");
  $graph->SetShadow();

  // Setup graph title
  $graph->title->Set($title);
  $graph->title->SetFont(FF_FONT1,FS_BOLD);

  // Create pie plot
  $p1 = new PiePlot($data);
  //$p1->SetFont(FF_VERDANA,FS_BOLD);
  //$p1->SetFontColor("darkred");
  $p1->SetSize(0.3);
  $p1->SetCenter(0.30);
  $p1->SetLegends($legend);
  //$p1->SetStartAngle(M_PI/8);
  //$p1->ExplodeSlice(0);

}
$graph->Add($p1);
$graph->Stroke();
?>
