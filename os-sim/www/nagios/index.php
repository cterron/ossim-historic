<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsAvailability");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<?php
    require_once ("classes/Security.inc");
    
    $sensor = GET('sensor');

    ossim_valid($sensor, OSS_ALPHA, OSS_PUNC, OSS_SPACE, 'illegal:'._("Sensor"));

    if (ossim_error()) {
            die(ossim_error());
    }
 

    require_once ('ossim_conf.inc');
    $conf = $GLOBALS["CONF"];

    $fr_up = "menu.php?sensor=$sensor";
    $fr_down = $conf->get_conf("nagios_link") . 
        "/cgi-bin/status.cgi?hostgroup=all";
?>
<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="<?php echo $fr_up ?>">
<frame src="<?php echo $fr_down ?>" name="nagios">

<body>
</body>
</html>

