<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsSession");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<?php

    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();

    if (!$sensor = $_GET["sensor"]) 
    {
        echo "<p align=\"center\"> " . gettext("Please select a sensor") . " </p>";
        exit;
    }
       
    #
    # get ntop proto and port from default ntop entry at
    # /etc/ossim/framework/ossim.conf
    # a better solution ??
    #
    $url_parsed = parse_url($conf->get_conf("ntop_link"));
    $port = $url_parsed["port"];
    $protocol = $url_parsed["scheme"];

    $fr_up = "menu_session.php?sensor=$sensor&port=$port&proto=$protocol";
    $fr_down = "$protocol://$sensor:$port/NetNetstat.html";

?>

<frameset rows="70,85%" border="0" frameborder="0">
<frame src="<?php echo $fr_up ?>">
<frame src="<?php echo $fr_down ?>" name="ntop">

<body>
</body>
</html>

