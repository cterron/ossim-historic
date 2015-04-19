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
    ossim_valid($what, OSS_ALPHA, OSS_NULLABLE, OSS_PUNC, 'illegal:'._("what"));
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
<table align="center">
  <tr>
    <td align="center" colspan="2">
      [<a href="<?= $_SERVER["PHP_SELF"] ?>?range=all&ip=<?=
      "$ip&what=$what&start=N-1D&type=$type&zoom=$zoom"?>"> <?= _("All"); ?> </a>]
      [<a href="<?= $_SERVER["PHP_SELF"] ?>?range=day&ip=<?=
      "$ip&what=$what&start=N-1D&type=$type&zoom=$zoom"?>"> <?= _("Last Day"); ?> </a>]
      [<a href="<?= $_SERVER["PHP_SELF"] ?>?range=week&ip=<?=
      "$ip&what=$what&start=N-7D&type=$type&zoom=$zoom"?>"> <?= _("Last Week"); ?> </a>]
      [<a href="<?= $_SERVER["PHP_SELF"] ?>?range=month&ip=<?=
      "$ip&what=$what&start=N-1M&type=$type&zoom=$zoom"?>"> <?= _("Last Month"); ?> </a>]
      [<a href="<?= $_SERVER["PHP_SELF"] ?>?range=year&ip=<?=
      "$ip&what=$what&start=N-1Y&type=$type&zoom=$zoom"?>"> <?= _("Last Year"); ?> </a>]
    </td>
  </tr>

<?php
    /* range = day, week, month or year. Only display a single graph */
    if ($range != "all") { 
?>
  <tr>
    <td class="noborder" style="text-align:right">
      <img src="<?php echo "../report/graphs/draw_rrd.php?ip=$ip&what=$what&start=$start&end=N&type=$type"; ?>">
    </td>
    <td class="noborder" style="text-align:left">
       file name: <b><?=$ip?>.rrd</b><br/>
       date range: <?=$range?><br/>
       rrd type: <?=$type?><br/>
    </td>
  </tr>
  
<?php
    /* range = all, display all graphs */
    } else {
        $dates = array ("day"   => "N-1D",
                       "week"  => "N-7D",
                       "month" => "N-1M",
                       "year"  => "N-1Y");
        foreach ($dates as $date_legend => $date_rrd) {
?>
  <tr>
    <td class="noborder" style="text-align:right">
      <img src="<?= "../report/graphs/draw_rrd.php?ip=$ip&what=$what&start=$date_rrd&end=N&type=$type"; ?>">
    </td>
    <td class="noborder" style="text-align:left">
       file name: <b><?=$ip?>.rrd</b><br/>
       date range: <?=$date_legend?><br/>
       rrd type: <?=$type?><br/>
    </td>
  </tr>
<?php
        } /* foreach */
    } /* else */
?>
</table>
</HTML>
<BODY>
