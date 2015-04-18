<?php


require_once ('ossim_conf.inc');
$conf = new ossim_conf();
$cgi_link = $conf->get_conf("show_image_link");
#$date_link = $conf->get_conf("date_path");

function graph_image_link($ip, $type, $what, $start, $end, $zoom, $range) {
    global $cgi_link;
    return "$cgi_link?range=$range&ip=$ip&what=$what&start=$start&end=$end&type=$type&zoom=$zoom";
}

function get_caption($ip, $type, $what, $range) {
$ip = escapeshellcmd($ip);
$type = escapeshellcmd($type);
$what = escapeshellcmd($what);
$range = escapeshellcmd($range);
if($range == "day"){$range = "1D";}
if($range == "month"){$range = "1M";}
if($range == "year"){$range = "1Y";}
$result = "";

$result = shell_exec("/home/dgil/os-sim/scripts/get_date.pl $ip $range $what $type");
return date("l dS of F Y h:i:s A",$result);
}


function echocolor ($val, $max)
{
    if ($val - $max > 0) {
        echo "<font color=\"#991e1e\">" . 
              $val . "</font>/" . intval($val * 100 / $max) ."%";
    } else {
        echo "$val/" . intval($val * 100 / $max) . "%";
    }
}

?>
