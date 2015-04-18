<?php

require_once ('classes/Net.inc');
require_once ('classes/Session.inc');

class Alarm {

    var $alert_id;
    var $backlog_id;
    var $timestamp;
    var $since;
    var $plugin_id;
    var $plugin_sid;
    var $protocol;
    var $src_ip;
    var $dst_ip;
    var $src_port;
    var $dst_port;
    var $risk;
    var $snort_sid;
    var $snort_cid;
    var $rule_level;
    var $alarm;

    function Alarm ($alert_id, $backlog_id, $timestamp, $since, $plugin_id, 
                    $plugin_sid, $protocol, $src_ip, $dst_ip, $src_port, 
                    $dst_port, $risk, $snort_sid, $snort_cid, 
                    $rule_level = "", $alarm = 1)
    {
        $this->alert_id     = intval($alert_id);
        $this->backlog_id   = intval($backlog_id);
        $this->timestamp    = $timestamp;
        $this->since        = $since;
        $this->plugin_id    = intval($plugin_id);
        $this->plugin_sid   = intval($plugin_sid);
        $this->protocol     = $protocol;
        $this->src_ip       = $src_ip;
        $this->dst_ip       = $dst_ip;
        $this->src_port     = intval($src_port);
        $this->dst_port     = intval($dst_port);
        $this->risk         = intval($risk);
        $this->snort_sid    = intval($snort_sid);
        $this->snort_cid    = intval($snort_cid);
        $this->rule_level   = intval($rule_level);
        $this->alarm        = $alarm;
    }

    function get_alert_id()         { return $this->alert_id;       }
    function get_backlog_id()       { return $this->backlog_id;     }
    function get_timestamp()        { return $this->timestamp;      }
    function get_since()            { return $this->since;          }
    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }
    function get_protocol()         { return $this->protocol;       }
    function get_src_ip()           { return $this->src_ip;         }
    function get_dst_ip()           { return $this->dst_ip;         }
    function get_src_port()         { return $this->src_port;       }
    function get_dst_port()         { return $this->dst_port;       }
    function get_risk()             { return $this->risk;           }
    function get_asset_src()        { return $this->asset_src;      }
    function get_asset_dst()        { return $this->asset_dst;      }
    function get_rule_level()       { return $this->rule_level;     }
    function get_snort_sid()        { return $this->snort_sid;      }
    function get_snort_cid()        { return $this->snort_cid;      }
    function get_alarm()            { return $this->alarm;          }

    function get_count($conn, $src_ip = "", $dst_ip = "")
    {
        $count = 0;
        $allowed_nets = Session::getValue();

        $query = "SELECT alert_id, src_ip, dst_ip FROM alarm ";

        if ($src_ip && $dst_ip)
            $query .= " WHERE (alarm.src_ip = inet_aton('$src_ip') OR
                             alarm.dst_ip = inet_aton('$dst_ip'))";
        elseif ($src_ip)
            $query .= " WHERE alarm.src_ip = inet_aton('$src_ip') ";
        elseif ($dst_ip)
            $query .= " WHERE alarm.dst_ip = inet_aton('$dst_ip') ";
        else
            $query .= "";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            if ((Net::isIpInNet($rs->fields["src_ip"],
                                $allowed_nets) or
                 Net::isIpInNet($rs->fields["dst_ip"],
                                $allowed_nets) or
                 (!$allowed_nets)))
            {
                $count++;
            }
                $rs->MoveNext();
            }
            return $count;
        }
    }

    function get_list($conn, $src_ip, $dst_ip, $order,
                      $inf = 0, $sup = 25)
    {
        $i = 0;

        /* get main alerts id */
        $query = "SELECT backlog_id, alert_id FROM alarm $order";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $backlog_ids[] = $rs->fields["backlog_id"];
                $alerts[] = $rs->fields["alert_id"];
                $rs->MoveNext();
            }
        }

        if (!$backlog_ids) {
            return array();
        }

        /* get alerts id with minimal rule_level */
        foreach ($backlog_ids as $backlog_id) {
            $query = "SELECT min(alert.id) AS alert_id,
                    alert.timestamp AS timestamp 
                FROM backlog_alert, alert 
                WHERE backlog_alert.alert_id = alert.id 
                    AND backlog_alert.backlog_id = $backlog_id
                GROUP BY backlog_alert.backlog_id";
            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
                if ($rs->EOF) {
                    $since[] = "";
                }
                while (!$rs->EOF) {
                    $min_alerts[] = $rs->fields["alert_id"];
                    $since[] = $rs->fields["timestamp"];
                    $rs->MoveNext();
                }
            }
        }

        foreach ($alerts as $alert_id) {

            $allowed_nets = Session::getValue();

            $array_backlog_id = array_shift($backlog_ids);
            $array_since = array_shift($since);

            $query = "SELECT *, inet_ntoa(src_ip), inet_ntoa(dst_ip) 
                FROM alarm WHERE alert_id = $alert_id 
                    AND backlog_id = $array_backlog_id $order";

            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
//                while (!$rs->EOF) {
                    if (($i++ >= $inf) && ($inf < $sup)) {

                        /* check for src & dest ips */
                        $can_insert = 0;
                        $db_src = $rs->fields["inet_ntoa(src_ip)"];
                        $db_dst = $rs->fields["inet_ntoa(dst_ip)"];
                        if ($src_ip && $dst_ip) {
                            if ( (!strcmp($src_ip, $db_src)) OR
                                 (!strcmp($dst_ip, $db_dst)) ) {
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
                            (Net::isIpInNet($db_src,
                                            $allowed_nets) or
                             Net::isIpInNet($db_dst,
                                            $allowed_nets) or
                            (!$allowed_nets)))
                        {
                            $inf++;
                            $list[] = new Alarm (
                                    $alert_id,
                                    $array_backlog_id,
                                    $rs->fields["timestamp"],
                                    $array_since,
                                    $rs->fields["plugin_id"],
                                    $rs->fields["plugin_sid"],
                                    $rs->fields["protocol"],
                                    $rs->fields["inet_ntoa(src_ip)"],
                                    $rs->fields["inet_ntoa(dst_ip)"],
                                    $rs->fields["src_port"],
                                    $rs->fields["dst_port"],
                                    $rs->fields["risk"],
                                    $rs->fields["snort_sid"],
                                    $rs->fields["snort_cid"],
                                    "",
                                    1
                                );
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
     *   $show_all = 1  =>  show all alerts
     *
     *   $alert_id => alarm to expand
     */
    function get_alerts($conn, $backlog_id, $show_all = 0, $alert_id = "")
    {
        $expand = false;
    
        $query = "SELECT backlog_alert.alert_id AS alert_id, 
                         backlog.id AS backlog_id, 
                         backlog.timestamp AS backlog_time,
                         alert.timestamp AS timestamp,
                         alert.plugin_id AS plugin_id,
                         alert.plugin_sid AS plugin_sid,
                         alert.protocol AS protocol,
                         inet_ntoa(alert.src_ip) AS src_ip, 
                         inet_ntoa(alert.dst_ip) AS dst_ip, 
                         alert.src_port AS src_port,
                         alert.dst_port AS dst_port,
                         alert.risk_a AS risk,
                         alert.snort_sid AS snort_sid,
                         alert.snort_cid AS snort_cid,
                         alert.alarm AS alarm,
                         backlog_alert.rule_level AS rule_level
            FROM backlog_alert, alert, backlog
            WHERE backlog_alert.alert_id = alert.id AND
                backlog.id = backlog_alert.backlog_id AND
                backlog.id = $backlog_id ";
                
        if (!$show_all) {
            $query .= " AND alert.alarm = 1 ";
        }
                
        $query .= "ORDER BY alert.id DESC;";
            
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                
                if ((!$expand) and ($rs->fields["alert_id"] == $alert_id)) {
                    $expand = true;
                } elseif (($expand) and ($rs->fields["alarm"] == 1)) {
                    $expand = false;
                }
               
                if (($expand) or ($rs->fields["alarm"] == 1)) {

                    $list[] = new Alarm (
                            $rs->fields["alert_id"],
                            $rs->fields["backlog_id"],
                            $rs->fields["timestamp"],
                            $rs->fields["backlog_time"],
                            $rs->fields["plugin_id"],
                            $rs->fields["plugin_sid"],
                            $rs->fields["protocol"],
                            $rs->fields["src_ip"],
                            $rs->fields["dst_ip"],
                            $rs->fields["src_port"],
                            $rs->fields["dst_port"],
                            $rs->fields["risk"],
                            $rs->fields["snort_sid"],
                            $rs->fields["snort_cid"],
                            $rs->fields["rule_level"],
                            $rs->fields["alarm"]
                        );
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function get_alarm_stats($conn, $backlog_id, $alert_id) 
    {
        $expand = false;
        $count = 0;
        
        $data = array();
        $dst_ips = array();
        $types = array();
        $dst_ports = array();

        $query = "
            SELECT alert.id AS alert_id, 
                alert.alarm AS alarm,
                alert.dst_ip AS dst_ip,
                alert.plugin_id AS plugin_id,
                alert.plugin_sid AS plugin_sid,
                alert.dst_port AS dst_port
            FROM backlog_alert, alert 
            WHERE backlog_alert.backlog_id = $backlog_id 
                AND alert.id = backlog_alert.alert_id 
            ORDER BY alert.id DESC";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                
                if (($expand) and ($rs->fields["alarm"] == 1)) {
                    $expand = false;
                }

                if ($expand) {
                    array_push($dst_ips, $rs->fields["dst_ip"]);
                    array_push($types, 
                      $rs->fields["plugin_id"]."-".$rs->fields["plugin_sid"]);
                    array_push($dst_ports, $rs->fields["dst_port"]);
                    $count++;
                }
                
                if ((!$expand) and ($rs->fields["alert_id"] == $alert_id)) {
                    $expand = true;
                }

                $rs->MoveNext();
            }
        }
        $data["count"] = $count;
        $uniq_dst_ips = array_unique($dst_ips);
        $data["dst_ips"] = count($uniq_dst_ips);
        $uniq_types = array_unique($types);
        $data["types"] = count($uniq_types);
        $uniq_dst_ports = array_unique($dst_ports);
        $data["dst_ports"] = count($uniq_dst_ports);

        return $data;
    }


    function delete_day($conn, $day)
    {
        $query = "SELECT backlog_id, alert_id FROM alarm
            WHERE to_days(timestamp) = to_days($day)";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if ($rs->fields["backlog_id"] == 0) {
                    Alarm::delete($conn, $rs->fields["alert_id"]);
                } else {
                    Alarm::delete_from_backlog ($conn,
                                                $rs->fields["backlog_id"],
                                                $rs->fields["alert_id"]);
                }
                $rs->MoveNext();
            }
        }
    }

    function delete($conn, $alert_id)
    {
        $query = "DELETE FROM alarm WHERE alert_id = '$alert_id'";
        $query2 = "DELETE FROM alert WHERE id = '$alert_id'";

        if ( ($conn->Execute($query) === false) or
             ($conn->Execute($query2) === false) )
        {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete_from_backlog($conn, $backlog_id, $alarm_id)
    {
        /* delete ALL */
        if (!strcmp($backlog_id, "all")) {
            if ( ($conn->Execute("DELETE FROM alarm") === false) or
                 ($conn->Execute("DELETE FROM alert") === false) or
                 ($conn->Execute("DELETE FROM backlog") === false) or
                 ($conn->Execute("DELETE FROM backlog_alert") === false) )
            {
                print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            }


        } else {

            $query = "DELETE FROM alarm WHERE alert_id = $alarm_id 
                AND backlog_id = $backlog_id";
            if ($conn->Execute($query) === false) {
                print 'error deleting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }

            $query = "SELECT COUNT(alert_id) as count FROM backlog_alert 
                WHERE alert_id = $alarm_id";
            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg().'<BR>';
                exit;
            } 

            if ($rs->fields["count"] == 1) {
                $query = "DELETE FROM alert WHERE id = $alarm_id";
                if ($conn->Execute($query) === false) {
                    print 'error deleting: '.$conn->ErrorMsg().'<BR>';
                    exit;
                }
            }

            $query = "DELETE FROM backlog_alert WHERE backlog_id = $backlog_id";
            $query2 = "DELETE FROM backlog WHERE id = $backlog_id";
            
            if ( ($conn->Execute($query) === false) or
                 ($conn->Execute($query2) === false) )
            {
                print 'error deleting: '.$conn->ErrorMsg().'<BR>';
                exit;
            }
        }
    }

}

?>
