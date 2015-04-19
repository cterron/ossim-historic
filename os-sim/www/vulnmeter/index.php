<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>

<html>
<head>
  <title> Vulnmeter </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/riskmeter.css"/>
</head>

<body>
<?php
    $host = $_GET["host"];
    if ($host) {
        echo "<h1 align=\"center\">Vulnmeter - $host</h1>";
    } else {
        echo "<h1 align=\"center\">Vulnmeter</h1>";
    }
?>


<?php
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Host_vulnerability.inc');
require_once ('classes/Net_vulnerability.inc');
require_once ('classes/Host.inc');

$db = new ossim_db();
$conn = $db->connect();

$BAR_LENGTH_LEFT = 300;
$BAR_LENGTH_RIGHT = 200;
$BAR_LENGTH = $BAR_LENGTH_LEFT + $BAR_LENGTH_RIGHT;


/* 
 * Nets
 */

$net_list = Net_vulnerability::get_list
        ($conn, "", "ORDER BY vulnerability DESC");

$max_level = ossim_db::max_val($conn, "vulnerability", "net_vulnerability");
?>
<table align="center">
    <tr>
<!--      <td colspan="2"><a
href="http://www.nessus.org/demo/report.html">Last scan</a></td> -->
<?php
$today = getdate();
$mon = $today[mon];
$mday = $today[mday];
if($mon < 10){ $mon = 0 . $mon; }
if($mday < 10){ $mday = 0 . $mday; }
$datedir = $today[year] . $mon . $mday;
?> 
<td><a href="last/index.html">Last scan</a></td>
<td> / <a href="do_nessus.php">Update scan</a></td>
</tr>
</table>

<br/>

<table align="center">
<?php
if ($net_list) {
?>
<tr><th colspan="2">Nets</th></tr>
<?php
    foreach ($net_list as $stat) {

    $net = $stat->get_net();

    /* calculate proportional bar width */
    if(!$max_level) $max_level = 1;
    $width = ((($vulnerability = $stat->get_vulnerability()) * 
                   $BAR_LENGTH) / $max_level);
?>
    <tr>
      <td align="center">
           <?php echo $net ?>
      </td>

      <td>
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
} /* if ($net_list) */


/* 
 * Hosts
 */

$ip_list = Host_vulnerability::get_list
        ($conn, "", "ORDER BY vulnerability DESC");

$max_level = ossim_db::max_val($conn, "vulnerability", "host_vulnerability");

if ($ip_list) {
?>

<tr><th colspan="2">Hosts</th></tr>

<?php
    foreach ($ip_list as $stat) {
    
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
        <a href="<?php 
            echo "last/" . $ip_ ?>/index.html">
<?php
    if (!strcmp($ip,$host))
        echo "<font color=\"red\">$ip</font>";
    else
        echo $ip;
?>
         </a>
      </td>

      <td>
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
    if (!$_GET["noimages"]) {

$conf = new ossim_conf();
$vmeter_dir = $conf->get_conf("base_dir") . "/vulnmeter/last/";

// Show only the non-empty GIF charts reported by Nessus

if ($handle = opendir('last')) {
    while (false !== ($file = readdir($handle))) {
   if (($file != ".") && ($file != "..") && (filesize($vmeter_dir.$file) > 0)){
            if (eregi("(.gif)$",$file)){
                echo "</br><table align=\"center\">";
                echo "  <tr>";
                echo "    <td><img echo \"src=\"last/$file\"></td>";
                echo "  </tr>";
                echo "</table>";
            }
        }
    }
    closedir($handle);
}

} // if (!$_GET["noimages"])
?>

<br/>

</body>
</html>

