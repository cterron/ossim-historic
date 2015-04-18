<?php


require_once ('ossim_conf.inc');
$conf = new ossim_conf();

function graph_image_link($ip, $type, $what, $start, $end, $zoom, $range) {
    $cgi_link = "show_image.php";
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


function echocolor ($val, $max, $image)
{
    if ($val / $max > 5) {
        echo "<td bgcolor=\"red\"><a href=\"" . $image . "\"><font color=\"white\"><b>" .  $val .  "</b></font></a></td>";
    } elseif ($val / $max > 3) {
        echo "<td bgcolor=\"orange\"><a href=\"" . $image . "\"><font color=\"black\"><b>" .  $val .  "</b></font></a></td>";
    } elseif ($val / $max > 1) {
        echo "<td bgcolor=\"green\"><a href=\"" . $image . "\"><font color=\"white\"><b>" .  $val .  "</b></font></a></td>";
    } else {
        echo "<td><a href=\"" . $image . "\"><font color=\"black\"><b>" .  $val . "</b></font></a></td>";
    }
}

?>
