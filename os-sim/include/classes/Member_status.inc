<?php

// Manager class
class Member_status
{
    function Member_status()
    {
        // Initialize all member specific objects
        $this->sensor = new Member_status_sensor();
    }

    /*
    $info = array(
        'name' => 
        'icon' =>
        'status_value' =>
        'status_text' =>
        'link' =>
        'description' =>
    );
    */
    
    function get($key, $type)
    {
        switch ($type) {
            case 'sensor':
                return $this->sensor->get($key); break;
        }
        return false;
    }
    
    /*
     * @param string $level => 'ok', 'low', 'med', 'high'
     */
    function get_icon($type, $level)
    {
        switch ($type) {
            case 'sensor':
                return $this->sensor->get_icon($level); break;
        }
        return '';
    }
}

class Member_status_sensor
{
    var $icon_on = '../js/OpenLayers/img/marker-green.png';
    var $icon_off = '../js/OpenLayers/img/marker.png';
    
    function Member_status_sensor()
    {
        include_once '../sensor/get_sensors.php';
        include_once 'classes/Sensor_interfaces.inc';
        $this->ntop = new Member_status_ntop_throughput;
    }
    
    /*
     * @return string 'on' or 'off'
     */
    function _get_status($ip)
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
    
    function _interface_througput($sensor)
    {
        global $conn;
        $list = Sensor_interfaces::get_list($conn, $sensor);
        $ret = '';
        foreach ($list as $data) {
            $t = $this->ntop->get($sensor, $data->get_interface());
            if (!$t) {
                $t = _("n/a");
            }
            $ret .= $data->get_interface() . ': ' . $t . " kb/s<br>\n"; 
        }
        return $ret;
    }
    
    function _connected_nets($name)
    {
        global $conn;
        $sql = "SELECT net_name FROM net_sensor_reference WHERE sensor_name=?";
        if (!$rs = $conn->Execute($sql, array($name))) {
            die($conn->ErrorMsg());
        }
        $nets = array();
        while (!$rs->EOF) {
            $nets[] = $rs->fields['net_name'];
            $rs->MoveNext();
        }
        if (!count($nets)) {
            return "<i>"._("No networks connected")."</i>";
        }
        return implode('<br>', $nets);
    }
    
    function get($name)
    {
        global $conn;
        $sql = "SELECT ip, name FROM sensor WHERE name=?";
        $row = $conn->GetRow($sql, array($name));
        if ($row === false) {
            die($conn->ErrorMsg());
        }
        $status = $this->_get_status($row['ip']);
        $icon = $status == 'on' ? $this->icon_on : $this->icon_off;
        $status_value = $status == 'on' ? 1 : 0;
        $status_link = false;
        
        if ($status == 'on') {
            $description = _("Sensor is"). " <font color='green'><b>"._("up")."</b></font><br>\n";
            $description .= "<br><b>"._("Throughput") . "</b>:<br>\n".
                            $this->_interface_througput($row['name']);
            $description .= "<br><b>"._("Connected networks") . "</b>:<br>\n".
                            $this->_connected_nets($row['name']);
        } else {
            $description = _("Sensor is"). " <font color='red'><b>"._("down")."</b></font><br>\n";
        }
        return array( 'ip' => $row['ip'],
                      'name' => $row['name'],
                      'status_value' => $status_value,
                      'status_text'  => $status,
                      'icon'         => $icon,
                      'link'         => '', // XXX add link
                      'description'  => $description
                      );
    }
    
    function get_icon($level)
    {
        if ($level == 'ok') {
            return $this->icon_on;
        }
        return $this->icon_off;
    }
}

class Member_status_ntop_throughput
{
    var $start = 'now-600s'; //ntop notation
    var $end = 'now';
    
    function get($ip, $interface)
    {
        $start = $this->start;
        $end = $this->end;
        $ntop = "http://$ip:3000/plugins/rrdPlugin?action=arbreq&which=csvfetch&arbfile=throughput&arbiface=$interface&arbip=&start=$start&end=$end";
        $fd = @fopen($ntop, 'rb');
        if (!$fd) {
            return false;
        }
        /* Format is (seems to be localizated against the system's locale):
         * 
         * "file","/var/ntop/rrd/interfaces/eth0/throughput.rrd"

            "lun 04 jun 2007 17:25:20 CEST",1180970720,3014,4
            "lun 04 jun 2007 17:25:40 CEST",1180970740,3245,48
            "lun 04 jun 2007 17:26:00 CEST",1180970760,433,76
         */
         fgets($fd); fgets($fd);
         $times = 0;
         $throughput = 0;
         while (!feof($fd)) {
             $line = trim(fgets($fd));
             // stop condition
             if ($line == '') {
                 break;
             }
             $parts = explode(',', $line, 3);
             if (count($parts) != 3) {
                 return false; // wrong format or error
             }
             $throughput += (float) str_replace(',', '.', $parts[2]);
             $times++;
         }
         // reported some "division by zero errors"
         // rrdPlugin use b/sec, conver to to kb/s
         return $times > 0 ? round($throughput/$times/1024) : 0;
    }
}

class Member_status_bp_asset
{
    function get()
    {
        
    }
}
?>
