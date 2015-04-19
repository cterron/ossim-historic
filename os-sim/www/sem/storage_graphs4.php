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
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelSEM");
require_once ('../graphs/charts.php');
require_once ('process.inc');
$db = new ossim_db();
$conn = $db->connect();
$a = $_SESSION["forensic_query"];
$start = $_SESSION["forensic_start"];
$end = $_SESSION["forensic_end"];
$uniqueid = $_GET["uniqueid"];
$label = $_GET["label"];
$what = $_GET["what"];
// PHP/SWF Chart License - Licensed to ossim.com. For distribution with ossim only. No other redistribution / usage allowed.
// For more information please check http://www.maani.us/charts/index.php?menu=License_bulk
$chart['license'] = "J1XF-CMEW9L.HSK5T4Q79KLYCK07EK";
//$chart[ 'chart_data' ] = array ( array ( "", "US","UK","India", "Japan","China" ), array ( "", 50,70,55,60,30 ) );
$chart['chart_pref'] = array(
    'rotation_x' => 60
);
$chart['chart_rect'] = array(
    'x' => 0,
    'y' => 30,
    'width' => 130,
    'height' => 200,
    'positive_alpha' => 0
);
//$chart[ 'chart_transition' ] = array ( 'type'=>"scale", 'delay'=>.1, 'duration'=>.3, 'order'=>"category" );
$chart['chart_type'] = "3d pie";
$chart['chart_value'] = array(
    'as_percentage' => false,
    'size' => 7,
    'color' => "000000",
    'alpha' => 85,
    'position' => "cursor"
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
    'x' => 140,
    'y' => 80,
    'width' => 20,
    'height' => 40,
    'fill_alpha' => 0
);
$chart['draw'] = array(
    array(
        'type' => "text",
        'color' => "000000",
        'alpha' => 75,
        'rotation' => 0,
        'size' => 20,
        'x' => 70,
        'y' => 30,
        'width' => 400,
        'height' => 200,
        'text' => $label,
        'h_align' => "left",
        'v_align' => "top"
    )
);
$chart['link_data'] = array(
    'url' => "javascript:parent.display_info(_col_,'_row_','_value_',_category_,_series_,'$what')",
    'target' => "javascript"
);
$legend = array(
    ""
);
$values = array(
    ""
);
$cmd = process($a, $start, $end, $offset, $sort_order, $what, $uniqueid);
//print $cmd;
if ($cmd != "") {
    $status = exec("$cmd 2>/dev/null", $result);
    $i = 0;
    foreach($result as $res) {
        if (preg_match("/^\s+(\d+)\s+(\S+)/", $res, $matches)) {
            if ($i > 9) break;

            $i++;
            // If it's plugin_id, resolve it
            if ($what == "plugin_id") {
                $query = "select name from plugin where id = " . $matches[2];
                if (!$rs = & $conn->Execute($query)) {
                    print $conn->ErrorMsg();
                    exit();
                }
                if ($rs->fields["name"] != "") {
                    array_push($legend, $rs->fields["name"]);
                } else {
                    array_push($legend, $matches[2]);
                }
            } elseif ($what == "sensor") {
                $query = "select name from sensor where ip  = \"" . $matches[2] . "\"";
                if (!$rs = & $conn->Execute($query)) {
                    print $conn->ErrorMsg();
                    exit();
                }
                if ($rs->fields["name"] != "") {
                    array_push($legend, $rs->fields["name"]);
                } else {
                    array_push($legend, $matches[2]);
                }
            } else {
                // Otherwise, push legend "as is"
                array_push($legend, $matches[2]);
            }
            array_push($values, $matches[1]);
        }
    } // end foreach
    $chart['chart_data'] = array(
        $legend,
        $values
    );
} else {
    $chart['draw'] = array(
        array(
            'type' => "text",
            'color' => "000000",
            'alpha' => 75,
            'rotation' => 0,
            'size' => 10,
            'x' => 10,
            'y' => 30,
            'width' => 400,
            'height' => 200,
            'text' => "Too many lines to graph. Max 500.000",
            'h_align' => "left",
            'v_align' => "top"
        )
    );
    $chart['chart_data'] = array(
        "",
        0
    );
}
SendChartData($chart);
?>
