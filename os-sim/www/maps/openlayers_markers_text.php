<?php
//
// This file is no longer in use and will be probably removed soon
//
ob_start();
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once '../sensor/get_sensors.php';
ob_end_clean();
Session::logcheck("MenuConfiguration", "ConfigurationMaps");

$map_id = GET('map_id') ? GET('map_id') : die("Invalid map_id");

$db = new ossim_db();
$conn = $db->connect();

function get_sensor_status($ip)
{
    global $conn;
    static $sensors = array();
    if (!count($sensors)) {
        ob_start();
        $sensors = server_get_sensors($conn);
        ob_end_clean();
    }
    foreach ($sensors as $s) {
        if ($s['sensor'] == $ip) {
            return $s['state'];
        }
    }
    return 'off';
}

function search_sensor($name)
{
    global $conn;
    static $sensors = array();
    if (!count($sensors)) {
        $sql = "SELECT ip, name FROM sensor";
        if (!$rs = $conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        while (!$rs->EOF) {
            $sensors[$rs->fields['name']] = array('ip' => $rs->fields['ip'],
                                                  'state' => get_sensor_status($rs->fields['ip']));
            $rs->MoveNext();
        }
    }
    return isset($sensors[$name]) ? $sensors[$name] : false;
}

$sql = "SELECT id, type, ossim_element_key, x, y FROM map_element WHERE map_id=?";
if (!$rs = $conn->Execute($sql, array($map_id))) {
    die($conn->ErrorMsg());
}

$items = array();
while (!$rs->EOF) {
    $s_name = $rs->fields['ossim_element_key'];
    if ($s_data = search_sensor($s_name)) {
        $items[$s_name]['state'] = $s_data['state'];
    } else {
        $items[$s_name]['state'] = false;
    }
    $items[$s_name]['x'] = $rs->fields['x'];
    $items[$s_name]['y'] = $rs->fields['y'];
    $rs->MoveNext();
}
header("Content-type: text/plain");
echo "point\ttitle\tdescription\ticon\n";
foreach ($items as $name => $i) {
    $icon = $i['state'] == 'on' ? '../js/OpenLayers/img/marker-green.png' : '../js/OpenLayers/img/marker.png';
    echo $i['y'].",".$i['x']."\t".
         $name."\t".
         "Sensor status: ".$i['state'].' '.$i['y'].",".$i['x']."\t".
         $icon."\n";
}
?>