<?php


require_once ('ossim_conf.inc');
$conf = new ossim_conf();
$cgi_link = $conf->get_conf("graph_link");

function graph_image_link($ip, $type, $what, $start, $end, $zoom) {
    global $cgi_link;
    return "$cgi_link?ip=$ip&what=$what&start=$start&end=$end&type=$type&zoom=$zoom";
}

function echocolor ($val, $max)
{
    if ($val - $max > 0) {
    echo "<font color=\"red\">$val</font>";
    } else {
    echo $val;
    }
}

?>
