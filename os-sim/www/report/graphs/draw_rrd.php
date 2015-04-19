<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'ossim_conf.inc';
Session::logcheck("MenuControlPanel", "ControlPanelMetrics");

$db = new ossim_db();
$conn = $db->connect();

$conf = $GLOBALS["CONF"];
$rrdtool_bin = $conf->get_conf("rrdtool_path") . "/rrdtool";

//
// This will show errors (both PHP Errors and those detected in the code)
// as graphics, so they can be read.
//
function mydie($errno, $errstr='', $errfile='', $errline='')
{
    global $conf;
    $jpgraph = $conf->get_conf("jpgraph_path");
    include_once "$jpgraph/jpgraph.php";
    
    $err = ($errstr) ? $errstr : $errno;
    if ($errfile) {
        switch ($errno) {
            case 1: $errprefix = 'Error'; break;
            case 2: $errprefix = 'Warning'; break;
            case 8: $errprefix = 'Notice'; break;
            default:
                return; // dont show E_STRICT errors
        }
        $err = "$errprefix: $err in '$errfile' line $errline";
    }
    $error = new JpGraphError();
    $error->Raise($err);
    exit;
}
set_error_handler('mydie');

$ip   = GET('ip');
$what = GET('what');
$type = GET('type');
$start = GET('start');
$end  = GET('end');
$zoom = GET('zoom') ? GET('zoom') : 1;
//
// params validations
//

if (!in_array($what, array('compromise', 'attack'))) {
    mydie(sprintf(_("Invalid param '%s' with value '%s'"), 'what', $what));
}
if (!in_array($type, array('host', 'net', 'global', 'level'))) {
    mydie(sprintf(_("Invalid param '%s' with value '%s'"), 'type', $type));
}
ossim_valid($ip,    OSS_LETTER, OSS_DIGIT, OSS_DOT, OSS_SCORE, 'illegal:'._('IP'));
ossim_valid($start, OSS_LETTER, OSS_DIGIT, OSS_SCORE, 'illegal:'._('Start param'));
ossim_valid($end,   OSS_LETTER, OSS_DIGIT, OSS_SCORE, 'illegal:'._('End param'));
ossim_valid($zoom,  OSS_DIGIT, OSS_DOT, 'illegal:'._('Zoom parameter'));

if (ossim_error()) {
    mydie(strip_tags(ossim_error()));
}

//
// Where to find the RRD file
//
switch ($type) {
    case 'host':
        $rrdpath = $conf->get_conf('rrdpath_host'); break;
    case 'net':
        $rrdpath = $conf->get_conf('rrdpath_net'); break;
    case 'global':
        $rrdpath = $conf->get_conf('rrdpath_global'); break;
    case 'level':
        $rrdpath = $conf->get_conf('rrdpath_level'); break;
}

//
// Graph style
//
$font = $conf->get_conf('font_path');
$tmpfile = tempnam('/tmp', 'OSSIM');

function clean_tmp()
{
    global $tmpfile;
    @unlink($tmpfile);
}
register_shutdown_function('clean_tmp');


if ($what == "compromise") {
    $ds="ds0";
    $color1="#0000ff";
    $color2="#ff0000";
} elseif ($what == "attack") {
    $ds="ds1";
    $color1="#ff0000";
    $color2="#0000ff";
}

//
// Threshold calculations
//

// default values
$threshold_a = $threshold_c = $conf->get_conf('threshold');
$hostname  = $ip;

if ($type == 'host' || $type == 'net') {
    $column = $what == 'compromise' ? 'threshold_c' : 'threshold_a';
    $match  = $type == 'host' ? 'ip' : 'name';
    $sql = "SELECT threshold_c, threshold_a FROM $type WHERE $match = ?";
    if (!$rs = $conn->Execute($sql, array($ip))) {
        mydie($conn->ErrorMsg());
    }
    if (!$rs->EOF) { // if a specific threshold was set for this host, use it
        $threshold_c = $rs->fields['threshold_c'];
        $threshold_a = $rs->fields['threshold_a'];
    }
}
//
// RRDTool cmd execution
//
if (!is_file("$rrdpath/$ip.rrd")) {
    mydie(sprintf(_("No RRD available for: '%s' at '%s'"), $ip, $rrdpath));
}
if ($type != 'host' && $type != 'net') { // beautify in case of "global_admin"
    $hostname = ucfirst(str_replace('_', ' ', $hostname));
}
$params = "graph $tmpfile " .
         "-s $start -e $end " .
         "-t '$hostname "._("Metrics")."' " .
         "--font TITLE:12:$font --font AXIS:7:$font " .
         "-r --zoom $zoom ";
if ($type != 'level') {
    $ymax = (int) 2.5 * $threshold_a;
    $ymin = -1 * (int)(2.5 * $threshold_c);
    $params .= "-u $ymax -l $ymin ";
}
$params .=
         "DEF:obs=$rrdpath/$ip.rrd:$ds:AVERAGE " .
         "DEF:obs2=$rrdpath/$ip.rrd:ds1:AVERAGE " .
         "CDEF:negcomp=0,obs,- " .
         "AREA:obs2$color2:" . _("Attack") . " " .
         "AREA:negcomp$color1:" . _("Compromise") . " " .
         "HRULE:$threshold_a#000000 " .
         "HRULE:-$threshold_c#000000 ";

         
exec("$rrdtool_bin $params 2>&1", $output, $exit_code);
if (preg_match('/^ERROR/i', $output[0]) || $exit_code != 0) {
    mydie(sprintf(_("rrdtool cmd failed with error: '%s' (exit code: %s)"), $output[0], $exit_code));
}
//
// Output generated image
//
if (!$fp = @fopen($tmpfile, 'r')) {
    mydie(sprintf(_("Could not read rrdtool created image: '%s'"), $tmpfile));
}
header("Content-Type: image/png");
header("Content-Length: " . filesize($tmpfile));
fpassthru($fp);
fclose($fp);

?>
