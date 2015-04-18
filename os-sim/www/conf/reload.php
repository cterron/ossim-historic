<?php

    /* what to reload... */
    if (!$what = $_GET["what"]) $what = 'all';

    require_once ('ossim_conf.inc');
    $ossim_conf = new ossim_conf();

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
    $result = socket_connect ($socket, $address, $port);
    if ($result < 0) {
        echo "socket_connect() failed.\nReason: ($result) " .
            socket_strerror($result) . "\n";
    } 

    $in = 'connect id="1"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    $out = socket_read ($socket, 2048);
    if (strncmp($out, 'ok id="1"', 9) != 0) {
        echo "Error connecting to server...\n";
        exit;
    }
    
    $in = 'reload-' . $what . ' id="2"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    $out = socket_read ($socket, 2048);
    if (strncmp($out, 'ok id="2"', 9) != 0) {
        echo "Bad response from server...\n";
        exit;
    }

    socket_close ($socket);
?>

<html>
<head>
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
  <p>Reload completed successfully</p>
</body>
</html>


