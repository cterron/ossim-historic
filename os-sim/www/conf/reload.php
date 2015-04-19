<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyServers");   // Who manage server can reload server conf

require_once ("classes/Session.inc");
require_once ("classes/Security.inc");

    $what = GET('what');
    $back = GET('back');

    ossim_valid($what, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("What"));
    ossim_valid($back, OSS_ALPHA, OSS_PUNC, 'illegal:'._("back"));
    
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

    $in = 'connect id="1" type="web"' . "\n";
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

    // Switch off web indicator
    require_once ('classes/WebIndicator.inc');
    if ($what == "all") {
        WebIndicator::set_off("Reload_policies");
        WebIndicator::set_off("Reload_hosts");
        WebIndicator::set_off("Reload_nets");
        WebIndicator::set_off("Reload_sensors");
        WebIndicator::set_off("Reload_plugins");
        WebIndicator::set_off("Reload_directives");
        WebIndicator::set_off("Reload_servers");
    } else {
    	WebIndicator::set_off("Reload_" . $what);
    }

    // Reset main indicator if no more policy reload need   
    if (!WebIndicator::is_on("Reload_policies") &&
        !WebIndicator::is_on("Reload_hosts") &&
        !WebIndicator::is_on("Reload_nets") &&
        !WebIndicator::is_on("Reload_sensors") &&
        !WebIndicator::is_on("Reload_plugins") &&
        !WebIndicator::is_on("Reload_directives") &&
        !WebIndicator::is_on("Reload_servers")) {
            WebIndicator::set_off("ReloadPolicy");
        }
    
    // update indicators on top frame
    $OssimWebIndicator->update_display();
?>

<html>
<head>
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
  <p> <?php echo gettext("Reload completed successfully"); ?> </p>
  <p><a href="<?php echo urldecode($back); ?>"> <?php echo gettext("Back"); ?> </a></p>
</body>
</html>


