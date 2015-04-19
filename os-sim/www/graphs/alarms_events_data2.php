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
require_once ('ossim_db.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/Util.inc');
require_once ('classes/Security.inc');
require_once ('charts.php');
$db = new ossim_db();
$conn = $db->connect();
$counter = (isset($_REQUEST['counter'])) ? $_REQUEST['counter'] : 1;
if ($counter == 1) {
    $counter = 2;
} else {
    $counter = 1;
}
switch ($counter) {
    case 1:
        $query = "select plugin_sid.name, count(*) as num from alarm, plugin_sid where alarm.plugin_id = plugin_sid.plugin_id and alarm.plugin_sid = plugin_sid.sid group by alarm.plugin_sid limit 7;";
        break;

    case 2:
        $query = "select count(*) as num, snort.signature.sig_name as name from snort.event, snort.signature where snort.signature.sig_id = snort.event.signature group by snort.event.signature order by num desc limit 7;";
        break;

    default:
        $query = "select count(*) as num, snort.signature.sig_name as name from snort.event, snort.signature where snort.signature.sig_id = snort.event.signature group by snort.event.signature order by num desc limit 7;";
        $chart['chart_type'] = "column";
        break;
}
// PHP/SWF Chart License - Licensed to ossim.com. For distribution with ossim only. No other redistribution / usage allowed.
// For more information please check http://www.maani.us/charts/index.php?menu=License_bulk
$chart['license'] = "J1XF-CMEW9L.HSK5T4Q79KLYCK07EK";
//$chart[ 'chart_data' ] = array ( array ( "", "US","UK","India", "Japan","China" ), array ( "", 50,70,55,60,30 ) );
$chart['chart_pref'] = array(
    'rotation_x' => 60
);
$chart['chart_rect'] = array(
    'x' => 50,
    'y' => 130,
    'width' => 130,
    'height' => 200,
    'positive_alpha' => 0
);
$chart['chart_transition'] = array(
    'type' => "scale",
    'delay' => .1,
    'duration' => .3,
    'order' => "category"
);
$chart['chart_type'] = "3d pie";
$chart['chart_value'] = array(
    'as_percentage' => true,
    'size' => 9,
    'color' => "000000",
    'alpha' => 85
);
$chart['legend_label'] = array(
    'layout' => "vertical",
    'bullet' => "circle",
    'size' => 11,
    'color' => "505050",
    'alpha' => 85,
    'bold' => false
);
$chart['legend_rect'] = array(
    'x' => 220,
    'y' => 220,
    'width' => 20,
    'height' => 40,
    'fill_alpha' => 0
);
$chart['series_color'] = array(
    "cc6600",
    "aaaa22",
    "8800dd",
    "666666",
    "4488aa"
);
$chart['series_explode'] = array(
    0,
    50
);
$legend = array();
$values = array();
if (!$rs = & $conn->Execute($query)) {
    print $conn->ErrorMsg();
    exit();
}
while (!$rs->EOF) {
    array_push($legend, $rs->fields["name"]);
    array_push($values, $rs->fields["num"]);
    $rs->MoveNext();
}
$chart['live_update'] = array(
    'url' => "/ossim/graphs/alarms_events_data2.php?counter=" . $counter . "&time=" . time() ,
    'delay' => 8
);
$chart['chart_data'] = array(
    $legend,
    $values
);
SendChartData($chart);
?>
