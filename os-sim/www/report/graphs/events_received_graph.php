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
$limit = GET('hosts');
$type = GET('type');
ossim_valid($limit, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("Limit"));
ossim_valid($type, OSS_ALPHA, OSS_NULLABLE, 'illegal:' . _("Report type"));
if (ossim_error()) {
    die(ossim_error());
}
/* hosts to show */
if (empty($limit)) {
    $limit = 10;
}
if (empty($type)) {
    $type = "event";
}
$security_report = new SecurityReport();
$list = $security_report->Events($limit, $type);
$legend = $data = array();
foreach($list as $l) {
    $legend[] = SecurityReport::Truncate($l[0], 60);
    $data[] = $l[1];
}
$conf = $GLOBALS["CONF"];
$jpgraph = $conf->get_conf("jpgraph_path");
require_once "$jpgraph/jpgraph.php";
require_once "$jpgraph/jpgraph_pie.php";
// Setup graph
$graph = new PieGraph(640, 300, "auto");
$graph->SetShadow();
// Setup graph title
if ($type == "event") {
    $graph->title->Set(gettext("ALERTS RECEIVED"));
} elseif ($type == "alarm") {
    $graph->title->Set(gettext("ALARMS RECEIVED"));
}
$graph->title->SetFont(FF_FONT1, FS_BOLD);
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
unset($graph);
?>