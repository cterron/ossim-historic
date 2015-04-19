<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsNetwork");
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
    
    #
    # get ntop proto and port from default ntop entry at
    # /etc/ossim/framework/ossim.conf
    # a better solution ??
    #
    
    if (!$conf->get_conf("use_ntop_rewrite")){

        $url_parsed = parse_url($conf->get_conf("ntop_link"));
        $port = $url_parsed["port"];
        $protocol = $url_parsed["scheme"];

        $fr_up = "menu.php?sensor=$sensor&port=$port&proto=$protocol";
        $fr_down = "$protocol://$sensor:$port/trafficStats.html";
    } else { //if use_ntop_rewrite is enabled
        
        $protocol = "http";
        if (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == "on") $protocol = "https";

        $fr_up = "menu.php?sensor=$sensor";
        $fr_down = "$protocol://".$_SERVER['SERVER_NAME']."/ntop-$sensor/";
    }
?>
<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="<?php echo $fr_up ?>">
<frame src="<?php echo $fr_down ?>" name="ntop">

<body>
</body>
</html>

