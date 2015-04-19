<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>


<html>
<head>
  <title> <?php echo gettext("Vulnmeter"); ?> </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>
<?php

// Number of hosts to show if no date or network is selected.
$num_hosts = 20;
// Aging color information
$medium_age = 15;
$max_age = 30;

require_once ('classes/Security.inc');

    $host = REQUEST('host');
    $net = REQUEST('net');
    $scan_date = REQUEST('scan_date');
    $num = REQUEST('num');

    ossim_valid($host, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Host"));
    ossim_valid($net, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Net"));
    ossim_valid($scan_date, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Scan date"));
    ossim_valid($num, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Number"));

    if (ossim_error()) {
        die(ossim_error());
    } 


    if (empty($num)) {
        $num = 10;
    }

    if(file_exists("last")){
    $last = basename(readlink("last"));
    } else {
    ?>
    <center>
    <?=_("No scans have been issued yet") . "."?><br>
    <a href="do_nessus.php?interactive=yes"> <?php echo gettext("Please update scan") . "."; ?> </a>
    </center>
    <?php
    exit();
    }
    if ($host) {
        echo "<h1 align=\"center\">".gettext("Vulnmeter")." - $host</h1>";
    } else {
        echo "<h1 align=\"center\">".gettext("Vulnmeter")."</h1>";
    }

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Host_vulnerability.inc');
require_once ('classes/Net_vulnerability.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Util.inc');

$db = new ossim_db();
$conn = $db->connect();

if(!$scan_date){
// Show all networks
} else {
// Was the scan complete ?
        if(!file_exists($scan_date)){
        $scan_date = $last;
        }
        if(!Host_vulnerability::scan_exists($conn, $scan_date)){
        echo _("Could not find database information for a scan happening at the specified\ndate") . " : <b>" . Util::timestamp2date($scan_date) . "</b>.<br>" . _("Exiting") . ".";
	$db->close($conn);
        exit();
        }
}

$BAR_LENGTH_LEFT = 300;
$BAR_LENGTH_RIGHT = 200;
$BAR_LENGTH = $BAR_LENGTH_LEFT + $BAR_LENGTH_RIGHT;

/*

Accepts an item '$what' and returns a colorized version depending on age, compared to second arg '$date'.
Default color values:
- Older than 2 week: orange.
- Older than 1 month: red.

*/

function colorize_item($what, $date){

global $medium_age;
global $max_age;

$now = time();

$date2 = strtotime(Util::timestamp2date($date));

$diff_days = Util::date_diff($now, $date2, 'd');

if($diff_days >= 0 && $diff_days < $medium_age ){
return $what;
} elseif ($diff_days >= $medium_age && $diff_days < $max_age) {
return "<font color=\"orange\"> $what </font>";
} elseif ($diff_days >= $max_age){
return "<font color=\"red\"> $what </font>";
}

// fallthrough
return $what;

}

/* 
 * Nets
 */

if(!$scan_date){
// As said, show all networks
      $net_list = Net_vulnerability::get_list
        ($conn, "", "ORDER BY vulnerability DESC", $aggregated = true);
      
      $max_level = 0;

      foreach($net_list as $net_vuln){
              $max_level = $net_vuln->get_vulnerability() > $max_level ? $net_vuln->get_vulnerability() : $max_level;
      }
} else {
      $net_list = Net_vulnerability::get_list
        ($conn, "WHERE scan_date = '$scan_date'", "ORDER BY vulnerability DESC");

      $max_level = ossim_db::max_val($conn, "vulnerability", "net_vulnerability", "WHERE scan_date = '$scan_date'");
}

?> 
<center>
[
<a href="<?php echo $last; ?>/index.html"> <?php echo gettext("Last scan"); ?> </a> |
 <a href="report.php"> <?php echo gettext("Reports"); ?> </a>|
 <a href="do_nessus.php?interactive=yes"> <?php echo gettext("Update scan"); ?> </a>|
 <a href="<?php echo $_SERVER["PHP_SELF"]; ?>"><?= _("Show aggregated scans"); ?> </a>|
 <a href="scheduler.php"> <?php echo gettext("Schedule scans"); ?> </a>|
 <a href="index.php"> <?php echo gettext("Back"); ?> </a>]
</center>
<br><br>
<center>
<?php
if($scan_date){
?>
<h3><?php echo _("Showing date:") . " " . Util::timestamp2date($scan_date); ?></h3>
<?php
} else {
?>
<h3><?php echo _("Showing aggregated scans"); ?></h3>
<?php
}
?>
<table border="0">
<tr>
<td colspan="7" border="0">
<h2><?php echo _("Last") . " " . $num . " " . _("scans"); ?> </h2>
</td>
</tr>
<?php

$i = 0;

// Previous scans
if ($handle = @opendir('.')) {
    while (false !== ($file = readdir($handle))) {
   // We'll be prune to the "y3k" issue but I don't care
   if ((is_dir($file)) && !(strncmp($file,"2",1)) && (strlen($file) == 14)){
        // Skip broken dirs. index.html should be present at least
        if(!file_exists($file . "/index.html")) continue;
        $folders[$i] = $file;
        $i++;
        }
    }
    closedir($handle);
} 

$net_vuln_array = array();

if($net){
	$net_list = Net_vulnerability::get_list
        ($conn, "WHERE net = '$net'", "");
	foreach($net_list as $net_vuln){
		array_push($net_vuln_array, $net_vuln->get_scan_date());
	}
}

if(is_array($folders)){
rsort($folders);
}
if($num > count($folders)) $num = count($folders);
for($i=0;$i<$num;$i++){
$file = $folders[$i];
if($file == "") continue;
if($net && !in_array(Util::timestamp2date($file), $net_vuln_array)) continue;

$add_net_tag = "";

if($net)
    $add_net_tag = "&net=" . $net;

?>
<tr>
<td border="0">* <a href="<?php echo $_SERVER["PHP_SELF"] . "?scan_date=$file" . $add_net_tag ?>"><?php echo colorize_item(Util::timestamp2date($file), $file);?> </a></td>
<td border="0"> <a href="<?php echo $file . "/";?>"><?php echo _("Show");?> </a></td>
<td border="0"> <a href="handle_scan.php?action=delete&scan_date=<?php echo $file; ?>"> <?php echo gettext("Delete"); ?> </a></td>
<td border="0"> <a href="handle_scan.php?action=archive&scan_date=<?php echo $file; ?>"> <?php echo gettext("Archive"); ?> </a></td>
<td width="20%"><?= _("Scanned Hosts") . ": " . str_pad(Host_vulnerability::get_scanned_hosts($conn, $file), 3, "0", STR_PAD_LEFT) ?> <td>
<td width="20%"><?= _("Scanned Nets") . ": " . str_pad(Net_vulnerability::get_scanned_nets($conn, $file), 2, "0", STR_PAD_LEFT) ?> <td>
</tr>
<?
}

?>

</table>
</center>
<?= _("Coloring information") ?>:
<ul>
<li> <?= _("Scans older than") . " <font color=\"orange\">" . $medium_age . " " . _("days") ?></font>
<li> <?= _("Scans older than") . " <font color=\"red\">" . $max_age . " " . _("days") ?></font>
</ul>
<br/>
<table align="center">
<?php

if (!$net && $net_list) {
?>
<tr><th colspan="2"> <?php echo gettext("Nets"); ?> </th></tr>
<?php
    foreach ($net_list as $stat) {

    $net_stat = $stat->get_net();
    if($stat->get_vulnerability() <= 1) continue;
    

    /* calculate proportional bar width */
    if(!$max_level) $max_level = 1;
    $width = ((($vulnerability = $stat->get_vulnerability()) * 
                   $BAR_LENGTH) / $max_level);
?>
    <tr>
      <td align="center">
           <a href="<?php echo $_SERVER["PHP_SELF"] .  "?net=$net_stat" ?>"><?php echo colorize_item($net_stat, $stat->get_scan_date()); ?></a>
      </td>

      <td class="left">
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $width ?>"
             title="<?php echo $vulnerability ?>">

<?php 
        echo $vulnerability;
    } /* foreach */
?>

      </td>
    </tr>
<br/>

<?php
} /* if (!net && $net_list) */


/* 
 * Hosts
 */
if ($net) {

    $net_name = $net;
    
    if ($net_list = Net::get_list($conn, "WHERE name = '$net_name'"))
    {
        $ips = $net_list[0]->get_ips();
        print "<h1>" . ucfirst($net_name) . " ($ips)</h1>";


    	if($scan_date){
        if ($ip_list = Host_vulnerability::get_list($conn, "WHERE scan_date = '$scan_date'", "ORDER BY vulnerability DESC", $aggregated = false))
        {
            foreach ($ip_list as $host_vuln)
            {
                if (Net::isIpInNet($host_vuln->get_ip(), $ips))
                {
                    $ip_stats[] = new Host_vulnerability
                                    ($host_vuln->get_ip(),
                                     $host_vuln->get_scan_date(),
                                     $host_vuln->get_vulnerability());
                }
            }
        }
	} else { // if scan_date
        if ($ip_list = Host_vulnerability::get_list($conn, "", "ORDER BY vulnerability DESC", $aggregated = true))
        {
            foreach ($ip_list as $host_vuln)
            {
                if (Net::isIpInNet($host_vuln->get_ip(), $ips))
                {
                    $ip_stats[] = new Host_vulnerability
                                    ($host_vuln->get_ip(),
                                     $host_vuln->get_scan_date(),
                                     $host_vuln->get_vulnerability());
                }
            }
        }
	}
    }

    
} else {
    if($scan_date){
    $ip_stats = Host_vulnerability::get_list ($conn, "WHERE scan_date = '$scan_date'", "ORDER BY vulnerability DESC", $aggregated = false);
        foreach($ip_stats as $host_vuln){
             $max_level = $host_vuln->get_vulnerability() > $max_level ? $host_vuln->get_vulnerability() : $max_level;
        }
    } else {
    $ip_stats = Host_vulnerability::get_list ($conn, "", "ORDER BY vulnerability DESC", $ggregated = true, $num_hosts);
        foreach($ip_stats as $host_vuln){
             $max_level = $host_vuln->get_vulnerability() > $max_level ? $host_vuln->get_vulnerability() : $max_level;
        }
    }
}

if ($ip_stats) {
?>

<tr><th colspan="2"><?php echo gettext("Hosts"); ?></th></tr>

<?php
    foreach ($ip_stats as $stat) {
    
        $ip = $stat->get_ip();

        /* replace . -> _ for nessus links */
        $ip_ = ereg_replace("\.","_",$ip);

        /* calculate proportional bar width */
        if(!$max_level) $max_level = 1;
        $width = ((($vulnerability = $stat->get_vulnerability()) * 
                   $BAR_LENGTH) / $max_level);
?>

    <tr>
      <td align="center">
        <a href="<?php echo date("YmdHis", strtotime($stat->get_scan_date())) . "/$ip_"; ?>/index.html">
<?php
    if (!strcmp($ip,$host))
        echo "<font color=\"red\">".Host::ip2hostname($conn, $ip)."</font>";
    else
        echo colorize_item(Host::ip2hostname($conn, $ip), $stat->get_scan_date());
?>
         </a>
      </td>

      <td class="left">
<?php
    if (!strcmp($ip,$host))
        $bar = "../pixmaps/solid-red.jpg";
    else
        $bar = "../pixmaps/solid-blue.jpg";
?>
        <img src="<?php echo $bar ?>" height="8" 
             width="<?php echo $width ?>"
             title="<?php echo $vulnerability ?>">
<?php 
        echo $vulnerability;
    } /* foreach */
?>
      </td>
    </tr>
<?php
} /* if ($ip_stats) */
?>
    <!-- end C & A levels for each IP -->
</table>
<br/>
<center><?= _("Showing top") . " " . $num_hosts . " " . _("hosts") ?></center>

<?php 
// Only show images if a specific date has been issued.
if($scan_date){
    if (!GET('noimages')) {

$conf = $GLOBALS["CONF"];
$vmeter_dir = $conf->get_conf("base_dir") . "/vulnmeter/$scan_date/";

// Show only the non-empty GIF charts reported by Nessus

if ($handle = @opendir($scan_date)) {
    while (false !== ($file = readdir($handle))) {
   if (($file != ".") && ($file != "..") && (@filesize($vmeter_dir.$file) > 0)){
            if (eregi("(.gif)$",$file)){
                echo "<br/><table align=\"center\">";
                echo "  <tr>";
                echo "    <td><img src=\"$scan_date/$file\"></td>";
                echo "  </tr>";
                echo "</table>";
            }
        }
    }
    closedir($handle);
} else {
    echo "<br/>" . gettext("No scans have been done yet") . ".<br/>";
}

} // if (!GET("noimages"))
}
$db->close($conn);
?>

<br/>

</body>
</html>
