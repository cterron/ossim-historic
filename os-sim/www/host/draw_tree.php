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
* - cmpf()
* Classes list:
*/
require_once 'classes/Security.inc';
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Host_services.inc');
require_once ('classes/Host_mac.inc');
require_once ('ossim_db.inc');
function cmpf($a, $b) {
    return (count($a) < count($b));
}
$db = new ossim_db();
$conn = $db->connect();
$filter = GET('filter');
ossim_valid($filter, OSS_ALPHA, OSS_DIGIT, OSS_PUNC, 'illegal:' . _("Filter"));
if (ossim_error() || $filter == "undefined") $filter = "";
$ossim_hosts = $all_hosts = $filterhosts = array();
$total_hosts++;
$ossim_nets = array();
if ($host_list = Host::get_list($conn, "", "ORDER BY hostname")) foreach($host_list as $host) if ($filter == "" || ($filter != "" && (preg_match("/$filter/i", $host->get_ip()) || preg_match("/$filter/i", $host->get_hostname())))) {
    $hname = $host->get_hostname();
    $hip = $host->get_ip();
    $ossim_hosts[$hip] = (trim($hname) != "") ? $hname : $hip;
    $cclas = preg_replace("/(\d+\.)(\d+\.)(\d+)\.\d+/", "\\1\\2\\3", $hip);
    $all_hosts[$cclas][] = $hip;
    $total_hosts++;
}
uasort($all_hosts, 'cmpf');
echo "[ {title: '', isFolder: true, key:'key1', icon:'../../pixmaps/theme/any.png', expand:true, children:[\n";
echo "{ key:'key1.1', isFolder:true, icon:'../../pixmaps/theme/host_os.png', title:'OS'\n";
if ($hg_list = Host_os::get_os_list($conn, $ossim_hosts, $filter)) {
    echo ", children:[";
    $j = 1;
    uasort($hg_list, 'cmpf');
    foreach($hg_list as $os => $hg) {
        $html = "";
        $pix = Host_os::get_os_pixmap_nodb($os, '../../pixmaps/', true);
        if ($pix == "") $pix = "../../pixmaps/theme/host_group.png";
        $li = "key:'key1.1.$j', url:'OS:$os', icon:'$pix', title:'$os <font style=\"font-weight:normal;font-size:80%\">(" . count($hg) . " hosts)</font>'";
        $k = 1;
        foreach($hg as $host_ip => $hname) {
            $hname = ($host_ip == $hname) ? $host_ip : "$host_ip <font style=\"font-size:80%\">($hname)</font>";
            $html.= "{ key:'key1.1.$j.$k', url:'$host_ip', icon:'../../pixmaps/theme/host.png', title:'$hname' },\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        $j++;
    }
    echo "]";
}
echo "},";
//ports
echo "{ key:'key1.2', isFolder:true, icon:'../../pixmaps/theme/ports.png', title:'Ports'\n";
if ($hg_list = Host_services::get_port_protocol_list($conn, $ossim_hosts, $filter)) {
    echo ", children:[";
    $j = 1;
    uasort($hg_list, 'cmpf');
    foreach($hg_list as $pp => $hg) {
        $html = "";
        $li = "key:'key1.2.$j', url:'PORT:$pp', icon:'../../pixmaps/theme/ports.png', title:'$pp <font style=\"font-weight:normal;font-size:80%\">(" . count($hg) . " hosts)</font>'\n";
        $k = 1;
        foreach($hg as $host_ip => $hname) {
            $hname = ($host_ip == $hname) ? $host_ip : "$host_ip <font style=\"font-size:80%\">($hname)</font>";
            $html.= "{ key:'key1.2.$j.$k', url:'$host_ip', icon:'../../pixmaps/theme/host.png', title:'$hname' },\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        $j++;
    }
    echo "]";
}
echo "},";
//mac/vendor
echo "{ key:'key1.3', isFolder:true, icon:'../../pixmaps/theme/mac.png', title:'MAC/Vendor'\n";
if ($hg_list = Host_mac::get_mac_vendor_list($conn, $ossim_hosts, $filter)) {
    echo ", children:[";
    $j = 1;
    uasort($hg_list, 'cmpf');
    foreach($hg_list as $mv => $hg) {
        $html = "";
        $macv = preg_replace("/(..:..:..)-(.*)/", "\\1-<font style=\"font-weight:normal;font-style:italic;font-size:80%\">\\2</font>", $mv);
        $li = "key:'key1.3.$j', url:'MAC:$mv', icon:'../../pixmaps/theme/mac.png', title:'$macv <font style=\"font-weight:normal;font-size:80%\">(" . count($hg) . " hosts)</font>'\n";
        $k = 1;
        foreach($hg as $host_ip => $hname) {
            $hname = ($host_ip == $hname) ? $host_ip : "$host_ip <font style=\"font-size:80%\">($hname)</font>";
            $html.= "{ key:'key1.3.$j.$k', url:'$host_ip', icon:'../../pixmaps/theme/host.png', title:'$hname' },\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        $j++;
    }
    echo "]";
}
echo "}";
// others
if ($total_hosts > 0) {
    echo ",{ key:'key1.4', isFolder:true, icon:'../../pixmaps/theme/host_group.png', title:'All Hosts <font style=\"font-weight:normal;font-size:80%\">(" . $total_hosts . " hosts)</font>'\n";
    echo ", children:[";
    $j = 1;
    foreach($all_hosts as $cclass => $hg) {
        $html = "";
        $li = "key:'key1.4.$j', url:'CCLASS:$cclass', icon:'../../pixmaps/theme/host_group.png', title:'$cclass <font style=\"font-weight:normal;font-size:80%\">(" . count($hg) . " hosts)</font>'\n";
        $k = 1;
        foreach($hg as $ip) {
            $hname = ($ip == $ossim_hosts[$ip]) ? $ossim_hosts[$ip] : "$ip <font style=\"font-size:80%\">(" . $ossim_hosts[$ip] . ")</font>";
            $html.= "{ key:'key1.4.$j.$k', url:'$ip', icon:'../../pixmaps/theme/host.png', title:'$hname' },\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        $j++;
    }
    echo "]}";
}
echo "]} ]\n";
?>
