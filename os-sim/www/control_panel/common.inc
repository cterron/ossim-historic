<?php


require_once ('ossim_conf.inc');
$conf = new ossim_conf();


function timestamp2date($timestamp)
{
    # YYYY-MM-DD HH:MM:SS
    return (substr($timestamp, 0, 4)  . '-' .  # YYYY
            substr($timestamp, 4, 2)  . '-' .  # MM
            substr($timestamp, 6, 2)  . ' ' .  # DD
            substr($timestamp, 8, 2)  . ':' .  # HH
            substr($timestamp, 10, 2) . ':' .  # MM
            substr($timestamp, 12, 2));        # SS
}


/*
 *  PRE: $date must be in format YYYY-MM-DD HH-MM
 *       $target must be ip_src or ip_dst
 */
function get_acid_date_link($date, $ip = "", $target = "")
{

    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $acid_link = $conf->get_conf("acid_link");
    
    $pattern = "/(\d+)-(\d+)-(\d+) (\d+):(\d+)/";
    
    /*
     * regs[1] => year
     * regs[2] => month
     * regs[3] => day
     * regs[4] => hour
     * regs[5] => minute
     */
    preg_match($pattern, $date, $regs);

    /* 10 minutes before */
    if ($regs[5] >= 10) {            // 3:45 -> 3:35
        $regs[5] -= 10;
    } else {
        $regs[5] = 60 - (10 - $regs[5]);
        if ($regs[4] > 0) {         // 3:06 -> 2:56
            $regs[4] -= 1;
        } else {                    // 0:07 -> 23:57
            $regs[4] = 23;
        }
    }

    $link = "$acid_link" . 
        "/acid_qry_main.php?new=1&time[0][1]=>=&time[0][2]=" . $regs[2] .
        "&time[0][3]=" . $regs[3] .
        "&time[0][4]=" . $regs[1] .
        "&time[0][5]=" . $regs[4] .
        "&time[0][6]=" . $regs[5];
        
    if ($target) {
        $link .= "&ip_addr[0][1]=$target&ip_addr[0][2]==&ip_addr[0][3]=$ip&";
    }

    $link .= "&sort_order=time_a&".
             "submit=Query+DB&num_result_rows=-1&time_cnt=1&ip_addr_cnt=1";

    return $link;
}


/*
 *  PRE: 
 *     $since and $last must be in format YYYY-MM-DD HH-MM-SS
 *     $desc must be "time_d" or "time_a"
 */
function get_acid_alerts_link($since, $last, $order = "time_d")
{

    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $acid_link = $conf->get_conf("acid_link");
    
    $pattern = "/(\d+)-(\d+)-(\d+) (\d+):(\d+):(\d+)/";
    
    /*
     * regs[1] => year
     * regs[2] => month
     * regs[3] => day
     * regs[4] => hour
     * regs[5] => minute
     * regs[6] => secs
     */
    preg_match($pattern, $since, $regs_since);
    preg_match($pattern, $last, $regs_last);

    $link = "$acid_link" . 
        "/acid_qry_main.php?new=1" . 
        "&time[0][0]=(" .
        "&time[0][1]=>=" . 
        "&time[0][2]=" . $regs_since[2] .
        "&time[0][3]=" . $regs_since[3] .
        "&time[0][4]=" . $regs_since[1] .
        "&time[0][5]=" . $regs_since[4] .
        "&time[0][6]=" . $regs_since[5] .
        "&time[0][7]=" . $regs_since[6] .
        "&time[0][8]=)" . 
        "&time[0][9]=AND" .
        "&time[1][0]=(" .
        "&time[1][1]=<=" . 
        "&time[1][2]=" . $regs_last[2] .
        "&time[1][3]=" . $regs_last[3] .
        "&time[1][4]=" . $regs_last[1] .
        "&time[1][5]=" . $regs_last[4] .
        "&time[1][6]=" . $regs_last[5] .
        "&time[1][7]=" . $regs_last[6] .
        "&time[1][8]=)";

    $link .= "&sort_order=$order&".
             "submit=Query+DB&num_result_rows=-1&time_cnt=2&ip_addr_cnt=1";

    return $link;
}


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
