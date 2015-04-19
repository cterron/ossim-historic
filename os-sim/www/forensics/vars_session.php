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
// TIME RANGE
if ($_GET['time_range'] != "") {
    // defined => save into session
    if (isset($_GET['time'])) $_SESSION['time'] = $_GET['time'];
    if (isset($_GET['time_cnt'])) $_SESSION['time_cnt'] = $_GET['time_cnt'];
    if (isset($_GET['time_range'])) $_SESSION['time_range'] = $_GET['time_range'];
} elseif ($_SESSION['time_range'] != "") {
    // not defined => load from session or unset
    if ($_GET["clear_criteria"] == "time") {
        unset($_SESSION['time']);
        unset($_SESSION['time_cnt']);
        $_GET['time_range'] = "all";
        $_SESSION['time_range'] = $_GET['time_range'];
    } else {
        if (isset($_SESSION['time'])) $_GET['time'] = $_SESSION['time'];
        if (isset($_SESSION['time_cnt'])) $_GET['time_cnt'] = $_SESSION['time_cnt'];
        if (isset($_SESSION['time_range'])) $_GET['time_range'] = $_SESSION['time_range'];
    }
} else {
    // default => load today values
    $_GET['time'][0] = array(
        null,
        ">=",
        date("m") ,
        date("d") ,
        date("Y") ,
        null,
        null,
        null,
        null,
        null
    );
    $_GET['time_cnt'] = "1";
    $_GET['time_range'] = "today";
    $_SESSION['time'] = $_GET['time'];
    $_SESSION['time_cnt'] = $_GET['time_cnt'];
    $_SESSION['time_range'] = $_GET['time_range'];
}
// PLAYLOAD
// IP
// LAYER 4 PROTO
//print_r($_GET);
//print_r($_SESSION['time']);

?>