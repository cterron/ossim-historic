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
* - du()
* - ConvertBytes()
* - ClearCache()
* - CleanCache()
* - GetCacheInfo()
* - PrintCacheInfo()
* Classes list:
*/
require_once 'classes/Session.inc';
Session::logcheck("MenuControlPanel", "ControlPanelSEM");
function du($dir) {
    if (!is_dir($dir)) {
        return;
    }
    $dir = escapeshellarg($dir);
    $res = `du -sk $dir`; // Unix command
    preg_match('/\d+/', $res, $KB); // Parse result
    $B = round($KB[0] * 1024, 1); // From kilobytes to bytes
    return $B;
}
function ConvertBytes($bytes) {
    $types = array(
        'B',
        'KB',
        'MB',
        'GB',
        'TB'
    );
    for ($i = 0; $bytes >= 1024 && $i < (count($types) - 1); $bytes/= 1024, $i++);
    return (round($bytes, 2) . " " . $types[$i]);
}
function ClearCache($dir) {
    if (!is_dir($dir)) {
        return;
    }
    $dir = escapeshellarg($dir);
    system("/bin/rm -f $dir/*.cache.*");
}
function CleanCache($dir) {
    if (!is_dir($dir)) {
        return;
    }
    $dir = escapeshellarg($dir);
    system("find $dir -name \"*.cache.*\" -mtime +1 | xargs rm");
}
function GetCacheInfo($dir) {
    if (!is_dir($dir)) {
        return "pct:0";
    }
    $total_space = disk_total_space($dir);
    $du = du($dir);
    $pct_used = intval(($du) / $total_space * 100);
    return "pct:$pct_used";
}
function PrintCacheInfo($dir) {
    if (!is_dir($dir)) {
        print "Configuration error, cache dir $dir doesn't exist.";
        return;
    }
    $free_space = disk_free_space($dir);
    $total_space = disk_total_space($dir);
    $total_used = $total_space - $free_space;
    $du = du($dir);
    $total_cache_used_pct = intval(($du) / $total_space * 100);
    $total_used_pct = intval((($total_space - $free_space) / $total_space) * 100);
    $status = "<font color=\"green\">OK</font>";
    if (($total_cache_used_pct > 50 && $total_cache_used_pct <= 80) || ($total_used_pct > 50 && $total_used_pct <= 80)) {
        $status = "<font color=\"orange\">Partially Full</font>";
    } elseif (($total_cache_used_pct > 80) || ($total_used_pct > 80)) {
        $status = "<font color=\"red\">Full</font>";
    }
    print "<small>Cache is using " . ConvertBytes($du) . " out of " . ConvertBytes($total_space) . "." . " Since the amount of free space is " . ConvertBytes($free_space) . ", we consider the cache being " . $status . ".</small>";
}
?>
