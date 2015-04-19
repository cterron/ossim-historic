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
* - get_date_str()
* - get_all_csv()
* - get_year_csv()
* - get_month_csv()
* - get_day_csv()
* Classes list:
*/
function get_date_str($month = "", $day = "", $type = "") {
    if ($type == "hours") return " at " . sprintf("%02d", $day) . "(h)";
    //echo $month;
    $month = mktime(0, 0, 0, $month, 0, 0);
    //echo strftime("%b", $month);
    if ($day != "") return strftime("%b", $month) . " $day, " . date("Y");
    return strftime("%b", $month) . ", " . date("Y");
}
function get_all_csv() {
    $config = parse_ini_file("everything.ini");
    if (file_exists($config['log_dir'] . ".csv_total_events")) $csv = file_get_contents($config['log_dir'] . ".csv_total_events");
    else $csv = array();
    $all = array();
    if (strlen($csv) > 0) $lines = explode("\n", $csv);
    if (count($lines) > 1) foreach($lines as $line) {
        $val = explode(",", $line);
        if ($val[0] >= 1) $all[$val[0]] = $val[1];
    }
    if (count($all) >= 1) for ($i = 2005; $i <= 2015; $i++) if (!isset($all[$i])) $all[$i] = 0;
    ksort($all);
    return $all;
}
function get_year_csv($year) {
    $config = parse_ini_file("everything.ini");
    if (file_exists($config['log_dir'] . "$year/.csv_total_events")) $csv = file_get_contents($config['log_dir'] . "$year/.csv_total_events");
    else $csv = array();
    $months = array();
    if (strlen($csv) > 0) $lines = explode("\n", $csv);
    if (count($lines) > 1) foreach($lines as $line) {
        $val = explode(",", $line);
        if ($val[0] >= 1) $months[(int)$val[0]] = $val[1];
    }
    for ($i = 1; $i <= 12; $i++) if (!isset($months[$i])) $months[$i] = 0;
    ksort($months);
    return $months;
}
function get_month_csv($year, $month) {
    $config = parse_ini_file("everything.ini");
    if (file_exists($config['log_dir'] . "$year/$month/.csv_total_events")) $csv = file_get_contents($config['log_dir'] . "$year/$month/.csv_total_events");
    else $csv = array();
    $days = array();
    //echo $csv;
    if (strlen($csv) > 0) $lines = explode("\n", $csv);
    if (count($lines) > 1) foreach($lines as $line) {
        $val = explode(",", $line);
        if ($val[0] >= 1) $days[sprintf("%d", $val[0]) ] = $val[1];
    }
    $final = array();
    for ($a = 31; $a >= 1; $a--) {
        $ind = sprintf("%d", $a);
        //		echo $days[$ind]."<br>";
        if ($days[$ind] <= 0) $final[$ind] = "0";
        else $final[$ind] = $days[$ind];
    }
    return array_reverse($final);
}
function get_day_csv($year, $month, $day) {
    $config = parse_ini_file("everything.ini");
    if (file_exists($config['log_dir'] . "$year/$month/" . sprintf("%02d", $day) . "/.csv_total_events")) $csv = file_get_contents($config['log_dir'] . "$year/$month/" . sprintf("%02d", $day) . "/.csv_total_events");
    else $csv = array();
    $days = array();
    if (strlen($csv) > 0) $lines = explode("\n", $csv);
    if (count($lines) > 1) for ($a = 23; $a >= 0; $a--) $days[$a] = 0;
    if (count($lines) > 1) foreach($lines as $line) {
        $val = explode(",", $line);
        if ($val[0] >= 1) $days[sprintf("%d", $val[0]) ] = $val[1];
    }
    return array_reverse($days);
}
?>
