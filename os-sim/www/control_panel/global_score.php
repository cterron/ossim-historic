<?php
/*
TODO
- missing sensors stuff (see Session::hostAllowed() & Host::get_realted_sensors())
- now everybody can see hosts outside defined networks, maybe add a new user perm
- max metric date could be shown in days/hours/mins (see Util::date_diff())
- current global score es simplemente la suma de los scores de host_qualification
        sin ningun chequeo extra (ver get_score())
- add help
*/

require_once 'classes/Session.inc';
require_once 'classes/Util.inc';
require_once 'classes/Net.inc';
Session::logcheck("MenuControlPanel", "ControlPanelMetrics");

$db = new ossim_db();
$conn = $db->connect();

////////////////////////////////////////////////////////////////
// Param validation
////////////////////////////////////////////////////////////////

$valid_range = array('day', 'week', 'month', 'year');
$range = GET('range');
if (!$range) {
    $range = 'day';
} elseif (!in_array($range, $valid_range)) {
    die(ossim_error('Invalid range'));
}

if ($range == 'day') {
    $rrd_start = "N-1D";
} elseif ($range == 'week') {
    $rrd_start = "N-7D";
} elseif ($range == 'month') {
    $rrd_start = "N-1M";
} elseif ($range == 'year') {
    $rrd_start = "N-1Y";
}

$user = Session::get_session_user();
$conf = $GLOBALS['CONF'];
$conf_threshold = $conf->get_conf('threshold');

////////////////////////////////////////////////////////////////
// Script private functions
////////////////////////////////////////////////////////////////

/*
 * @param $name, string with the id of the object (ex: a network name or a host
 * ip)
 * @param $type, enum ('day', 'month', ...)
 */
function get_score($name, $type)
{
    global $conn, $range;
    static $scores = null;
    
    // first time build the scores cache    
    if (!$scores) {
        $sql = "SELECT id, rrd_type, max_c, max_a, max_c_date, max_a_date
                FROM control_panel WHERE time_range = ?";
        $params = array($range);
        if (!$rs = &$conn->Execute($sql, $params)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            // $id = 'netfoo.com#net'
            $id = $rs->fields['id'].'#'.$rs->fields['rrd_type'];
            $scores[$id] = array(
                          'max_c'      => $rs->fields['max_c'],
                          'max_a'      => $rs->fields['max_a'],
                          'max_c_date' => $rs->fields['max_c_date'],
                          'max_a_date' => $rs->fields['max_a_date']
                          );
           $rs->MoveNext();
        }
    }
    $id = $name.'#'.$type;
    if (isset($scores[$id])) {
        return $scores[$id];
    }
    return array('max_c' => 0, 'max_a' => 0, 'max_c_date' => 0, 'max_a_date' => 0);
}

function get_current_metric($name, $type='host', $ac='attack')
{
    static $qualification;
    global $conn;
    if (!$qualification) {
        $sql = "SELECT host_ip, compromise, attack FROM host_qualification";
        if (!$rs = &$conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        $qualification['global']['global']['attack'] = 0;
        $qualification['global']['global']['compromise'] = 0;
        while (!$rs->EOF) {
            $host = $rs->fields['host_ip'];
            $qualification['host'][$host]['attack'] = $rs->fields['attack'];
            $qualification['global']['global']['attack'] += $rs->fields['attack'];
            $qualification['host'][$host]['compromise'] = $rs->fields['compromise'];
            $qualification['global']['global']['compromise'] += $rs->fields['compromise'];
            $rs->MoveNext();
        }
        $sql = "SELECT net_name, compromise, attack FROM net_qualification";
        if (!$rs = &$conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            $host = $rs->fields['net_name'];
            $qualification['net'][$host]['attack'] = $rs->fields['attack'];
            $qualification['net'][$host]['compromise'] = $rs->fields['compromise'];
            $rs->MoveNext();
        }
    }
    if (isset($qualification[$type][$name][$ac])) {
        return $qualification[$type][$name][$ac];
    }
    // no current metric for this network object
    return 0;
}

/*
 * 
 * @param string $ip,  the host ip
 * @return mixed     - array: with full network data
 *                   - false: user have no perms over the network
 *                   - null: host is not in any defined network
 */
function host_get_network_data($ip)
{
    global $groups, $networks;
    // search in groups
    foreach ($groups as $group_name => $g_data) {
        
        foreach ($g_data['nets'] as $net_name => $n_data) {
            $address = $n_data['address'];
            if (!strpos($address, "/")) {
                // tvvcox: i've detected some wrong network addresses, catch them with that
                //echo "<font color='red'>"._("Invalid network address for")." $net_name: $address</font><br>";
                continue;
            }
            if (Net::isIpInNet($ip, $address)) {
                if (!$n_data['has_perms'] && !check_sensor_perms($ip, 'host')) {
                    return false;
                }
                $n_data['group'] = $group_name;
                $n_data['name'] = $net_name;
                return $n_data;
            }
        }
    }
    // search in nets
    foreach ($networks as $net_name => $n_data) {
        $address = $n_data['address'];
        if (Net::isIpInNet($ip, $address)) {
            if (!$n_data['has_perms'] && !check_sensor_perms($ip, 'host')) {
                return false;
            }
            $n_data['group'] = false;
            $n_data['name'] = $net_name;
            return $n_data;
        }
    }
    // This means the host didn't belong to any net
    //echo "$ip not in any network<br>";
    return null;
}

/*
 * A user has perms over a:
 * 
 * a) host: If an allowed sensor has the same ip as $subject or if the user has
 * an allowed sensor related to this host (host_sensor_reference)
 * 
 * b) net: if the user has an allowed sensor related to this net
 * (net_sensor_reference)
 */
function check_sensor_perms($subject, $type='host')
{
    global $conn, $allowed_sensors, $groups, $networks;
    static $host_sensors = false, $sensors_ip = array(), $net_sensors = false;
    
    // if $allowed_sensors is empty, that means permit all
    if (!$allowed_sensors) {
        return true;
    }
    if ($type == 'host') {
        // First time build the static arrays
        if (!$host_sensors) {
            // Get the IP of each allowed sensor
            $sql = "SELECT sensor.ip FROM sensor WHERE ";
            $sqls = array();
            foreach ($allowed_sensors as $s) {
                $sqls[] = "sensor.name = '$s'";
            }
            $sql .= implode(' OR ', $sqls);
            if (!$rs = $conn->Execute($sql)) {
                die($conn->ErrorMsg());
            }
            while (!$rs->EOF) {
                $sensors_ip[] = $rs->fields['ip'];
                $rs->MoveNext();
            }
            // Get the sensors related to the IP
            $sql = "SELECT host_ip, sensor_name FROM host_sensor_reference";    
            if (!$rs = $conn->Execute($sql)) {
                die($conn->ErrorMsg());
            }
            while (!$rs->EOF) {
                $sensor_name = $rs->fields['sensor_name'];
                if (in_array($sensor_name, $allowed_sensors)) {
                    $host_sensors[$rs->fields['host_ip']][] = $sensor_name;
                }
                $rs->MoveNext();
            }
        }
        // if the ip has related sensors and one of each related sensor
        // is listed as allowed then permit
        if (isset($host_sensors[$subject])) {
            return count(array_intersect($host_sensors[$subject], $allowed_sensors));
        }
        // if the ip matches the ip of one allowed sensor: permit
        return in_array($subject, $sensors_ip);
    }
    if ($type == 'net') {
        // First time build the static array
        if (!$net_sensors) {
            // Get the sensors related to the net
            $sql = "SELECT net_name, sensor_name FROM net_sensor_reference";    
            if (!$rs = $conn->Execute($sql)) {
                die($conn->ErrorMsg());
            }
            while (!$rs->EOF) {
                $sensor_name = $rs->fields['sensor_name'];
                if (in_array($sensor_name, $allowed_sensors)) {
                    $net_sensors[$rs->fields['net_name']][] = $sensor_name;
                }
                $rs->MoveNext();
            }
        }
        // if the net has related sensors and one of each related sensor
        // is listed as allowed then permit
        if (isset($net_sensors[$subject])) {
            return count(array_intersect($net_sensors[$subject], $allowed_sensors));
        }
    }
    return false;
}


function check_net_perms($net_name)
{
    global $allowed_nets;
    if (is_array($allowed_nets) &&
        !in_array($net_name, $allowed_nets))
    {
        return false;
    }
    return true;
}

function order_by_risk($a, $b)
{
    global $order_by_risk_type;
    $max       = $order_by_risk_type == 'attack' ? 'max_a' : 'max_c';
    $threshold = $order_by_risk_type == 'attack' ? 'threshold_a' : 'threshold_c';
    $val_a = round($a[$max] / $a[$threshold]);
    $val_b = round($b[$max] / $b[$threshold]);
    if ($val_a == $val_b) {
        // same risk, so order alphabetically
        return strnatcmp($a['name'], $b['name']);
        // same risk order by max (like previous version)
        /*
        if ($a[$max] != $b[$max]) {
            return $a[$max] > $b[$max] ? -1 : 1;
        }
        return 0;
        */
    }
    return ($val_a > $val_b) ? -1 : 1;
}

function html_service_level()
{
    global $conn, $conf, $user, $range, $rrd_start;
    $sql = "SELECT c_sec_level, a_sec_level FROM control_panel WHERE id = ? AND time_range = ?";
    $params = array("global_$user", $range);
    if (!$rs = &$conn->Execute($sql, $params)) {
        die($conn->ErrorMsg());
    }
    if ($rs->EOF) {
        return "<td>"._("n/a")."<td>";
    }
    $level = ($rs->fields["c_sec_level"] + $rs->fields["a_sec_level"]) / 2;
    $level = sprintf("%.2f", $level);
    
    $link = Util::graph_image_link("level_$user", "level", "attack",
                                   $rrd_start, "N", 1, $range);
    
    $use_svg = $conf->get_conf("use_svg_graphics");
    if ($use_svg) {
        return "
            <td><a href='$link'>
                 <embed src='svg_level.php?sl=$sec_level&scale=0.8'  
                        pluginspage='http://www.adobe.com/svg/viewer/install/'
                        type='image/svg+xml' height='85' width='100' /> 
            </a></td>";
    } else {
        if ($level >= 95) {
            $bgcolor = "green";
            $fontcolor = "white";
        } elseif ($level >= 90) {
            $bgcolor = "#CCFF00";
            $fontcolor = "black";
        } elseif ($level >= 85) {
            $bgcolor = "#FFFF00";
            $fontcolor = "black";
        } elseif ($level >= 80) {
            $bgcolor = "orange";
            $fontcolor = "black";
        } elseif ($level >= 75) {
            $bgcolor = "#FF3300";
            $fontcolor = "white";
        } else {
            $bgcolor = "red";
            $fontcolor = "white";
        }
        return "
          <td bgcolor='$bgcolor'><b>
            <a href='$link'>
              <font size='+1' color='$fontcolor'>$level%</font>
            </a>
          </b></td>";
    }
}

function html_set_values($subject,
                         $subject_type,
                         $max,
                         $max_date,
                         $current,
                         $threshold,
                         $ac)
{
    $GLOBALS['_subject'] = $subject;
    $GLOBALS['_subject_type'] = $subject_type;
    $GLOBALS['_max'] = $max;
    $GLOBALS['_max_date'] = $max_date;
    $GLOBALS['_current'] = $current;
    $GLOBALS['_threshold'] = $threshold;
    $GLOBALS['_ac'] = $ac;
}
     
function _html_metric($metric, $threshold, $link)
{
    $risk = round($metric/$threshold*100);
    $font_color = 'color="white"';
    $color = '';
    if ($risk > 500) {
        $color = 'bgcolor="#FF0000"';
        $risk = 'high';
    } elseif ($risk > 300) {
        $color = 'bgcolor="orange"';
        $risk = 'med';
    } elseif ($risk > 100) {
        $color = 'bgcolor="green"';
        $risk = 'low';
    } else {
        $font_color = 'color="black"';
        $risk = '-';
    }
    $html = "<td $color><span title='$metric / $threshold ("._("metric/threshold").")'>".
            "<a href='$link'><font $font_color>$risk</font></a></span></td>"; 
    return $html; 
}

function _html_rrd_link()
{
    global $user, $range, $rrd_start;
    $type = $GLOBALS['_ac'] == 'c' ? 'compromise' : 'attack';
    $link = Util::graph_image_link($GLOBALS['_subject'],
                                   $GLOBALS['_subject_type'],
                                   $type,
                                   $rrd_start, "N", 1, $range);
    return $link;
}

function html_max()
{
    if ($GLOBALS['_max_date'] == 0) {
        $link = '#';
    } else {
        $link = Util::get_acid_date_link($GLOBALS['_max_date']);
    }
    return _html_metric($GLOBALS['_max'], $GLOBALS['_threshold'], $link); 
}

function html_current()
{
    $link = _html_rrd_link();
    return _html_metric($GLOBALS['_current'], $GLOBALS['_threshold'], $link);
}

function html_rrd()
{
    return '<a href="'._html_rrd_link().'"><img 
            src="../pixmaps/graph.gif" border="0"/></a>';
}

function html_report()
{
    $subject   = $GLOBALS['_subject'];
    $subject_type = $GLOBALS['_subject_type'];
    $metric    = $GLOBALS['_max'];
    $threshold = $GLOBALS['_threshold'];
    $ac        = $GLOBALS['_ac'];
    
    $title = sprintf(_("Metric Threshold: %s level exceeded"), strtoupper($ac));
    $target = "$subject_type: $subject";
    $type = $ac == 'c' ? 'Compromise' : 'Attack';
    $priority = round($metric/$threshold);
    if ($priority > 10) {
        $priority = 10;
    }
    $html = 
        "<a href='../incidents/newincident.php?" .
        "ref=Metric&" .
        "title=".urlencode("$title ($target)")."&".
        "priority=$priority&" .
        "target=".urlencode($target)."&" .
        "metric_type=$type&" .
        "metric_value=$metric'>".
        '<img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>'.
        '</a>';
    return $html;
}

function html_date()
{
    // max_date == 0, when there was no metric
    if ($GLOBALS['_max_date'] == 0 || strtotime($GLOBALS['_max_date']) == 0) {
        return _('n/a');
    }
    return $GLOBALS['_max_date'];
}

////////////////////////////////////////////////////////////////
// Network Groups
////////////////////////////////////////////////////////////////

// If allowed_nets === null, then permit all
$allowed_nets = Session::allowedNets($user);
if ($allowed_nets) {
    $allowed_nets = explode(',', $allowed_nets);
}
$allowed_sensors = Session::allowedSensors($user);
if ($allowed_sensors) {
    $allowed_sensors = explode(',', $allowed_sensors);
}

// We can't join the control_panel table, because new ossim installations
// holds no data there
$sql = "SELECT
            net_group.name as group_name,
            net_group.threshold_c as group_threshold_c,
            net_group.threshold_a as group_threshold_a,
            net.name as net_name,
            net.threshold_c as net_threshold_c,
            net.threshold_a as net_threshold_a,
            net.ips as net_address
        FROM
            net_group,
            net,
            net_group_reference
        WHERE
            net_group_reference.net_name = net.name AND
            net_group_reference.net_group_name = net_group.name";
if (!$rs = &$conn->Execute($sql)) {
    die($conn->ErrorMsg());
}
$groups = array();
$group_max_c = $group_max_a = 0;

while (!$rs->EOF) {
    $group = $rs->fields['group_name'];
    $groups[$group]['name'] = $group;
    
    // check perms over the network
    $has_net_perms = check_net_perms($rs->fields['net_address']);
    // if no perms over the network, try perms over the related sensor
    $has_perms = $has_net_perms ? true : check_sensor_perms($rs->fields['net_address'], 'net');
    
    // the user only have perms over this group if he has perms over
    // all the networks of this group
    if (!isset($groups[$group]['has_perms'])) {
        $groups[$group]['has_perms'] = $has_perms;
    } elseif (!$has_perms) {
        $groups[$group]['has_perms'] = false;
    }
    
    // If there is no threshold specified for a group, pick the configured default threshold
    $group_threshold_a = $rs->fields['group_threshold_a'] ? $rs->fields['group_threshold_a'] : $conf_threshold;
    $group_threshold_c = $rs->fields['group_threshold_c'] ? $rs->fields['group_threshold_c'] : $conf_threshold;
    $groups[$group]['threshold_a'] = $group_threshold_a;
    $groups[$group]['threshold_c'] = $group_threshold_c;
    
    $net = $rs->fields['net_name'];
    // current metrics
    $net_current_a = get_current_metric($net, 'net', 'attack');
    $net_current_c = get_current_metric($net, 'net', 'compromise');
    @$groups[$group]['current_a'] += $net_current_a;
    @$groups[$group]['current_c'] += $net_current_c;
    
    // scores
    $score = get_score($net, 'net');
    @$groups[$group]['max_c'] += $score['max_c'];
    @$groups[$group]['max_a'] += $score['max_a'];
    $net_max_c_time = strtotime($score['max_c_date']);
    $net_max_a_time = strtotime($score['max_a_date']);
        
    if (!isset($groups[$group]['max_c_date'])) {
        $groups[$group]['max_c_date'] = $score['max_c_date'];
    } else {
        $group_max_c_time = strtotime($groups[$group]['max_c_date']);
        if ($net_max_c_time > $group_max_c_time) {
            $groups[$group]['max_c_date'] = $score['max_c_date'];
        }
    }
    if (!isset($groups[$group]['max_a_date'])) {
        $groups[$group]['max_a_date'] = $score['max_a_date'];
    } else {
        $group_max_a_time = strtotime($groups[$group]['max_a_date']);
        if ($net_max_c_time > $group_max_c_time) {
            $groups[$group]['max_a_date'] = $score['max_a_date'];
        }
    }

    // If there is no threshold specified for a network, pick the group threshold
    $net_threshold_a = $rs->fields['net_threshold_a'] ? $rs->fields['net_threshold_a'] : $group_threshold_a;
    $net_threshold_c = $rs->fields['net_threshold_c'] ? $rs->fields['net_threshold_c'] : $group_threshold_c;
    
    $groups[$group]['nets'][$net] = array(
                                    'name'        => $net,
                                    'threshold_a' => $net_threshold_a,    
                                    'threshold_c' => $net_threshold_c,
                                    'max_a'       => $score['max_a'],
                                    'max_c'       => $score['max_c'],
                                    'max_a_date'  => $score['max_a_date'],
                                    'max_c_date'  => $score['max_c_date'],
                                    'address'     => $rs->fields['net_address'],
                                    'current_a'   => $net_current_a,
                                    'current_c'   => $net_current_c,
                                    'has_perms'   => $has_perms
                                    );
    $rs->MoveNext();
}

////////////////////////////////////////////////////////////////
// Networks outside groups
////////////////////////////////////////////////////////////////
$sql = "SELECT
            net.name as net_name,
            net.threshold_c as net_threshold_c,
            net.threshold_a as net_threshold_a,
            net.ips as net_address
        FROM
            net
        WHERE
            net.name NOT IN (SELECT net_name FROM net_group_reference)";

if (!$rs = &$conn->Execute($sql)) {
    die($conn->ErrorMsg());
}
$networks = array();
while (!$rs->EOF) {
    
    // check perms over the network
    $has_net_perms = check_net_perms($rs->fields['net_address']);
    // if no perms over the network, try perms over the related sensor
    $has_perms = $has_net_perms ? true : check_sensor_perms($rs->fields['net_address'], 'net');

    $net = $rs->fields['net_name'];
    $score = get_score($net, 'net');

    // If there is no threshold specified for the network, pick the global configured threshold
    $net_threshold_a = $rs->fields['net_threshold_a'] ? $rs->fields['net_threshold_a'] : $conf_threshold;
    $net_threshold_c = $rs->fields['net_threshold_c'] ? $rs->fields['net_threshold_c'] : $conf_threshold;

    $networks[$net] = array(
                        'name'        => $net,
                        'threshold_a' => $net_threshold_a,    
                        'threshold_c' => $net_threshold_c,
                        'max_a'       => $score['max_a'],
                        'max_c'       => $score['max_c'],
                        'max_a_date'  => $score['max_a_date'],
                        'max_c_date'  => $score['max_c_date'],
                        'address'     => $rs->fields['net_address'],
                        'current_a'   => get_current_metric($net, 'net', 'attack'),
                        'current_c'   => get_current_metric($net, 'net', 'compromise'),
                        'has_perms'   => $has_perms
                        );
    $rs->MoveNext();
}

////////////////////////////////////////////////////////////////
// Hosts
////////////////////////////////////////////////////////////////
$sql = "SELECT
            control_panel.id,
            control_panel.max_c,
            control_panel.max_a,
            control_panel.max_c_date,
            control_panel.max_a_date,
            host.threshold_a,
            host.threshold_c,
            host.hostname
        FROM
            control_panel
        LEFT JOIN host ON control_panel.id = host.ip
        WHERE
            control_panel.time_range = ? AND
            control_panel.rrd_type = 'host'";
$params = array($range);
if (!$rs = &$conn->Execute($sql, $params)) {
    die($conn->ErrorMsg());
}
$hosts = $ext_hosts = array();
$global_a = $global_c = 0;
while (!$rs->EOF) {
    $ip = $rs->fields['id'];
    $net = host_get_network_data($ip);

    // No perms over the host's network
    if ($net === false) {
        $rs->MoveNext(); continue;

    // Host doesn't belong to any network
    } elseif ($net === null) {
        $threshold_a = $conf_threshold;
        $threshold_c = $conf_threshold;
    
    // User got perms
    } else {
        // threshold inheritance
        $threshold_a = $rs->fields['threshold_a'] ? $rs->fields['threshold_a'] : $net['threshold_a'];
        $threshold_c = $rs->fields['threshold_c'] ? $rs->fields['threshold_c'] : $net['threshold_c'];
    }    
    // get host & global metrics
    $current_a = get_current_metric($ip, 'host', 'attack');
    $current_c = get_current_metric($ip, 'host', 'compromise');
    $global_a += $current_a;
    $global_c += $current_c;
    
    // only show hosts over their threshold
    $max_a_level     = round($rs->fields['max_a'] / $threshold_a);
    $current_a_level = round($current_a / $threshold_a);
    $max_c_level     = round($rs->fields['max_c'] / $threshold_c);
    $current_c_level = round($current_c / $threshold_c);
    
    //* comment out this if you want to see all hosts
    if ($max_a_level <= 1 && $current_a_level <= 1 &&
        $max_c_level <= 1 && $current_c_level <= 1)
    {
        $rs->MoveNext(); continue;
    }
    //*/
    $name = $rs->fields['hostname'] ? $rs->fields['hostname'] : $ip;
    if ($net === null) {
        $ext_hosts[$ip] = array(
                        'name'        => $name,
                        'threshold_a' => $threshold_a,
                        'threshold_c' => $threshold_c,
                        'max_c'       => $rs->fields['max_c'],
                        'max_a'       => $rs->fields['max_a'],
                        'max_c_date'  => $rs->fields['max_c_date'],
                        'max_a_date'  => $rs->fields['max_a_date'],
                        'current_a'   => $current_a,
                        'current_c'   => $current_c,
                    );
    } else {
        $data = array(
                'name'        => $name,
                'threshold_a' => $threshold_a,
                'threshold_c' => $threshold_c,
                'max_c'       => $rs->fields['max_c'],
                'max_a'       => $rs->fields['max_a'],
                'max_c_date'  => $rs->fields['max_c_date'],
                'max_a_date'  => $rs->fields['max_a_date'],
                'current_a'   => $current_a,
                'current_c'   => $current_c,
                'network'     => $net['name'],
                'group'       => $net['group']
                );

        $hosts[$ip] = $data;
        
        $group = $net['group'];
        $net_name = $net['name'];
        if ($group) {
            $groups[$group]['nets'][$net_name]['hosts'][$name] = $data;
        } else {
            $networks[$net_name]['hosts'][$name] = $data;
        }
        //printr($data);
    }
    $rs->MoveNext();
}

////////////////////////////////////////////////////////////////
// Global score
////////////////////////////////////////////////////////////////

$global = get_score("global_$user", 'global');
$global['current_a'] = get_current_metric('global', 'global', 'attack');
$global['current_c'] = get_current_metric('global', 'global', 'compromise');
$global['threshold_a'] = $conf_threshold;
$global['threshold_c'] = $conf_threshold;

////////////////////////////////////////////////////////////////
// Permissions & Ordering
////////////////////////////////////////////////////////////////

foreach ($networks as $net => $net_data) {
    $net_perms = $net_data['has_perms'];
    if (!$net_perms) {
        unset($networks[$net]);
    }
}
// Groups
$order_by_risk_type = 'compromise';
uasort($groups, 'order_by_risk');
foreach ($groups as $group => $group_data) {
    $group_perms = $group_data['has_perms'];
    uasort($groups[$group]['nets'], 'order_by_risk');
    foreach ($group_data['nets'] as $net => $net_data) {
        $net_perms = $net_data['has_perms'];
        if (isset($groups[$group]['nets'][$net]['hosts'])) {
            uasort($groups[$group]['nets'][$net]['hosts'], 'order_by_risk');
        }
        // the user doesn't have perms over the group but only over
        // some networks of it. List that networks as networks outside
        // groups.
        if (!$group_perms && $net_perms) {
            $networks[$net] = $net_data;
        }
    }
    if (!$group_perms) {
        unset($groups[$group]);
    }
}
// Networks outside groups
uasort($networks, 'order_by_risk');
// Hosts in networks
uasort($hosts, 'order_by_risk');
// Host outside networks
uasort($ext_hosts, 'order_by_risk');

////////////////////////////////////////////////////////////////
// HTML Code
////////////////////////////////////////////////////////////////

?>
<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
  <script src="../js/prototype.js" type="text/javascript"></script>
  <script language="javascript">
  <!--
    function toggle(type, start_id, end_id, link_id)
    {
        if ($(link_id+'_c').innerHTML == '+') {
            for (i=0; i < end_id; i++) {
                id = start_id + i;
                tr_id = type + '_' + id;
                Element.show(tr_id+'_c');
                Element.show(tr_id+'_a');
            }
            $(link_id+'_c').innerHTML = '-';
            $(link_id+'_a').innerHTML = '-';
        } else {
            for (i=0; i < end_id; i++) {
                id = start_id + i;
                tr_id = type + '_' + id;
                Element.hide(tr_id+'_c');
                Element.hide(tr_id+'_a');
            }
            $(link_id+'_c').innerHTML = '+';
            $(link_id+'_a').innerHTML = '+';
        }
    }
  -->
  </script>
  <style type="text/css">

  body.score {
      margin-right: 5px;
      margin-left: 5px;
  }
  </style>
  
</head>
<body class="score">

<table width="100%" align="center" style="border: 0px;">
<tr>
<td class="noborder" colspan="2">
<!--

Page Header (links, riskmeter, rrd)

-->
    <table width="100%" align="center" style="border: 0px;">
    <tr>
    <td colspan="2">
    <? foreach (array('day'  =>_("Last day"),
                      'week' =>_("Last week"),
                      'month'=>_("Last month"),
                      'year' =>_("Last year")) as $r => $text) {
        if ($r == $range) echo '<b>';
    ?>
       [ <a href="<?=$_SERVER['PHP_SELF']?>?range=<?=$r?>"><?=$text?></a> ]&nbsp;
    <?
        if ($r == $range) echo '</b>';
    } ?>
    </td>
    </tr><tr>
    <td class="noborder">
    <img src="../report/graphs/draw_rrd.php?ip=global_<?=$user?>&what=compromise&start=<?=$rrd_start?>&end=N&type=global&zoom=0.85">
    </td>
    <td class="noborder">
    <table>
        <tr>
          <th><?=_("Riskmeter")?></th>
          <th><?=_("Service Level")?>&nbsp;</th>
        </tr><tr>
        <td class="noborder">
            <a href="../riskmeter/index.php"><img border="0" src="../pixmaps/riskmeter.png"/></a>
        </td>
        <?=html_service_level()?>
        </tr>
    </table>
    </td>
    </tr>
    </table>
</td>
</tr>
<tr>
<?
foreach (array('compromise', 'attack') as $metric_type) {
    $a = 1;
    $net = $host = 0;
    if ($metric_type == 'compromise') {
        $title = _("C O M P R O M I S E");
        $ac = 'c';
    } else {
        $title = _("A T T A C K");
        $ac = 'a';
    }
?>
<td width="50%" class="noborder" valign="top">
    <table width="100%" align="center">
    <tr><td colspan="6"><center><b><?=$title?></b></center></td></tr>
    <tr>
        <th colspan="6" class="noborder"><?=_("Global")?></th>
    </tr>
<!--

Global

-->
    <tr>
        <th colspan="3"><?=_("Global")?></th>
        <th><?=_("Max Date")?></th>
        <th><?=_("Max")?></th>
        <th><?=_("Current")?></th>
    </tr>
    <tr>
        <td colspan="2"><b><?=_("GLOBAL SCORE")?><b></td>
        <?
        html_set_values("global_$user",
                   'global',
                   $global["max_$ac"],
                   $global["max_{$ac}_date"],
                   $global["current_$ac"],
                   $global["threshold_$ac"],
                   $ac
                   );
        ?>
        <td><?=html_rrd()?> <?=html_report()?></td>
        <td nowrap><?=html_date()?></td>
        <?=html_max()?>
        <?=html_current()?>
    </tr>
    <tr>
        <td colspan="6" class="noborder">&nbsp;</td>
    </tr>
<!--

Network Groups

-->
    <? if (count($groups)) { ?>
    <tr>
        <th colspan="6" class="noborder"><?=_("Network Groups")?></th>
    </tr>
    <tr>
        <th colspan="3"><?=_("Group")?></th>
        <th><?=_("Max Date")?></th>
        <th><?=_("Max")?></th>
        <th><?=_("Current")?></th>
    </tr>
        <?
        
        foreach ($groups as $group_name => $group_data) {
            $num_nets = count($group_data['nets']);
        ?>
            <tr>
            <td class="noborder">
                <a id="a_<?=++$a?>_<?=$ac?>" href="javascript: toggle('net', <?=$net+1?>, <?=$num_nets?>, 'a_<?=$a?>');">+</a>
            </td>
            <td style="text-align: left"><b><?=$group_name?></b></td>
            <?
            html_set_values('group_'.$group_name,
                       'net',
                       $group_data["max_$ac"],
                       $group_data["max_{$ac}_date"],
                       $group_data["current_$ac"],
                       $group_data["threshold_$ac"],
                       $ac
                       );
            ?>
            <td><?=html_rrd()?> <?=html_report()?></td>
            <td nowrap><?=html_date()?></td>
            <?=html_max()?>
            <?=html_current()?>
            </tr>
            <?
            foreach ($group_data['nets'] as $net_name => $net_data) {
                $net++;
                $num_hosts = isset($net_data['hosts']) ? count($net_data['hosts']) : 0;
            ?>
                <tr id="net_<?=$net?>_<?=$ac?>" style="display: none">
                    
                    <td width="3%" class="noborder">&nbsp;</td>
                    <td style="text-align: left">
                        <? if ($num_hosts) { ?>
                        <a id="a_<?=++$a?>_<?=$ac?>" href="javascript: toggle('host', <?=$host+1?>, <?=$num_hosts?>, 'a_<?=$a?>');">+</a>&nbsp;
                        <? } ?>
                        <?=$net_name?>
                    </td>
                    <?
                    html_set_values($net_name,
                               'net',
                               $net_data["max_$ac"],
                               $net_data["max_{$ac}_date"],
                               $net_data["current_$ac"],
                               $net_data["threshold_$ac"],
                               $ac
                               );
                    ?>
                    <td><?=html_rrd()?> <?=html_report()?></td>
                    <td nowrap><?=html_date()?></td>
                    <?=html_max()?>
                    <?=html_current()?>
                </tr>
                <?
                if (isset($net_data['hosts'])) {
                    foreach ($net_data['hosts'] as $host_name => $host_data) {
                        $host++;
                ?>
                        <tr id="host_<?=$host?>_<?=$ac?>" style="display: none">
                            <td width="6%" style="border: 0px;">&nbsp;</td>
                            <td style="text-align: left">&nbsp;&nbsp;
                                <a href="../report/index.php?host=<?=$host_name?>&section=metrics"><?=$host_name?></a>
                            </td>
                            <?
                            html_set_values($host_name,
                                       'host',
                                       $host_data["max_$ac"],
                                       $host_data["max_{$ac}_date"],
                                       $host_data["current_$ac"],
                                       $host_data["threshold_$ac"],
                                       $ac
                                       );
                            ?>
                            <td><?=html_rrd()?> <?=html_report()?></td>
                            <td nowrap><?=html_date()?></td>
                            <?=html_max()?>
                            <?=html_current()?>
                        </tr>   
                   <? } ?>
               <? } ?>
            <? } ?>
        <? } ?>
    <? } ?>
<!--

Network outside groups

-->
    <? if (count($networks)) { ?>
    <tr>
        <th colspan="6" class="noborder"><?=_("Networks outside groups")?></th>
    </tr>
    <tr>
        <th colspan="3"><?=_("Network")?></th>
        <th><?=_("Max Date")?></th>
        <th><?=_("Max")?></th>
        <th><?=_("Current")?></th>
    </tr>
        <?
        $i = 0;
        foreach ($networks as $net_name => $net_data) {
            $num_hosts = isset($net_data['hosts']) ? count($net_data['hosts']) : 0;
        ?>
        <tr>
        <td colspan="2" style="text-align: left">
            <? if ($num_hosts) { ?>
            <a id="a_<?=++$a?>_<?=$ac?>" href="javascript: toggle('host', <?=$host+1?>, <?=$num_hosts?>, 'a_<?=$a?>');">+</a>&nbsp;
            <? } ?>
            <b><?=$net_name?></b>
        </td>
        <?
        html_set_values($net_name,
                   'net',
                   $net_data["max_$ac"],
                   $net_data["max_{$ac}_date"],
                   $net_data["current_$ac"],
                   $net_data["threshold_$ac"],
                   $ac
                   );
        ?>
        <td nowrap><?=html_rrd()?> <?=html_report()?></td>
        <td nowrap><?=html_date()?></td>
        <?=html_max()?>
        <?=html_current()?>
        </tr>
            <?
            if ($num_hosts) {
                uasort($net_data['hosts'], 'order_by_risk');
                foreach ($net_data['hosts'] as $host_name => $host_data) {
                    $host++;
            ?>
                    <tr id="host_<?=$host?>_<?=$ac?>" style="display: none">
                        <td width="3%" style="border: 0px;">&nbsp;</td>
                        <td style="text-align: left">&nbsp;&nbsp;
                            <a href="../report/index.php?host=<?=$host_name?>&section=metrics"><?=$host_name?></a>
                        </td>
                        <?
                        html_set_values($host_name,
                                   'host',
                                   $host_data["max_$ac"],
                                   $host_data["max_{$ac}_date"],
                                   $host_data["current_$ac"],
                                   $host_data["threshold_$ac"],
                                   $ac
                                   );
                        ?>
                        <td><?=html_rrd()?> <?=html_report()?></td>
                        <td nowrap><?=html_date()?></td>
                        <?=html_max()?>
                        <?=html_current()?>
                    </tr>   
               <? } ?>
           <? } ?>
        <? } ?>
    <? } ?>
<!--

Hosts

-->
    <? if (count($hosts)) { ?>
    <tr>
        <th colspan="6" class="noborder"><?=_("Hosts")?></th>
    </tr>
    <tr>
        <th colspan="3"><?=_("Host Address")?></th>
        <th><?=_("Max Date")?></th>
        <th><?=_("Max")?></th>
        <th><?=_("Current")?></th>
    </tr>
        <?
        $i = 0;
        foreach ($hosts as $ip => $host_data) {
        ?>
        <tr>
        <td nowrap colspan="2" style="text-align: left">
            <a href="../report/index.php?host=<?=$ip?>&section=metrics"><?=$host_data['name']." - ".$host_data['network']?><br>(<?=$host_data['group']?>)</a>
        </td>
        <?
        html_set_values($ip,
                   'host',
                   $host_data["max_$ac"],
                   $host_data["max_{$ac}_date"],
                   $host_data["current_$ac"],
                   $host_data["threshold_$ac"],
                   $ac
                   );
        ?>
        <td><?=html_rrd()?> <?=html_report()?></td>
        <td nowrap><?=html_date()?></td>
        <?=html_max()?>
        <?=html_current()?>
        </tr>
        <? } ?>
    <? } ?>
<!--

Hosts outside networks

-->
    <? if (count($ext_hosts)) { ?>
    <tr>
        <th colspan="6" class="noborder"><?=_("Hosts outside defined networks")?></th>
    </tr>
    <tr>
        <th colspan="3"><?=_("Host Address")?></th>
        <th><?=_("Max Date")?></th>
        <th><?=_("Max")?></th>
        <th><?=_("Current")?></th>
    </tr>
        <?
        $i = 0;
        foreach ($ext_hosts as $ip => $host_data) {
        ?>
        <tr>
        <td colspan="2" style="text-align: left">
            <a href="../report/index.php?host=<?=$ip?>&section=metrics"><?=$ip?></a>
        </td>
        <?
        html_set_values($ip,
                   'host',
                   $host_data["max_$ac"],
                   $host_data["max_{$ac}_date"],
                   $host_data["current_$ac"],
                   $host_data["threshold_$ac"],
                   $ac
                   );
        ?>
        <td><?=html_rrd()?> <?=html_report()?></td>
        <td nowrap><?=html_date()?></td>
        <?=html_max()?>
        <?=html_current()?>
        </tr>
        <? } ?>
    <? } ?>
    </table>
</td>
<? } ?>

</td>
</tr>
</table>
<br>
<b>Legend:</b><br>
<table width="30%" align="left">
<tr>
    <?=_html_metric(0, 100, '#')?>
    <td><?=_("No appreciable risk")?></td>
</tr>
<tr>
    <?=_html_metric(101, 100, '#')?>
    <td><?=_("Metric over 100% threshold")?></td>
</tr>
<tr>
    <?=_html_metric(301, 100, '#')?>
    <td><?=_("Metric over 300% threshold")?></td>
</tr>
<tr>
    <?=_html_metric(501, 100, '#')?>
    <td><?=_("Metric over 500% threshold")?></td>
</tr>
</table>
<br>
</body></html>