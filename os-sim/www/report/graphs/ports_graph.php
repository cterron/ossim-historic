<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* Classes list:
*/
require_once 'classes/SecurityReport.inc';
require_once 'classes/Security.inc';
Session::logcheck("MenuReports", "ReportsSecurityReport");
$limit = GET('ports');
$type = GET('type');
ossim_valid($limit, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("Limit"));
ossim_valid($type, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("Report type"));
if (ossim_error()) {
    die(ossim_error());
}
/* ports to show */
if (empty($limit)) {
    $limit = 10;
}
if (empty($type)) {
    $type = "event";
}
$security_report = new SecurityReport();
$list = $security_report->Ports($limit, $type);
$datax = $datay = array();
foreach($list as $l) {
    $datax[] = $l[0];
    $datay[] = $l[2];
}
$conf = $GLOBALS["CONF"];
$jpgraph = $conf->get_conf("jpgraph_path");
require_once "$jpgraph/jpgraph.php";
require_once "$jpgraph/jpgraph_bar.php";
$titlecolor = "darkorange";
$color = "darkorange";
$color2 = "lightyellow";
$background = "#f1f1f1";
$title = "DESTINATION PORTS";
// Setup the graph.
$graph = new Graph(400, 250, "auto");
$graph->img->SetMargin(60, 20, 30, 70);
$graph->SetScale("textlin");
$graph->SetMarginColor("$background");
$graph->SetShadow();
// Set up the title for the graph
$graph->title->Set("$title");
$graph->title->SetFont(FF_FONT1, FS_BOLD, 18);
$graph->title->SetColor("$titlecolor");
// Setup font for axis
$graph->xaxis->SetFont(FF_FONT1, FS_NORMAL, 8);
$graph->yaxis->SetFont(FF_FONT1, FS_NORMAL, 11);
// Show 0 label on Y-axis (default is not to show)
$graph->yscale->ticks->SupressZeroLabel(false);
// Setup X-axis labels
$graph->xaxis->SetTickLabels($datax);
$graph->xaxis->SetLabelAngle(90);
// Create the bar pot
$bplot = new BarPlot($datay);
$bplot->SetWidth(0.6);
// Setup color for gradient fill style
$bplot->SetFillGradient($color, $color2, GRAD_MIDVER);
// Set color for the frame of each bar
$bplot->SetColor($color);
$graph->Add($bplot);
// Finally send the graph to the browser
$graph->Stroke();
unset($graph);
?>
