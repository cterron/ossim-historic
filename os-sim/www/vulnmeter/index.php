<html>
<head>
  <title> Vulmeter </title>
<!--  <meta http-equiv="refresh" content="3"> -->
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/riskmeter.css"/>
</head>

<body>

  <h1 align="center">OSSIM Framework</h1>
  <h2 align="center">Vulnmeter</h2>

<?php
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Conf.inc');
require_once ('classes/Host_vulnerability.inc');
require_once ('classes/Net_vulnerability.inc');
require_once ('classes/Host.inc');

$db = new ossim_db();
$conn = $db->connect();

$conf = Conf::get_conf($conn);
$BAR_LENGTH_LEFT = $conf->get_bar_length_left();
$BAR_LENGTH_RIGHT = $conf->get_bar_length_right();
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
      <td colspan="2"><a href="<?php echo $datedir?>/index.html">Last scan</a></td>
    </tr>
</table>
<table align="center">
<tr><th colspan="2">Nets</th></tr>
<?php

if ($net_list) {
    foreach ($net_list as $stat) {
    
        $net = $stat->get_net();

        /* calculate proportional bar width */
        $width = ((($vulnerability = $stat->get_vulnerability()) * 
                   $BAR_LENGTH) / $max_level);
?>

    <tr>
      <td align="center">
           <?php echo $net ?></a>
      </td>

      <td>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $width ?>"
             title="<?php echo $vulnerability ?>">
<?php 
        echo $vulnerability;
    } /* foreach */
} /* if */
?>
      </td>
    </tr>

<?php

/* 
 * Hosts
 */

$ip_list = Host_vulnerability::get_list
        ($conn, "", "ORDER BY vulnerability DESC");

$max_level = ossim_db::max_val($conn, "vulnerability", "host_vulnerability");

?>
<br/>
<tr><td colspan="2"></td></tr>
<tr><td colspan="2"></td></tr>
<tr><td colspan="2"></td></tr>
<tr><th colspan="2">Hosts</th></tr>
<?php

if ($ip_list) {
    foreach ($ip_list as $stat) {
    
        $ip = $stat->get_ip();

        /* replace . -> _ for nessus links */
        $ip_ = ereg_replace("\.","_",$ip);

        /* calculate proportional bar width */
        $width = ((($vulnerability = $stat->get_vulnerability()) * 
                   $BAR_LENGTH) / $max_level);
?>

    <!-- C & A levels for each IP -->
    <tr>
      <td align="center">
        <a href="<?php 
            echo $datedir . "/" . $ip_ ?>/index.html"><?php echo $ip ?></a>
      </td>

      <td>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $width ?>"
             title="<?php echo $vulnerability ?>">
<?php 
        echo $vulnerability;
    } /* foreach */
} /* if */
?>
      </td>
    </tr>
    <!-- end C & A levels for each IP -->
    
</table>

<br/>
<table align="center">
  <tr>
    <td><img
src="<?php echo $datedir; ?>/chart_dangerous_services.gif"></td>
  </tr>
</table>
<br/>
<table align="center">
  <tr>
    <td><img
src="<?php echo $datedir; ?>/chart_services_occurences.gif"></td>
  </tr>
</table>
<br/>
<table align="center">
  <tr>
    <td><img src="<?php echo $datedir; ?>/pie_risks.gif"></td>
  </tr>
</table>
<br/>
<table align="center">
  <tr>
    <td><img src="<?php echo $datedir; ?>/pie_most.gif"></td>
  </tr>
</table>
<br/>

</body>
</html>

