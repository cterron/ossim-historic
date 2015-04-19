<?php
require_once 'classes/Session.inc';
require_once 'classes/Incident.inc';
require_once 'ossim_db.inc';

Session::logcheck("MenuIncidents", "IncidentsReport");

if (!$by = $_GET["by"]) {
    require_once ('ossim_error.inc');
    $error = new OssimError();
    $error->display("FORM_MISSING_FIELDS");
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
} elseif ($by == "status") {
    $title = _("INCIDENT BY STATUS");
    $list = Incident::incidents_by_status($conn);
}

foreach ($list as $l) {
        $legend[] = $l[0];
        $data[]   = $l[1];
}

$db->close($conn);

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

$graph->Add($p1);
$graph->Stroke();
?>
