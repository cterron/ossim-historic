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
* - Alarm()
* - get_event_id()
* - get_backlog_id()
* - get_timestamp()
* - get_status()
* - get_since()
* - get_plugin_id()
* - get_plugin_sid()
* - get_protocol()
* - get_src_ip()
* - get_dst_ip()
* - get_src_port()
* - get_dst_port()
* - get_src_service()
* - get_dst_service()
* - get_risk()
* - get_asset_src()
* - get_asset_dst()
* - get_rule_level()
* - get_sensors()
* - get_snort_sid()
* - get_snort_cid()
* - get_alarm()
* - get_sid_name()
* - get_count()
* - __get_max_sensor()
* - __get_sensors()
* - __get_since()
* - get_list()
* - get_list3()
* - get_max_byfield()
* - get_list2()
* - get_events()
* - get_alarm_stats()
* - delete_day()
* - close()
* - open()
* - delete()
* - delete_from_backlog()
* - purge()
* Classes list:
* - Alarm
*/
require_once ('classes/Net.inc');
require_once ('classes/Session.inc');
require_once ('classes/Security.inc');
require_once ('classes/Log_action.inc');
require_once ("ossim_db.inc");
class Alarm {
    var $event_id;
    var $backlog_id;
    var $timestamp;
    var $status;
    var $since;
    var $plugin_id;
    var $plugin_sid;
    var $protocol;
    var $src_ip;
    var $dst_ip;
    var $src_port;
    var $dst_port;
    var $risk;
    var $sensors = array();
    var $snort_sid;
    var $snort_cid;
    var $rule_level;
    var $alarm;
    var $sid_name;
    function Alarm($event_id, $backlog_id, $timestamp, $status, $since, $plugin_id, $plugin_sid, $protocol, $src_ip, $dst_ip, $src_port, $dst_port, $risk, $sensors, $snort_sid, $snort_cid, $sid_name, $rule_level = "", $alarm = 1) {
        $this->event_id = intval($event_id);
        $this->backlog_id = intval($backlog_id);
        $this->timestamp = $timestamp;
        $this->status = $status;
        $this->since = $since;
        $this->plugin_id = intval($plugin_id);
        $this->plugin_sid = intval($plugin_sid);
        $this->protocol = $protocol;
        $this->src_ip = $src_ip;
        $this->dst_ip = $dst_ip;
        $this->src_port = intval($src_port);
        $this->dst_port = intval($dst_port);
        $this->risk = intval($risk);
        $this->snort_sid = intval($snort_sid);
        $this->snort_cid = intval($snort_cid);
        $this->rule_level = intval($rule_level);
        $this->alarm = $alarm;
        $this->sid_name = $sid_name;
        if (is_array($sensors)) $this->sensors = $sensors;
        else $this->sensors = array(
            $sensors
        );
    }
    function get_event_id() {
        return $this->event_id;
    }
    function get_backlog_id() {
        return $this->backlog_id;
    }
    function get_timestamp() {
        return $this->timestamp;
    }
    function get_status() {
        return $this->status;
    }
    function get_since() {
        return $this->since;
    }
    function get_plugin_id() {
        return $this->plugin_id;
    }
    function get_plugin_sid() {
        return $this->plugin_sid;
    }
    function get_protocol() {
        return $this->protocol;
    }
    function get_src_ip() {
        return $this->src_ip;
    }
    function get_dst_ip() {
        return $this->dst_ip;
    }
    function get_src_port() {
        return $this->src_port;
    }
    function get_dst_port() {
        return $this->dst_port;
    }
    function get_src_service() {
        return $this->src_service;
    }
    function get_dst_service() {
        return $this->dst_service;
    }
    function get_risk() {
        return $this->risk;
    }
    function get_asset_src() {
        return $this->asset_src;
    }
    function get_asset_dst() {
        return $this->asset_dst;
    }
    function get_rule_level() {
        return $this->rule_level;
    }
    function get_sensors() {
        return $this->sensors;
    }
    function get_snort_sid() {
        return $this->snort_sid;
    }
    function get_snort_cid() {
        return $this->snort_cid;
    }
    function get_alarm() {
        return $this->alarm;
    }
    function get_sid_name() {
        return $this->sid_name;
    }
    /* deprecated */
    function get_count($conn, $src_ip = "", $dst_ip = "", $hide_closed = 1) {
        $count = 0;
        $query = "SELECT event_id, src_ip, dst_ip FROM alarm ";
        if ($src_ip && $dst_ip) $where = " WHERE (alarm.src_ip = inet_aton('$src_ip') OR
                             alarm.dst_ip = inet_aton('$dst_ip'))";
        elseif ($src_ip) $where = " WHERE alarm.src_ip = inet_aton('$src_ip') ";
        elseif ($dst_ip) $where = " WHERE alarm.dst_ip = inet_aton('$dst_ip') ";
        else $where = "";
        if ($hide_closed == 1) {
            if ($where == "") $where = " WHERE alarm.status = 'open' ";
            else $where = " AND (alarm.status = 'open') ";
        }
        $query = OssimQuery($query . $where);
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if ((Session::hostAllowed($conn, $rs->fields["src_ip"])) or (Session::hostAllowed($conn, $rs->fields["dst_ip"]))) {
                    $count++;
                }
                $rs->MoveNext();
            }
            return $count;
        }
    }
    /* function used in get_list to get the sensor asociated with the alarm */
    function __get_max_sensor($conn, $backlog_id) {
        $sensor = "-";
        $sql = "SELECT count(*) AS count, event.sensor AS sensor FROM event, backlog_event WHERE backlog_event.backlog_id = ? AND
                backlog_event.event_id = event.id GROUP BY event.sensor ORDER by count DESC LIMIT 1";
        $params = array(
            $backlog_id
        );
        if (!$rs = & $conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            $sensor = $rs->fields["sensor"];
        }
        return $sensor;
    }
    function __get_sensors($conn, $event_id, $backlog_id) {
        $sensor = array();
        if ($backlog_id != 0) {
            $sql = "SELECT distinct event.sensor AS sensor FROM event, backlog_event 
                    WHERE backlog_event.backlog_id = ? AND backlog_event.event_id = event.id";
            $params = array(
                $backlog_id
            );
            if (!$rs = & $conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
                return $sensor;
            }
            while (!$rs->EOF) {
                $sensor[] = $rs->fields["sensor"];
                $rs->MoveNext();
            }
        } else {
            $sql = "SELECT sensor FROM event WHERE id = ?";
            $params = array(
                $event_id
            );
            if (!$rs = & $conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
                return $sensor;
            } elseif (!$rs->EOF) {
                $sensor[] = $rs->fields["sensor"];
            }
        }
        return $sensor;
    }
    /* function used in get_list to get the date when alarm began */
    function __get_since($conn, $event_id, $backlog_id) {
        $since = "-";
        if ($backlog_id != 0) {
            $sql = "SELECT min(event.id) AS event_id, event.timestamp as since FROM event ,backlog_event 
                    WHERE backlog_event.backlog_id = ? AND backlog_event.event_id = event.id
                    GROUP BY backlog_event.backlog_id";
            $params = array(
                $backlog_id
            );
            if (!$rs = & $conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
            } elseif (!$rs->EOF) {
                $since = $rs->fields["since"];
            }
        } else {
            $sql = "SELECT timestamp FROM event WHERE id = ?";
            $params = array(
                $event_id
            );
            if (!$rs = & $conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
            } elseif (!$rs->EOF) {
                $since = $rs->fields["timestamp"];
            }
        }
        return $since;
    }
    function get_list($conn, $src_ip, $dst_ip, $hide_closed = 1, $order, $inf = null, $sup = null, $from_date = null, $to_date = null) {
        $list = array();
        $where = array();
        /* ip filter */
        if (valid_ip($src_ip)) $src_ip.= "/32";
        if (valid_ip($dst_ip)) $dst_ip.= "/32";
        if (isset($src_ip)) list($source_start, $source_end) = Util::cidr_conv($src_ip);
        if (isset($dst_ip)) list($dest_start, $dest_end) = Util::cidr_conv($dst_ip);
        /* If both src and dst are equal, try to find all alarms where src OR dst are equal to the IP */
        if ($src_ip && ($src_ip == $dst_ip)) {
            $where[] = "(inet_aton('$source_start') <= src_ip AND inet_aton('$source_end') >= src_ip)
                        OR
                        (inet_aton('$dest_start') <= dst_ip AND inet_aton('$dest_end') >= dst_ip)";
        } elseif ($src_ip) {
            $where[] = "(inet_aton('$source_start') <= src_ip AND inet_aton('$source_end') >= src_ip)";
        } elseif ($dst_ip) {
            $where[] = "(inet_aton('$dest_start') <= dst_ip AND inet_aton('$dest_end') >= dst_ip)";
        }
        /* hide closed */
        if ($hide_closed == 1) {
            $where[] = "alarm.status = 'open' ";
        }
        /* date range filter */
        if ($from_date) {
            list($y, $m, $d) = preg_split('#-#', $from_date);
            if (!checkdate($m, $d, $y)) {
                print _("Invalid date") . ": $from_date";
                return array();
            }
            $where[] = "(timestamp >= '$from_date')";
        }
        if ($to_date) {
            list($y, $m, $d) = preg_split('#-#', $to_date);
            if (!checkdate($m, $d, $y)) {
                print _("Invalid date") . ": $to_date";
                return array();
            }
            $where[] = "(timestamp <= '$to_date 23:59:59')";
        }
        /* number of events per page */
        $limit = "";
        if ($sup !== null && $inf !== null) {
            $nevents = $sup - $inf;
            $limit = "LIMIT $nevents OFFSET $inf";
        }
        $sql_where = count($where) ? 'WHERE ' . implode(" AND ", $where) : "";
        $sql = "SELECT *, timestamp + 0 AS timestamp, 
                  inet_ntoa(src_ip), inet_ntoa(dst_ip) 
                FROM alarm $sql_where $order $limit";
        if (!$rs = & $conn->Execute($sql)) {
            print $conn->ErrorMsg();
            return array();
        }
        while (!$rs->EOF) {
            $sensors = Alarm::__get_sensors($conn, $rs->fields["event_id"], $rs->fields["backlog_id"]);
            $sensor_allowed = 0;
            foreach($sensors as $sensor) {
                if (Session::hostAllowed($conn, $sensor)) $sensor_allowed = 1;
            }
            if ((Session::hostAllowed($conn, $rs->fields["inet_ntoa(src_ip)"])) or (Session::hostAllowed($conn, $rs->fields["inet_ntoa(dst_ip)"])) or ($sensor_allowed)) {
                $list[] = new Alarm($rs->fields["event_id"], $rs->fields["backlog_id"], $rs->fields["timestamp"], $rs->fields["status"], Alarm::__get_since($conn, $rs->fields["event_id"], $rs->fields["backlog_id"]) , $rs->fields["plugin_id"], $rs->fields["plugin_sid"], $rs->fields["protocol"], $rs->fields["inet_ntoa(src_ip)"], $rs->fields["inet_ntoa(dst_ip)"], $rs->fields["src_port"], $rs->fields["dst_port"], $rs->fields["risk"], $sensors, $rs->fields["snort_sid"], $rs->fields["snort_cid"], "", 1);
            }
            $rs->MoveNext();
        }
        return $list;
    }
    function get_list3($conn, $src_ip, $dst_ip, $hide_closed = 1, $order, $inf = null, $sup = null, $from_date = null, $to_date = null, $query = "") {
        $list = array();
        $where = array();
        // Pre-Selected Ports (Granada 27 de mayo 2009)
        $ports = array();
        if (!$rsp = & $conn->Execute("SELECT * FROM port")) {
            print $conn->ErrorMsg();
            return array();
        }
        while (!$rsp->EOF) {
            $ports[$rsp->fields["protocol_name"]][$rsp->fields["port_number"]] = $rsp->fields["service"];
            $rsp->MoveNext();
        }
        /* ip filter */
        if (valid_ip($src_ip)) $src_ip.= "/32";
        if (valid_ip($dst_ip)) $dst_ip.= "/32";
        if (isset($src_ip)) list($source_start, $source_end) = Util::cidr_conv($src_ip);
        if (isset($dst_ip)) list($dest_start, $dest_end) = Util::cidr_conv($dst_ip);
        /* If both src and dst are equal, try to find all alarms where src OR dst are equal to the IP */
        if ($src_ip && ($src_ip == $dst_ip)) {
            $where[] = "(inet_aton('$source_start') <= a.src_ip AND inet_aton('$source_end') >= a.src_ip)
                        OR
                        (inet_aton('$dest_start') <= a.dst_ip AND inet_aton('$dest_end') >= a.dst_ip)";
        } elseif ($src_ip) {
            $where[] = "(inet_aton('$source_start') <= a.src_ip AND inet_aton('$source_end') >= a.src_ip)";
        } elseif ($dst_ip) {
            $where[] = "(inet_aton('$dest_start') <= a.dst_ip AND inet_aton('$dest_end') >= a.dst_ip)";
        }
        /* hide closed */
        if ($hide_closed == 1) {
            $where[] = "a.status = 'open' ";
        }
        /* date range filter */
        if ($from_date) {
            list($y, $m, $d) = preg_split('#-#', $from_date);
            if (!checkdate($m, $d, $y)) {
                print _("Invalid date") . ": $from_date";
                return array();
            }
            $where[] = "(a.timestamp >= '$from_date')";
        }
        if ($to_date) {
            list($y, $m, $d) = preg_split('#-#', $to_date);
            if (!checkdate($m, $d, $y)) {
                print _("Invalid date") . ": $to_date";
                return array();
            }
            $where[] = "(a.timestamp <= '$to_date 23:59:59')";
        }
        /* number of events per page */
        $limit = "";
        if ($sup !== null && $inf !== null) {
            $nevents = $sup - $inf;
            $limit = "LIMIT $nevents OFFSET $inf";
        }
        if ($query != "") {
            $where[] = "(p.name LIKE \"%$query%\")";
        }
        $sql_where = count($where) ? 'WHERE a.plugin_id=p.plugin_id AND a.plugin_sid=p.sid AND ' . implode(" AND ", $where) : "WHERE a.plugin_id=p.plugin_id AND a.plugin_sid=p.sid";
        // ... ", a.timestamp + 0 AS timestamp" what is this?
        $sql = "SELECT SQL_CALC_FOUND_ROWS *, p.name as sid_name, 
                  inet_ntoa(a.src_ip), inet_ntoa(a.dst_ip)
                FROM alarm a,plugin_sid p $sql_where $order $limit";
        //echo "SQL1: ".$sql."<br>";
        //exit;
        if (!$rs = & $conn->Execute($sql)) {
            print $conn->ErrorMsg();
            return array();
        }
        // Total rows
        $total = & $conn->Execute("SELECT FOUND_ROWS() as contador");
        $num_rows = 0;
        if (!$total->EOF) $num_rows = $total->fields['contador'];
        while (!$rs->EOF) {
            $sensors = Alarm::__get_sensors($conn, $rs->fields["event_id"], $rs->fields["backlog_id"]);
            $sensor_allowed = 0;
            foreach($sensors as $sensor) {
                if (Session::hostAllowed($conn, $sensor)) $sensor_allowed = 1;
            }
            if ((Session::hostAllowed($conn, $rs->fields["inet_ntoa(src_ip)"])) or (Session::hostAllowed($conn, $rs->fields["inet_ntoa(dst_ip)"])) or ($sensor_allowed)) {
                $rs->fields["src_port"] = ($ports[$rs->fields["protocol"]][$rs->fields["src_port"]] != "") ? $ports[$rs->fields["protocol"]][$rs->fields["src_port"]] : $rs->fields["src_port"];
                $rs->fields["dst_port"] = ($ports[$rs->fields["protocol"]][$rs->fields["dst_port"]] != "") ? $ports[$rs->fields["protocol"]][$rs->fields["dst_port"]] : $rs->fields["dst_port"];
                $list[] = new Alarm($rs->fields["event_id"], $rs->fields["backlog_id"], $rs->fields["timestamp"], $rs->fields["status"], Alarm::__get_since($conn, $rs->fields["event_id"], $rs->fields["backlog_id"]) , $rs->fields["plugin_id"], $rs->fields["plugin_sid"], $rs->fields["protocol"], $rs->fields["inet_ntoa(a.src_ip)"], $rs->fields["inet_ntoa(a.dst_ip)"], $rs->fields["src_port"], $rs->fields["dst_port"], $rs->fields["risk"], $sensors, $rs->fields["snort_sid"], $rs->fields["snort_cid"], $rs->fields["sid_name"], "", 1);
            }
            $rs->MoveNext();
        }
        return array(
            $list,
            $num_rows
        );
    }
    // Get max value of field ex.(max timestamp, max risk value, etc. <<Used in status bar>>)
    function get_max_byfield($conn, $field) {
        $query = OssimQuery("SELECT backlog_id,$field FROM alarm ORDER BY $field DESC LIMIT 1");
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            if ($rw = $rs->fetchRow()) return array(
                $rw[$field],
                $rw['backlog_id']
            );
            else print $conn->ErrorMsg();
        }
    }
    function get_list2($conn, $src_ip, $dst_ip, $order, $inf = 0, $sup = 25) {
        $i = 0;
        /* get main events id */
        $query = OssimQuery("SELECT backlog_id, event_id FROM alarm $order");
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $backlog_ids[] = $rs->fields["backlog_id"];
                $events[] = $rs->fields["event_id"];
                $rs->MoveNext();
            }
        }
        if (!$backlog_ids) {
            return array();
        }
        /* get events id with minimal rule_level */
        foreach($backlog_ids as $backlog_id) {
            $sql = OssimQuery("SELECT min(event.id) AS event_id,
                    event.timestamp AS timestamp,
                    event.sensor AS sensor
                FROM event LEFT JOIN backlog_event
                ON backlog_event.backlog_id = $backlog_id
                    AND backlog_event.event_id = event.id 
                GROUP BY backlog_event.backlog_id");
            if (!$rs = & $conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
                if ($rs->EOF) {
                    $since[] = "";
                }
                while (!$rs->EOF) {
                    $min_events[] = $rs->fields["event_id"];
                    $since[] = $rs->fields["timestamp"];
                    $sensor[] = $rs->fields["sensor"];
                    $rs->MoveNext();
                }
            }
        }
        foreach($events as $event_id) {
            $array_backlog_id = $array_since = $array_sensor = NULL;
            if (is_array($backlog_ids)) $array_backlog_id = array_shift($backlog_ids);
            if (is_array($since)) $array_since = array_shift($since);
            if (is_array($sensor)) $array_sensor = array_shift($sensor);
            $query = OssimQuery("SELECT *, inet_ntoa(src_ip), inet_ntoa(dst_ip) 
                FROM alarm WHERE event_id = $event_id 
                    AND backlog_id = $array_backlog_id $order");
            if (!$rs = & $conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
                //                while (!$rs->EOF) {
                if (($i++ >= $inf) && ($inf < $sup)) {
                    /* check for src & dest ips */
                    $can_insert = 0;
                    $db_src = $rs->fields["inet_ntoa(src_ip)"];
                    $db_dst = $rs->fields["inet_ntoa(dst_ip)"];
                    if ($src_ip && $dst_ip) {
                        if ((!strcmp($src_ip, $db_src)) OR (!strcmp($dst_ip, $db_dst))) {
                            $can_insert = 1;
                        }
                    } elseif ($src_ip) {
                        if (!strcmp($src_ip, $db_src)) {
                            $can_insert = 1;
                        }
                    } elseif ($dst_ip) {
                        if (!strcmp($dst_ip, $db_dst)) {
                            $can_insert = 1;
                        }
                    } else {
                        $can_insert = 1;
                    }
                    if (($can_insert) and
                    /* net filter */
                    ((Session::hostAllowed($conn, $db_src)) or (Session::hostAllowed($conn, $db_dst)))) {
                        $inf++;
                        $maxrisk = ($rs->fields["risk_c"] > $rs->fields["risk_a"]) ? $rs->fields["risk_c"] : $rs->fields["risk_a"];
                        $list[] = new Alarm($event_id, $array_backlog_id, $rs->fields["timestamp"], $array_since, $rs->fields["plugin_id"], $rs->fields["plugin_sid"], $rs->fields["protocol"], $rs->fields["inet_ntoa(src_ip)"], $rs->fields["inet_ntoa(dst_ip)"], $rs->fields["src_port"], $rs->fields["dst_port"], $maxrisk, $array_sensor, $rs->fields["snort_sid"], $rs->fields["snort_cid"], "", 1);
                    }
                }
                //                    $rs->MoveNext();
                //                }
                
            }
        }
        return $list;
    }
    /*
    * PRE:
    *   $show_all = 0  =>  show only alarms
    *   $show_all = 1  =>  show all events
    *
    *   $event_id => alarm to expand
    */
    function get_events($conn, $backlog_id, $show_all = 0, $event_id = "") {
        $expand = false;
        $query = "SELECT backlog_event.event_id AS event_id, 
                         backlog_event.backlog_id AS backlog_id, 
                         event.timestamp AS backlog_time,
                         event.timestamp AS timestamp,
                         event.plugin_id AS plugin_id,
                         event.plugin_sid AS plugin_sid,
                         event.protocol AS protocol,
                         inet_ntoa(event.src_ip) AS src_ip, 
                         inet_ntoa(event.dst_ip) AS dst_ip, 
                         event.src_port AS src_port,
                         event.dst_port AS dst_port,
                         event.risk_c AS risk_c,
                         event.risk_a AS risk_a,
                         event.sensor AS sensor,
                         event.snort_sid AS snort_sid,
                         event.snort_cid AS snort_cid,
                         event.alarm AS alarm,
                         backlog_event.rule_level AS rule_level
            FROM backlog_event, event
            WHERE backlog_event.event_id = event.id AND
                backlog_event.backlog_id = $backlog_id ";
        if (!$show_all) {
            $query.= " AND event.alarm = 1 ";
        }
        $query.= "ORDER BY event.id DESC";
        $query = OssimQuery($query);
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                if ((!$expand) and ($rs->fields["event_id"] == $event_id)) {
                    $expand = true;
                } elseif (($expand) and ($rs->fields["alarm"] == 1)) {
                    $expand = false;
                }
                if (($expand) or ($rs->fields["alarm"] == 1)) {
                    $maxrisk = ($rs->fields["risk_c"] > $rs->fields["risk_a"]) ? $rs->fields["risk_c"] : $rs->fields["risk_a"];
                    $list[] = new Alarm($rs->fields["event_id"], $rs->fields["backlog_id"], $rs->fields["timestamp"], "", /* status */
                    $rs->fields["backlog_time"], $rs->fields["plugin_id"], $rs->fields["plugin_sid"], $rs->fields["protocol"], $rs->fields["src_ip"], $rs->fields["dst_ip"], $rs->fields["src_port"], $rs->fields["dst_port"], $maxrisk, $rs->fields["sensor"], $rs->fields["snort_sid"], $rs->fields["snort_cid"], $rs->fields["rule_level"], $rs->fields["alarm"]);
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }
    function get_alarm_stats($conn, $backlog_id, $event_id) {
        $expand = false;
        $count = 0;
        $total_count = 0;
        $data = array();
        $dst_ips = array();
        $types = array();
        $dst_ports = array();
        $sql = "SELECT event.id AS event_id, 
                event.alarm AS alarm,
                event.dst_ip AS dst_ip,
                event.plugin_id AS plugin_id,
                event.plugin_sid AS plugin_sid,
                event.dst_port AS dst_port
                FROM backlog_event, event 
                WHERE backlog_event.backlog_id = ? 
                AND event.id = backlog_event.event_id 
                ORDER BY event.id DESC";
        $params = array(
            $backlog_id
        );
        //echo $sql."<br>";
        if (!$rs = & $conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if (($expand) and ($rs->fields["alarm"] == 1)) {
                    $expand = false;
                }
                if ($expand) {
                    array_push($dst_ips, $rs->fields["dst_ip"]);
                    array_push($types, $rs->fields["plugin_id"] . "-" . $rs->fields["plugin_sid"]);
                    array_push($dst_ports, $rs->fields["dst_port"]);
                    $count++;
                }
                if ((!$expand) and ($rs->fields["event_id"] == $event_id)) {
                    $expand = true;
                }
                $total_count++;
                $rs->MoveNext();
            }
        }
        $data["count"] = $count;
        $data["total_count"] = $total_count;
        $uniq_dst_ips = array_unique($dst_ips);
        $data["dst_ips"] = count($uniq_dst_ips);
        $uniq_types = array_unique($types);
        $data["types"] = count($uniq_types);
        $uniq_dst_ports = array_unique($dst_ports);
        $data["dst_ports"] = count($uniq_dst_ports);
        return $data;
    }
    function delete_day($conn, $day) {
        $sql = "SELECT backlog_id, event_id FROM alarm WHERE to_days(timestamp) = to_days(?)";
        $params = array(
            $day
        );
        if (!$rs = & $conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if ($rs->fields["backlog_id"] == 0) {
                    Alarm::delete($conn, $rs->fields["event_id"]);
                } else {
                    Alarm::delete_from_backlog($conn, $rs->fields["backlog_id"], $rs->fields["event_id"]);
                }
                $rs->MoveNext();
            }
        }
        $infolog = array(
            $day
        );
        Log_action::log(13, $infolog);
    }
    function close($conn, $event_id) {
        $sql = "UPDATE alarm SET status = 'closed', timestamp = timestamp WHERE event_id = ?";
        $params = array(
            $event_id
        );
        if ($conn->Execute($sql, $params) === false) {
            print 'error closing alarm: ' . $conn->ErrorMsg() . '<BR>';
            exit;
        }
        $infolog = array(
            $event_id
        );
        Log_action::log(12, $infolog);
    }
    function open($conn, $event_id) {
        $sql = "UPDATE alarm SET status = 'open', timestamp = timestamp WHERE event_id = ?";
        $params = array(
            $event_id
        );
        if ($conn->Execute($sql, $params) === false) {
            print 'error opening alarm: ' . $conn->ErrorMsg() . '<BR>';
            exit;
        }
        $infolog = array(
            $event_id
        );
        Log_action::log(12, $infolog);
    }
    function delete($conn, $event_id) {
        $sql = "DELETE FROM alarm WHERE event_id = ?";
        $sql2 = "DELETE FROM event WHERE id = ?";
        $params = array(
            $event_id
        );
        if (($conn->Execute($sql, $params) === false) or ($conn->Execute($sql2, $params) === false)) {
            print 'error deleting: ' . $conn->ErrorMsg() . '<BR>';
            exit;
        }
        $infolog = array(
            $event_id
        );
        Log_action::log(11, $infolog);
    }
    function delete_from_backlog($conn, $backlog_id, $alarm_id, $hide_closed = 0, $src_ip = null, $dst_ip = null, $from_date = null, $to_date = null) {
        /* delete ALL */
        if (!strcmp($backlog_id, "all")) {
            if (($conn->Execute("DELETE FROM alarm") === false) or ($conn->Execute("DELETE FROM event") === false) or ($conn->Execute("DELETE FROM backlog") === false) or ($conn->Execute("DELETE FROM backlog_event") === false)) {
                print 'error deleting: ' . $conn->ErrorMsg() . '<BR>';
            }
            // XXXX Work in progress, non functional
            
        } elseif ($backlog_id == 'filter') {
            $where_ip = array();
            $where_alarm_status = array();
            $where_timestamp = array();
            /* ip filter */
            if ($src_ip and $dst_ip) {
                $where_ip[] = "(inet_aton('$src_ip') = src_ip OR
                             inet_aton('$dst_ip') = dst_ip) ";
            } elseif ($src_ip) {
                $where_ip[] = "(inet_aton('$src_ip') = src_ip) ";
            } elseif ($dst_ip) {
                $where_ip[] = "(inet_aton('$dst_ip') = dst_ip) ";
            }
            /* hide closed */
            if ($hide_closed == 1) {
                $where_alarm_status[] = "alarm.status = 'open' ";
            }
            /* date range filter */
            if ($from_date) {
                list($y, $m, $d) = preg_split('#-#', $from_date);
                if (!checkdate($m, $d, $y)) {
                    print _("Invalid date") . ": $from_date";
                    return array();
                }
                $where_timestamp[] = "(timestamp >= '$from_date')";
            }
            if ($to_date) {
                list($y, $m, $d) = preg_split('#-#', $to_date);
                if (!checkdate($m, $d, $y)) {
                    print _("Invalid date") . ": $to_date";
                    return array();
                }
                $where_timestamp[] = "(timestamp <= '$to_date 23:59:59')";
            }
            $where1 = '';
            // XXX to be continued...
            
        } else {
            /* Delete all events related with $backlog_id */
            $sql = "SELECT event.id AS id FROM event, backlog_event WHERE backlog_event.backlog_id = ?
                    AND backlog_event.event_id = event.id";
            $params = array(
                $backlog_id
            );
            if (!$rs = & $conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
            } else {
                while (!$rs->EOF) {
                    Alarm::delete($conn, $rs->fields["id"]);
                    $rs->MoveNext();
                }
            }
            $sql = "DELETE FROM backlog_event WHERE backlog_id = ?";
            $params = array(
                $backlog_id
            );
            $sql2 = "DELETE FROM backlog WHERE id = ?";
            $params2 = array(
                $backlog_id
            );
            $sql3 = "DELETE FROM alarm WHERE event_id = ? AND backlog_id = ?";
            $params3 = array(
                $alarm_id,
                $backlog_id
            );
            if (($conn->Execute($sql, $params) === false) or ($conn->Execute($sql2, $params2) === false) or ($conn->Execute($sql3, $params3) === false)) {
                print 'error deleting: ' . $conn->ErrorMsg() . '<BR>';
                exit;
            }
        }
    }
    function purge($conn) {
        /* TODO: very heavy query!! */
        // Optimized 09/06/2009 Granada. From OSSIM forum user gherees (USA)
        /*$sql = "SELECT event.id AS event_id, backlog_event.backlog_id AS
        backlog_id FROM event LEFT JOIN backlog_event ON
        backlog_event.event_id = event.id WHERE backlog_id IS NULL";*/
        $sql = "SELECT event.id AS event_id FROM event WHERE event.id IN
               (SELECT backlog_event.event_id FROM backlog_event WHERE
                backlog_event.backlog_id IS NULL)";
        if (!$rs = & $conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                Alarm::delete($conn, $rs->fields["event_id"]);
                $rs->MoveNext();
            }
        }
    }
}
?>
