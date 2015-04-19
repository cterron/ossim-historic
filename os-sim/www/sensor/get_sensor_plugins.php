<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
?>

<?php

function server_get_sensor_plugins() {

    require_once ('ossim_conf.inc');
    $ossim_conf = $GLOBALS["CONF"];

    /* get the port and IP address of the server */
    $address = $ossim_conf->get_conf("server_address");
    $port = $ossim_conf->get_conf("server_port");

    /* create socket */
    $socket = socket_create (AF_INET, SOCK_STREAM, 0);
    if ($socket < 0) {
        echo "socket_create() failed: reason: " . 
            socket_strerror ($socket) . "\n";
    }

    $list = array();

    /* connect */
    $result = @socket_connect ($socket, $address, $port);
    if (!$result) {
        echo "<p><b>socket error</b>: " . gettext("Is OSSIM server running at") . " $address:$port?</p>";
        return $list;
} 

    /* first send a connect message to server */
    $in = 'connect id="1" type="web"' . "\n";
    $out = '';
    socket_write($socket, $in, strlen($in));
    $out = socket_read($socket, 2048, PHP_NORMAL_READ);
    if (strncmp($out, "ok id=", 4)) {
        echo "<p><b>" . gettext("Bad response from server") . "</b></p>";
		return $list;
    }

    /* get sensor plugins from server */
    $in = 'server-get-sensor-plugins id="2"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    
    $pattern = '/sensor="([^"]*)" plugin_id="([^"]*)" ' . 
               'state="([^"]*)" enabled="([^"]*)"/';
               
    while ($out = socket_read ($socket, 2048, PHP_NORMAL_READ)) 
    {
        if (preg_match($pattern, $out, $regs)) 
        {
            $s["sensor"]    = $regs[1];
            $s["plugin_id"] = $regs[2];
            $s["state"]     = $regs[3];
            $s["enabled"]   = $regs[4];
            if (!in_array($s, $list)) 
                $list[] = $s;
        } elseif (!strncmp($out, "ok id=", 4)) {
            break;
        }
    }
    
    socket_close ($socket);

    return $list;
}

#
# debug
# print_r(server_get_sensor_plugins());
#

?>
