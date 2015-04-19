<?php
    require_once ('classes/Session.inc');
    Session::logcheck("MenuControlPanel", "ControlPanelMetrics");

    require_once ('ossim_conf.inc');
    require_once ("ossim_db.inc");
    require_once ('classes/Security.inc');
    $conf = $GLOBALS["CONF"];

    
    $range  = GET('range');
    $ip     = GET('ip');
    $what   = GET('what');
    $start  = GET('start');
    $type   = GET('type');
    $zoom   = GET('zoom');

    ossim_valid($range, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("range"));
    ossim_valid($ip, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("ip"));
    ossim_valid($what, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("what"));
    ossim_valid($start, OSS_ALPHA, OSS_PUNC, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("start"));
    ossim_valid($type, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("type"));
    ossim_valid($zoom, OSS_DIGIT, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("zoom"));

    if (ossim_error()) {
        die(ossim_error());
    }

?>

<html>
<head>
  <title> <?php echo "$ip " . gettext("graph"); ?> </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>
<table align="center" width="100%">
    <tr><td align="center">
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day&ip=<?php echo
      "$ip&what=$what&start=N-1D&type=$type&zoom=$zoom"?>"> <?php echo gettext("Last Day"); ?> </a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=week&ip=<?php echo
      "$ip&what=$what&start=N-7D&type=$type&zoom=$zoom"?>"> <?php echo gettext("Last Week"); ?> </a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month&ip=<?php echo
      "$ip&what=$what&start=N-1M&type=$type&zoom=$zoom"?>"> <?php echo gettext("Last Month"); ?> </a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year&ip=<?php echo
      "$ip&what=$what&start=N-1Y&type=$type&zoom=$zoom"?>"> <?php echo gettext("Last Year"); ?> </a>]
    </td><td><?php echo gettext("Show"); ?> [<a href="<?php echo $_SERVER["PHP_SELF"]?><?php echo
    "?range=$range&ip=$ip&what=compromise&start=$start&type=$type&zoom=$zoom"?>">
    <?php echo gettext("Compromise"); ?> </a>]<BR>
    <?php echo gettext("Show"); ?> [<a href="<?php echo $_SERVER["PHP_SELF"]?><?php echo
    "?range=$range&ip=$ip&what=attack&start=$start&type=$type&zoom=$zoom"?>">
    <?php echo gettext("Attack"); ?> </a>]</td></tr>
    <tr><td colspan="2"><HR noshade></td></tr>
    <tr><td colspan="2" align="center">
      <img src="<?php echo "../report/graphs/draw_rrd.php?ip=$ip&what=$what&start=$start&end=N&type=$type"; ?>">
    </td></tr>
    </table>
</HTML>
<BODY>
