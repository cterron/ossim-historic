<?php

require_once ('classes/Session.inc');
require_once ('classes/Util.inc');

function server_get_sensors($conn) {

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

    $in = 'server-get-sensors id="1"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    
    $pattern = '/sensor host="([^"]*)" state="([^"]*)"/ ';
               
    while ($out = socket_read ($socket, 2048, PHP_NORMAL_READ)) 
    {
        if (preg_match($pattern, $out, $regs)) 
        {
            if (Session::hostAllowed($conn, $regs[1]))
            {
                $s["sensor"]    = $regs[1];
                $s["state"]     = $regs[2];

                ## This should be checked in the server TODO FIXME
                if (!in_array($s, $list))
                    $list[] = $s;
            }
        } elseif (!strncmp($out, "ok id=", 4)) {
            break;
        }
    }
    
    socket_close ($socket);
    return $list;
}

#
# debug
# print_r(server_get_sensors());
#

?>


