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
require_once ('classes/Host.inc');
require_once ('classes/Host_group.inc');
require_once ('classes/Net.inc');
require_once ('classes/Net_group.inc');
require_once ('ossim_db.inc');
$db = new ossim_db();
$conn = $db->connect();
$filter = GET('filter');
ossim_valid($filter, OSS_ALPHA, OSS_DIGIT, OSS_PUNC, 'illegal:' . _("Filter"));
if (ossim_error() || $filter == "undefined") $filter = "";
$ossim_hosts = $all_hosts = array();
$total_hosts = 0;
$ossim_nets = array();
if ($host_list = Host::get_list($conn, "", "ORDER BY hostname")) foreach($host_list as $host) if ($filter == "" || ($filter != "" && (preg_match("/$filter/i", $host->get_ip()) || preg_match("/$filter/i", $host->get_hostname())))) {
    $ossim_hosts[$host->get_ip() ] = $host->get_hostname();
    $all_hosts[$host->get_ip() ] = 1;
    $cclass = preg_replace("/(\d+\.)(\d+\.)(\d+)\.\d+/", "\\1\\2\\3", $host->get_ip());
    $all_cclass_hosts[$cclass][] = $host->get_ip();
    $total_hosts++;
}
if ($hg_list = Host_group::get_list($conn, "ORDER BY name")) {
    foreach($hg_list as $hg) {
        $hg_hosts = $hg->get_hosts($conn, $hg->get_name());
        foreach($hg_hosts as $hosts) {
            $ip = $hosts->get_host_ip();
            unset($all_hosts[$ip]);
        }
    }
}
if ($net_list = Net::get_list($conn, "ORDER BY name")) {
    foreach($net_list as $net) {
        $net_name = $net->get_name();
        $net_ips = $net->get_ips();
        $hostin = array();
        foreach($ossim_hosts as $ip => $hname) if ($net->isIpInNet($ip, $net_ips)) {
            $hostin[$ip] = $hname;
            unset($all_hosts[$ip]);
        }
        $ossim_nets[$net_name] = $hostin;
    }
}
echo "[ {title: 'ANY', key:'key1', url:'ANY', icon:'../../pixmaps/theme/any.png', expand:true, children:[\n";
echo "{ key:'key1.1', isFolder:true, icon:'../../pixmaps/theme/host_group.png', title:'Host Group'\n";
if ($hg_list = Host_group::get_list($conn, "ORDER BY name")) {
    echo ", children:[";
    $j = 1;
    foreach($hg_list as $hg) {
        $hg_name = $hg->get_name();
        $html = "";
        $li = "key:'key1.1.$j', url:'HOST_GROUP:$hg_name', icon:'../../pixmaps/theme/host_group.png', title:'$hg_name'\n";
        if ($hg_hosts = $hg->get_hosts($conn, $hg_name)) {
            $k = 1;
            foreach($hg_hosts as $hosts) {
                $host_ip = $hosts->get_host_ip();
                if (isset($ossim_hosts[$host_ip])) { // test filter
                    $html.= "{ key:'key1.1.$j.$k', url:'HOST:$host_ip', icon:'../../pixmaps/theme/host.png', title:'$host_ip <font style=\"font-size:80%\">(" . $ossim_hosts[$host_ip] . ")</font>' },\n";
                    $k++;
                }
            }
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        else echo (($j > 1) ? "," : "") . "{ $li }\n";
        $j++;
    }
    // others
    if (count($all_hosts) > 0) {
        $li = "key:'key1.1.$j', isFolder:true, icon:'../../pixmaps/theme/host_group.png', title:'Others <font style=\"font-weight:normal;font-size:80%\">(" . count($all_hosts) . " hosts)</font>'\n";
        $k = 1;
        $html = "";
        foreach($all_hosts as $ip => $val) {
            $html.= "{ key:'key1.1.$j.$k', url:'HOST:$ip', icon:'../../pixmaps/theme/host.png', title:'$ip <font style=\"font-size:80%\">(" . $ossim_hosts[$ip] . ")</font>' },\n";
            $k++;
        }
        echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
    }
    echo "]";
}
echo "},";
// Nets
echo "{ key:'key1.2', isFolder:true, icon:'../../pixmaps/theme/net.png', title:'Networks'\n";
if ($net_list = Net::get_list($conn, "ORDER BY name")) {
    echo ", children:[";
    $j = 1;
    foreach($net_list as $net) {
        $net_name = $net->get_name();
        $ips = $net->get_ips();
        $html = "";
        $li = "key:'key1.2.$j', url:'NETWORK:$net_name', icon:'../../pixmaps/theme/net.png', title:'$net_name <font style=\"font-size:80%\">($ips)</font>'\n";
        $k = 1;
        foreach($ossim_nets[$net_name] as $ip => $host_name) {
            $html.= "{ key:'key1.2.$j.$k', url:'HOST:$ip', icon:'../../pixmaps/theme/host.png', title:'$ip <font style=\"font-size:80%\">($host_name)</font>' },\n";
            $k++;
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        else echo (($j > 1) ? "," : "") . "{ $li }\n";
        $j++;
    }
    echo "]";
}
echo "},";
// Net groups
echo "{ key:'key1.3', isFolder:true, icon:'../../pixmaps/theme/net_group.png', title:'Network Groups'\n";
if ($net_group_list = Net_group::get_list($conn, "ORDER BY name")) {
    echo ", children:[";
    $j = 1;
    foreach($net_group_list as $net_group) {
        $net_group_name = $net_group->get_name();
        $nets = $net_group->get_networks($conn, $net_group_name);
        $li = "key:'key1.3.$j', url:'NETWORK_GROUP:$net_group_name', icon:'../../pixmaps/theme/net_group.png', title:'$net_group_name'\n";
        $k = 1;
        foreach($nets as $net) {
            $net_name = $net->get_net_name();
            if (isset($ossim_nets[$net_name]) && count($ossim_nets[$net_name]) > 0) {
                $html.= "{ key:'key1.3.$j.$k', url:'NETWORK:$net_name', icon:'../../pixmaps/theme/net.png', title:'$net_name' },\n";
                $k++;
            }
        }
        if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
        else echo (($j > 1) ? "," : "") . "{ $li }\n";
        $j++;
    }
    echo "]";
}
echo "},";
// All hosts
echo "{ key:'key1.4', isFolder:true, icon:'../../pixmaps/theme/host.png', title:'All Hosts <font style=\"font-weight:normal;font-size:80%\">(" . $total_hosts . " hosts)</font>'\n";
echo ", children:[";
$j = 1;
foreach($all_cclass_hosts as $cclass => $hg) {
    $html = "";
    $li = "key:'key1.4.$j', url:'CCLASS:$cclass', icon:'../../pixmaps/theme/host_add.png', title:'$cclass <font style=\"font-weight:normal;font-size:80%\">(" . count($hg) . " hosts)</font>'\n";
    $k = 1;
    foreach($hg as $ip) {
        $hname = ($ip == $ossim_hosts[$ip]) ? $ossim_hosts[$ip] : "$ip <font style=\"font-size:80%\">(" . $ossim_hosts[$ip] . ")</font>";
        $html.= "{ key:'key1.4.$j.$k', url:'HOST:$ip', icon:'../../pixmaps/theme/host.png', title:'$hname' },\n";
        $k++;
    }
    if ($html != "") echo (($j > 1) ? "," : "") . "{ $li, children:[ " . preg_replace("/,$/", "", $html) . " ] }\n";
    $j++;
}
echo "]}";
echo "]} ]\n";
?>
