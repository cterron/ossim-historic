<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>

<html>
<head>
  <title> <?php echo gettext("Vulnmeter"); ?> </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

<?php
    require_once ('ossim_conf.inc');
    require_once ('ossim_sql.inc');
    require_once ('classes/Security.inc');
    require_once ('classes/Util.inc');

    $action = REQUEST('action');
    $scan_date = REQUEST('scan_date');
    
    ossim_valid($scan_date, OSS_ALPHA, OSS_PUNC, 'illegal:'._("Scan date"));
    ossim_valid($action, OSS_ALPHA, 'illegal:'._("Action"));

    if (ossim_error()) {
        die(ossim_error());
    }
    
    switch($action){
    case 'delete':
        print _("Deleting") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;
    case 'archive':
        print _("Archiving") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;
    case 'restore':
        print _("Restoring") . " " . Util::timestamp2date($scan_date) . "...<br>";
        break;
    default:
        require_once("ossim_error.inc");
        $error = new OssimError();
        $error->display("UNK_ACTION");
    }

    $conf = $GLOBALS["CONF"];
    $address = $conf->get_conf("frameworkd_address");
    $port = $conf->get_conf("frameworkd_port");

   /* create socket */
    $socket = socket_create (AF_INET, SOCK_STREAM, 0);
    if ($socket < 0) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("CRE_SOCKET", array(socket_strerror ($socket)));
    }

    /* connect */
    $result = @socket_connect ($socket, $address, $port);
    if (!$result) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("FRAMW_NOTRUN", array($address.":".$port));
    }

    $in = 'nessus action="' . $action . '" report="' . $scan_date . '"' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));

    $pattern = '/nessus \w+ ack ([^\s]*)/ ';

    while ($out = socket_read ($socket, 255, PHP_BINARY_READ))
    {
        if (preg_match($pattern, $out, $regs))
        {
            print gettext("Successfully") . " " . gettext($action . "d") . " " .  Util::timestamp2date($regs[1]);
            print "<br><a href=\"index.php\"> " . gettext("Back") . " </a>";
            socket_close ($socket);
            exit();
        }
    }

    socket_close ($socket);

?>
