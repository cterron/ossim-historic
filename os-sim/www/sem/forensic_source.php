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
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelSEM");
require_once ('ossim_db.inc');
require_once ('classes/Util.inc');
require_once ('classes/Security.inc');
require_once ('../graphs/charts.php');
require_once ('forensics_stats.inc');
$db = new ossim_db();
$conn = $db->connect();
// PHP/SWF Chart License - Licensed to ossim.com. For distribution with ossim only. No other redistribution / usage allYearsowed.
// For more information please check http://www.maani.us/charts/index.php?menu=License_bulk
$chart['license'] = "J1XF-CMEW9L.HSK5T4Q79KLYCK07EK";
$chart['axis_category'] = array(
    'size' => 10,
    'color' => "000000",
    'alpha' => 75,
    'orientation' => "diagonal_up"
);
$chart['axis_ticks'] = array(
    'value_ticks' => true,
    'category_ticks' => true,
    'minor_count' => 1
);
$chart['axis_value'] = array(
    'size' => 10,
    'color' => "FFFFFF",
    'alpha' => 75
);
$chart['chart_border'] = array(
    'top_thickness' => 0,
    'bottom_thickness' => 2,
    'left_thickness' => 2,
    'right_thickness' => 0
);
$chart['chart_grid_h'] = array(
    'thickness' => 1,
    'type' => "dashed"
);
$chart['chart_grid_v'] = array(
    'thickness' => 1,
    'type' => "solid"
);
$chart['chart_rect'] = array(
    'x' => 100,
    'y' => 05,
    'width' => 900,
    'height' => 150,
    'color' => '000000',
    'positive_color' => "000000",
    'positive_alpha' => 50
);
$chart['chart_pref'] = array(
    'rotation_x' => 15,
    'rotation_y' => 0
);
$chart['chart_value'] = array(
    'position' => 'cursor',
    'hide_zero' => 'true',
    'size' => '12',
    'color' => '0044FF',
    'alpha' => '100'
);
$chart['chart_type'] = "stacked column";
$chart['chart_transition'] = array(
    'type' => "none",
    'delay' => 0,
    'duration' => 1,
    'order' => "series"
);
$chart['legend'] = array(
    'layout' => "hide",
    'transition' => "none"
);
$chart['legend_label'] = array(
    'layout' => "horizontal",
    'font' => "arial",
    'bold' => true,
    'size' => 0,
    'color' => "000000",
    'alpha' => 0
);
$chart['legend_rect'] = array(
    'x' => 0,
    'y' => 0,
    'width' => 0,
    'height' => 0,
    'margin' => 0,
    'fill_color' => "000000",
    'fill_alpha' => 0,
    'line_color' => "000000",
    'line_alpha' => 0,
    'line_thickness' => 0,
    'layout' => "hide"
);
$chart['series_color'] = array(
    "ff6600",
    "88ff00",
    "8866ff"
);
$chart['series_gap'] = array(
    'bar_gap' => 0,
    'set_gap' => 20
);
$chart['draw'] = array(
    array(
        'type' => "text",
        'color' => "ffffff",
        'alpha' => 75,
        'rotation' => 0,
        'size' => 16,
        'x' => 150,
        'y' => 15,
        'width' => 1000,
        'height' => 200,
        'text' => "OSSIM FORENSIC LOGS: Total Events",
        'h_align' => "left",
        'v_align' => "top"
    )
);
$gt = $_SESSION["graph_type"];
$cat = $_SESSION["cat"];
//if(!preg_match("/all|month|year|day/",$cat))
//  $gt="all";
switch ($gt) {
    case "year":
        $t_year = $cat;
        break;

    case "month":
        $tmp = explode(",", $cat);
        $t_year = str_replace(" ", "", $tmp[1]);
        $t_month = str_replace(" ", "", $tmp[0]);
        break;

    case "day":
        $tmp = explode(",", $cat);
        $t_year = str_replace(" ", "", $tmp[1]);
        $tmp = explode(" ", $tmp[0]);
        $t_month = str_replace(" ", "", $tmp[0]);
        $t_day = str_replace(" ", "", $tmp[1]);
        break;
}
$t_month = date('m', strtotime("01 " . $t_month . " 2000"));
//echo "year: $t_year, month: $t_month, day: $t_day";
//Target allYears by default
if ($gt == "") $gt = "allYears";
$chart['link_data'] = array(
    'url' => "javascript:parent.graph_by_date( _col_, _row_, _value_, _category_, _series_, '" . $t_year . "','" . $t_month . "')",
    'target' => "javascript"
);
$allYears = array();
if ($gt == "all") $allYears = get_all_csv();
if ($gt == "year") $years = get_year_csv($t_year);
else $years = get_year_csv(date("Y"));
if ($gt == "month") $months = get_month_csv($t_year, $t_month);
else $months = get_month_csv(date("Y") , date("m"));
if ($gt == "day") $days = get_day_csv($t_year, $t_month, $t_day);
$general = array();
$i = 0;
$j = 0;
$general[$j][$i++] = "NULL";
if ($gt == "all" || $gt != "month" && $gt != "year" && $gt != "day") foreach($allYears as $k => $v) $general[$j][$i++] = $k;
if ($gt == "year") foreach($years as $k => $v) $general[$j][$i++] = get_date_str($k + 1);
if ($gt == "month") foreach($months as $k => $v) $general[$j][$i++] = get_date_str($t_month + 1, $k + 1, "days");
if ($gt == "day") foreach($days as $k => $v) $general[$j][$i++] = get_date_str("", $k, "hours");
for ($a = 1; $a < 5; $a++) {
    $i = 0;
    switch ($a) {
        case 1:
            //$general[$a][$i++]="Year stats";
            $general[$a][$i++] = "";
            break;

        case 2:
            //$general[$a][$i++]="Month stats";
            $general[$a][$i++] = "";
            break;

        case 3:
            //$general[$a][$i++]="Day stats";
            $general[$a][$i++] = "";
            break;

        case 4:
            //$general[$a][$i++]="Hour stats";
            $general[$a][$i++] = "";
            break;
    }
    if ($gt == "all" || $gt != "month" && $gt != "year" && $gt != "day") foreach($allYears as $k => $v) if ($a == 1) $general[$a][$i++] = $v; //number_format($v,0,',','.');
    else $general[$a][$i++] = "";
    if ($gt == "year") foreach($years as $k => $v) if ($a == 2) $general[$a][$i++] = $v; //number_format($v,0,',','.');
    else $general[$a][$i++] = "";
    if ($gt == "month") if ($a == 3) foreach($months as $k => $v) $general[$a][$i++] = $v; //number_format($v,0,',','.');
    else $general[$a][$i++] = "";
    if ($gt == "day") if ($a == 4) foreach($days as $k => $v) $general[$a][$i++] = $v; //number_format($v,0,',','.');
    else $general[$a][$i++] = "";
}
$chart['chart_data'] = $general;
SendChartData($chart);
?>
