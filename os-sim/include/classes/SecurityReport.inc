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
* - SecurityReport()
* - AttackHost()
* - Events()
* - Ports()
* - EventsByRisk()
* - Truncate()
* Classes list:
* - SecurityReport
*/
require_once ('ossim_db.inc');
require_once ('classes/Log_action.inc');
class SecurityReport {
    var $ossim_conn;
    var $snort_conn;
    function SecurityReport() {
        $db = new ossim_db();
        $this->ossim_conn = $db->connect();
        $this->snort_conn = $db->snort_connect();
    }
    function AttackHost($conn, $ip_target, $limit = 10, $type = "event") {
        require_once ('classes/Host.inc');
        require_once ('classes/Net.inc');
        require_once ('classes/Session.inc');
        $list = array();
        $temp_conn = $this->snort_conn;
        if (!strcmp($ip_target, "ip_src")) {
            if ($type == "alarm") $target = "src_ip";
            $title = "Attacker hosts";
        } elseif (!strcmp($ip_target, "ip_dst")) {
            if ($type == "alarm") $target = "dst_ip";
            $title = "Attacked hosts";
        }
        switch ($type) {
            case "alarm":
                if (!strcmp($ip_target, "ip_src")) {
                    $ip_target = "src_ip";
                }
                if (!strcmp($ip_target, "ip_dst")) {
                    $ip_target = "dst_ip";
                }
                $query = OssimQuery("SELECT count($ip_target) AS occurrences,
                  inet_ntoa($ip_target) as ip FROM alarm where $ip_target != INET_ATON('0.0.0.0')
                  GROUP BY $ip_target ORDER BY occurrences DESC");
                $temp_conn = $this->ossim_conn;
                break;

            case "event":
            default:
                $query = OssimQuery("SELECT count($ip_target) AS occurrences,
                  inet_ntoa($ip_target) as ip FROM acid_event where $ip_target != INET_ATON('0.0.0.0')
                  AND ossim_risk_c >= 0 AND ossim_risk_a >= 0
                  GROUP BY $ip_target ORDER BY occurrences DESC");
                $temp_conn = $this->snort_conn;
        }
        if (!$rs = & $temp_conn->Execute($query)) {
            print $temp_conn->ErrorMsg();
        } else {
            $i = 0;
            while (!$rs->EOF) {
                if (Session::hostAllowed($conn, $rs->fields["ip"])) {
                    $tmp_array = array(
                        $rs->fields["ip"],
                        $rs->fields["occurrences"]
                    );
                    array_push($list, $tmp_array);
                    $i+= 1;
                }
                $rs->MoveNext();
                /* check limit */
                if ($i >= $limit) {
                    $rs->EOF = true;
                }
            }
        }
        return $list;
    }
    function Events($limit = 10, $type = "event") {
        $list = array();
        $temp_conn = $this->snort_conn;
        switch ($type) {
            case "alarm":
                $query = "SELECT count(name) AS occurrences, name as sig_name
                            FROM alarm, plugin_sid
                            WHERE plugin_sid.plugin_id = alarm.plugin_id AND
                                  plugin_sid.sid = alarm.plugin_sid
                            GROUP BY name
                            ORDER BY occurrences 
                            DESC LIMIT $limit";
                $temp_conn = $this->ossim_conn;
                break;

            case "event":
            default:
                $query = "SELECT count(sig_name) AS occurrences, sig_name
                            FROM acid_event
                            GROUP BY sig_name
                            ORDER BY occurrences
                            DESC LIMIT $limit";
                $temp_conn = $this->snort_conn;
        }
        if (!$rs = & $temp_conn->Execute($query)) {
            print $temp_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $tmp_array = array(
                    $rs->fields["sig_name"],
                    $rs->fields["occurrences"]
                );
                array_push($list, $tmp_array);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    function Ports($limit = 10, $type = "event") {
        require_once ('classes/Port.inc');
        $list = array();
        $temp_conn = $this->snort_conn;
        switch ($type) {
            case "alarm":
                $query = OssimQuery("SELECT count(dst_port) AS occurrences, dst_port as layer4_dport
                  FROM alarm GROUP BY dst_port ORDER BY occurrences DESC LIMIT $limit");
                $temp_conn = $this->ossim_conn;
                break;

            case "event":
            default:
                $query = OssimQuery("SELECT count(layer4_dport) AS occurrences, layer4_dport
                  FROM acid_event 
                  GROUP BY layer4_dport
                  ORDER BY occurrences DESC LIMIT $limit");
                $temp_conn = $this->snort_conn;
        }
        if (!$rs = & $temp_conn->Execute($query)) {
            print $temp_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $port = $rs->fields["layer4_dport"];
                $service = "unknown";
                $occurrences = $rs->fields["occurrences"];
                if ($port != "NULL" and $occurrences != 0) {
                    if ($port_list = Port::get_list($this->ossim_conn, "WHERE port_number = $port")) {
                        $service = $port_list[0]->get_service();
                    }
                    $tmp_array = array(
                        $port,
                        $service,
                        $occurrences
                    );
                    array_push($list, $tmp_array);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }
    function EventsByRisk($limit = 10, $type = "event") {
        $list = array();
        $temp_conn = $this->snort_conn;
        switch ($type) {
            case "alarm":
                $query = OssimQuery("SELECT name as sig_name, risk as ossim_risk_a from alarm, plugin_sid
                  WHERE plugin_sid.plugin_id = alarm.plugin_id AND plugin_sid.sid = alarm.plugin_sid
                  GROUP BY name ORDER BY risk DESC LIMIT $limit");
                $temp_conn = $this->ossim_conn;
                break;

            case "event":
            default:
                $query = OssimQuery("SELECT sig_name, ossim_risk_a
                  FROM acid_event GROUP BY sig_name
                  ORDER BY ossim_risk_a DESC LIMIT $limit");
                $temp_conn = $this->snort_conn;
        }
        if (!$rs = & $temp_conn->Execute($query)) {
            print $temp_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $tmp_array = array(
                    $rs->fields["sig_name"],
                    $rs->fields["ossim_risk_a"]
                );
                array_push($list, $tmp_array);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    function Truncate($str, $length = 10, $trailing = '...') {
        // take off chars for the trailing
        $length-= strlen($trailing);
        if (strlen($str) > $length) {
            // string exceeded length, truncate and add trailing dots
            return substr($str, 0, $length) . $trailing;
        } else {
            // string was already short enough, return the string
            $res = $str;
        }
        return $res;
    }
}
?>
