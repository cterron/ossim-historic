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

require_once ('classes/Security.inc');

    $host = GET('host');
    $net = GET('net');
    $scan_date = GET('scan_date');
    $num = GET('num');

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
    <a href="do_nessus.php"> <?php echo gettext("Please update scan") . "."; ?> </a>
    </center>
    <?php
    exit();
    }
    if ($host) {
        echo "<h1 align=\"center\">".gettext("Vulnmeter")." - $host</h1>";
    } else {
        echo "<h1 align=\"center\">".gettext("Vulnmeter")."</h1>";
    }
?>


<?php
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Host_vulnerability.inc');
require_once ('classes/Net_vulnerability.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Util.inc');

$db = new ossim_db();
$conn = $db->connect();

// Was the scan complete ?
if(!file_exists($scan_date)){
    $scan_date = $last;
}
if(!Host_vulnerability::scan_exists($conn, $scan_date)){
    echo _("Could not find database information for a scan happening at the specified\ndate") . " : <b>" . Util::timestamp2date($scan_date) . "</b>.<br>" . _("Exiting") . ".";
    exit();
}

$BAR_LENGTH_LEFT = 300;
$BAR_LENGTH_RIGHT = 200;
$BAR_LENGTH = $BAR_LENGTH_LEFT + $BAR_LENGTH_RIGHT;


/* 
 * Nets
 */

$net_list = Net_vulnerability::get_list
        ($conn, "WHERE scan_date = '$scan_date'", "ORDER BY vulnerability DESC");

$max_level = ossim_db::max_val($conn, "vulnerability", "net_vulnerability", "WHERE scan_date = '$scan_date'");
?>

<?php
$today = getdate();
$mon = $today['mon'];
$mday = $today['mday'];

if($mon < 10){ $mon = 0 . $mon; }
if($mday < 10){ $mday = 0 . $mday; }

$datedir = $today['year'] . $mon . $mday;

?> 
<center>
[
<a href="<?php echo $last; ?>/index.html"> <?php echo gettext("Last scan"); ?> </a> |
 <a href="do_nessus.php"> <?php echo gettext("Update scan"); ?> </a>|
 <a href="index.php"> <?php echo gettext("Back"); ?> </a>]
</center>
<br><br>
<center>
<h3><?php echo _("Showing date:") . " " . Util::timestamp2date($scan_date); ?></h3>
<table border="0">
<tr>
<td colspan="4" border="0">
<h2><?php echo _("Last") . " " . $num . " " . _("scans"); ?> </h2>
</td>
</tr>
<?php

$i = 0;

// Previous scans
if ($handle = @opendir('.')) {
    while (false !== ($file = readdir($handle))) {
   // We'll be prune to the "y3k" issue but I don't care
   if ((is_dir($file)) && !(strncmp($file,"2",1))){
        // Skip broken dirs. index.html should be present at least
        if(!file_exists($file . "/index.html")) continue;
        $folders[$i] = $file;
        $i++;
        }
    }
    closedir($handle);
} 

if(is_array($folders)){
rsort($folders);
}
for($i=0;$i<$num;$i++){
$file = $folders[$i];
if($file == "") continue;
?>
<tr>
<td border="0">* <a href="<?php echo $_SERVER["PHP_SELF"] . "?scan_date=$file" ?>"><?php echo Util::timestamp2date($file);?> </a></td>
<td border="0"> <a href="<?php echo $file . "/";?>"><?php echo _("Show");?> </a></td>
<td border="0"> <a href="handle_scan.php?action=delete&scan_date=<?php echo $file; ?>"> <?php echo gettext("Delete"); ?> </a></td>
<td border="0"> <a href="handle_scan.php?action=archive&scan_date=<?php echo $file; ?>"> <?php echo gettext("Archive"); ?> </a></td>
</tr>
<?
}

?>

</table>
</center>

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
           <a href="<?php echo $_SERVER["PHP_SELF"] .  "?net=$net_stat" ?>"><?php echo $net_stat ?></a>
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


        if ($ip_list = Host_vulnerability::get_list($conn, "WHERE scan_date = '$scan_date'"))
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

    
} else {
    $ip_stats = Host_vulnerability::get_list ($conn, "WHERE scan_date = '$scan_date'", "ORDER BY vulnerability DESC");
}

$max_level = ossim_db::max_val($conn, "vulnerability", "host_vulnerability");

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

    <!-- C & A levels for each IP -->
    <tr>
      <td align="center">
        <a href="<?php echo "$scan_date/$ip_"; ?>/index.html">
<?php
    if (!strcmp($ip,$host))
        echo "<font color=\"red\">$ip</font>";
    else
        echo $ip;
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
} /* if ($ip_list) */
?>
    <!-- end C & A levels for each IP -->
</table>

<?php 
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
?>

<br/>

</body>
</html>

