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
require_once 'classes/Security.inc';
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPolicy");
require_once ('classes/Port_group.inc');
require_once ('ossim_db.inc');
$db = new ossim_db();
$conn = $db->connect();
$port_groups = array();
if ($port_group_list = Port_group::get_list($conn, "ORDER BY name")) {
    foreach($port_group_list as $port_group) {
        $pg_name = $port_group->get_name();
        $pg_ports = $port_group->get_reference_ports($conn, $pg_name);
        $port_groups[] = (!preg_match("/ANY/i", $pg_name)) ? array(
            $pg_name,
            $pg_ports
        ) : array(
            $pg_name,
            array()
        );
    }
}
echo "[ {title: 'Port Groups', key:'key1', isFolder:true, icon:'../../pixmaps/theme/ports.png', expand:true\n";
if (count($port_groups) > 0) {
    echo ", children:[";
    $j = 1;
    foreach($port_groups as $pg) {
        $pg_name = $pg[0];
        $pg_ports = $pg[1];
        $html = "";
        $li = "key:'key1.1.$j', url:'$pg_name', icon:'../../pixmaps/theme/ports.png', title:'$pg_name'\n";
        $k = 1;
        foreach($pg_ports as $pg_port) {
            $port = $pg_port->get_port_number();
            $proto = $pg_port->get_protocol_name();
            $html.= "{ key:'key1.1.$j.$k', url:'noport', icon:'../../pixmaps/theme/ports.png', title:'>$port $proto</font>'},\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        else echo (($j > 1) ? "," : "") . "{ $li }\n";
        $j++;
    }
    echo "]";
}
echo "}]\n";
?>
