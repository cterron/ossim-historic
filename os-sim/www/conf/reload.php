<?php
require_once ("classes/Session.inc");
require_once ("classes/Security.inc");

    $what = GET('what');

    ossim_valid($what, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("What"));
    
    if (ossim_error()) {
        die(ossim_error());
    }

    /* what to reload... */
    if (empty($what))  $what = 'all';

    require_once ('ossim_conf.inc');
    $ossim_conf = $GLOBALS["CONF"];

    /* get the port and IP address of the server */
    $address = $ossim_conf->get_conf("server_address");
    $port = $ossim_conf->get_conf("server_port");

    /* create socket */
    $socket = socket_create (AF_INET, SOCK_STREAM, 0);
    if ($socket < 0) {
        printf(gettext("socket_create() failed: reason: %s\n"), socket_strerror($socket)); 
    }

    /* connect */
    $result = socket_connect ($socket, $address, $port);
    if ($result < 0) {
        printf(gettext("socket_connect() failed: reason: %s %s\n"), $result, socket_strerror($result));
    } 

    $in = 'connect id="1"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    $out = socket_read ($socket, 2048);
    if (strncmp($out, 'ok id="1"', 9) != 0) {
        echo gettext("Error connecting to server") . " ...\n";
        exit;
    }
    
    $in = 'reload-' . $what . ' id="2"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
    $out = socket_read ($socket, 2048);
    if (strncmp($out, 'ok id="2"', 9) != 0) {
        echo gettext("Bad response from server") . " ...\n";
        exit;
    }

    socket_close ($socket);
?>

<html>
<head>
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
  <p> <?php echo gettext("Reload completed successfully"); ?> </p>
</body>
</html>


