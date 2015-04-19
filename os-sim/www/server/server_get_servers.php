<?php

require_once ('classes/Session.inc');
require_once ('classes/Util.inc');

function check_server($conn) {

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

    /* connect */
    $result = @socket_connect ($socket, $address, $port);
    if (!$result) {
        return false;
    } 
    return true;
}



function server_get_servers($conn) {

    $name = GET ('name');
    ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Server name"));

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

    /* get servers from server */
		if ($name != NULL)
      $in = 'server-get-servers id="2" servername="' . $name . '"' . "\n";
		else
      $in = 'server-get-servers id="2"' . "\n";

    $out = '';
    socket_write ($socket, $in, strlen ($in));
    
    $pattern = '/server host="([^"]*)" servername="([^"]*)"/ ';
               
    while ($out = socket_read ($socket, 2048, PHP_NORMAL_READ)) 
    {
        if (preg_match($pattern, $out, $regs)) 
        {
            if (Session::hostAllowed($conn, $regs[1]))
            {
                $s["host"]    = $regs[1];
                $s["servername"]     = $regs[2];
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


