<?php
    require_once ('classes/Session.inc');
    Session::logcheck("MenuMonitors", "MonitorsRiskmeter");

    require_once 'classes/Security.inc';

    $ip = GET('ip');

    ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("IP address"));

    if (ossim_error()) {
        die(ossim_error());
    }
                                
    require_once ('ossim_db.inc');
    require_once ('classes/Host_qualification.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    Host_qualification::delete($conn, $ip);
?>    
    <p align="center"> <?php echo gettext("Reset completed"); ?> </p>

<?php
    $db->close($conn);
?>


