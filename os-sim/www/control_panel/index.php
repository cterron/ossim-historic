<html>
<head>
  <title> Control Panel </title>
  <meta http-equiv="refresh" content="60">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center">OSSIM Framework</h1>
  <h2 align="center">Control Panel</h2>

<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Control_panel_host.inc');
require_once ('classes/Control_panel_net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Host_mac.inc');
require_once ('acid_funcs.inc');
require_once ('common.inc');

if (!$range = mysql_escape_string($_GET["range"]))  $range = 'day';

/* get conf */
$conf = new ossim_conf();
$mrtg_link = $conf->get_conf("mrtg_link");
$graph_link = $conf->get_conf("graph_link");
$acid_link = $conf->get_conf("acid_link");
$ntop_link = $conf->get_conf("ntop_link");
$opennms_link = $conf->get_conf("opennms_link");
$stats_link = $conf->get_conf("stats_link");
$mailstats_link = $conf->get_conf("mailstats_link");

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

/* get host & net lists */
$hosts_order_by_c = Control_panel_host::get_list($conn, 
            "WHERE time_range = '$range' ORDER BY max_c DESC", 5);
$hosts_order_by_a = Control_panel_host::get_list($conn, 
            "WHERE time_range = '$range' ORDER BY max_a DESC", 5);
$nets_order_by_c = Control_panel_net::get_list($conn, 
            "WHERE time_range = '$range' ORDER BY max_c DESC", 5);
$nets_order_by_a = Control_panel_net::get_list($conn, 
            "WHERE time_range = '$range' ORDER BY max_a DESC", 5);

?>

  <table align="center" width="100%">
    <tr><td colspan="8">
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day">Last Day</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month">Last Month</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year">Last Year</a>]
    </td></tr>
    <tr><td colspan="8">
<?php
        if ($range == 'day') {
            $image2 = "$graph_link?ip=global&what=attack&start=N-24h&end=N&type=global&zoom=0.85";
            $image1 = "$graph_link?ip=global&what=compromise&start=N-24h&end=N&type=global&zoom=0.85";
            $start = "N-1D";
        } elseif ($range == 'month') {
            $image2 = "$graph_link?ip=global&what=attack&start=N-1M&end=N&type=global&zoom=0.85";
            $image1 = "$graph_link?ip=global&what=compromise&start=N-1M&end=N&type=global&zoom=0.85";
            $start = "N-1M";
        } elseif ($range == 'year') {
            $image2 = "$graph_link?ip=global&what=attack&start=N-1Y&end=N&type=global&zoom=0.85";
            $image1 = "$graph_link?ip=global&what=compromise&start=N-1Y&end=N&type=global&zoom=0.85";
            $start = "N-1Y";
        }
?>
      <img src="<?php echo "$image1"; ?>">
      <img src="<?php echo "$image2"; ?>">
    </td></tr>
    <tr><th colspan="8">Compromise and Attack level - Top 5 Hosts</th></tr>
    <tr>
      <th>Host</th>
      <th>Max C</th>
      <th>Min C</th>
      <th>Avg C</th>
      <th>Host</th>
      <th>Max A</th>
      <th>Min A</th>
      <th>Avg A</th>
    </tr>
    <tr>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) { 
        $host_ip = $host->get_host_ip();
?>
          <tr>
            <td><a href="<?php echo get_acid_info($host_ip, 
                                                  $acid_link); ?>">
            <?php echo Host::ip2hostname($conn, $host_ip); ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) {
        $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                                  $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_max_c(), 
                            Host::ipthresh_c($conn,$host->get_host_ip())); ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) { 
        $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                                  $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_min_c(), 
                            Host::ipthresh_c($conn, $host->get_host_ip())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) { 
        $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                                  $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_avg_c(), 
                            Host::ipthresh_c($conn, $host->get_host_ip())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) { 
        $host_ip = $host->get_host_ip();
    ?>
          <tr>
            <td><a href="<?php echo get_acid_info($host_ip, 
                                                  $acid_link); ?>">
            <?php echo Host::ip2hostname($conn, $host_ip); ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) {
        $image = graph_image_link($host->get_host_ip(), "host", "attack",
                                  $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_max_a(), 
                            Host::ipthresh_a($conn, $host->get_host_ip())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) { 
        $image = graph_image_link($host->get_host_ip(), "host", "attack",
                                  $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_min_a(), 
                            Host::ipthresh_a($conn, $host->get_host_ip())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) { 
        $image = graph_image_link($host->get_host_ip(), "host", "attack",
                                  $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($host->get_avg_a(), 
                            Host::ipthresh_a($conn, $host->get_host_ip())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>

    <tr><th colspan="8">Compromise and Attack level - Top 5 Networks</th></tr>
    <tr>
      <th>Network</th>
      <th>Max C</th>
      <th>Min C</th>
      <th>Avg C</th>
      <th>Network</th>
      <th>Max A</th>
      <th>Min A</th>
      <th>Avg A</th>
    </tr>
    <tr>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) { ?>
          <tr>
            <td><?php echo $net->get_net_name(); ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_max_c(), 
                            Net::netthresh_c($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_min_c(), 
                            Net::netthresh_c($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_avg_c(), 
                            Net::netthresh_c($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { ?>
          <tr>
            <td><?php echo $net->get_net_name(); ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_max_a(), 
                            Net::netthresh_a($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_min_a(), 
                            Net::netthresh_a($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>">
            <?php echocolor($net->get_avg_a(), 
                            Net::netthresh_a($conn, $net->get_net_name())) ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>
    </table>
    <br/>
    <table width="100%">
    <tr>
    <td colspan = 8>
    <table align="center" width="100%">
    <tr>
    <th colspan=4><u>RRD anomalies</u> <a name="Anomalies" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#Anomalies" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>
    </th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=1">Acknowledged</A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=0">Not Acknowledged</A></th>
    <th align="center"><A HREF="<?php echo $_SERVER["PHP_SELF"] ?>?acked=-1">All</A></th>
    </tr>
    <tr>
    <th> Host </th><th> What </th><th> When </th>
    <th> Not acked count (hours)</th><th> Over threshold (absolute)</th>
    <th align="center">Ack</th>
    <th align="center">Delete</th>
    </tr>

<form action="handle_anomaly.php" method="GET">
<?php
require_once 'ossim_db.inc';
require_once 'classes/RRD_conf_global.inc';
require_once 'classes/RRD_conf.inc';
require_once 'classes/RRD_anomaly.inc';
require_once 'classes/RRD_anomaly_global.inc';
require_once 'classes/RRD_data.inc';

$db = new ossim_db();
$conn = $db->connect();
$where_clause = "where acked = 0";
switch ($_GET["acked"]){
    case -1:
    $where_clause = "";
    break;
    case 0:
    $where_clause = "where acked = 0";
    break;
    case 1:
    $where_clause = "where acked = 1";
    break;
}

$perl_interval = 4; // Global perl is being executed every 15 minutes

if ($alert_list_global = RRD_anomaly_global::get_list($conn, $where_clause,
"order by anomaly_time desc")) {
    foreach($alert_list_global as $alert) {
    $ip = "Global";
    if($rrd_list_temp = RRD_conf_global::get_list($conn, "")) {
    $rrd_temp = $rrd_list_temp[0];
    }
    if(($alert->get_count() / $perl_interval) <
    ($rrd_temp->get_col($alert->get_what(), "persistence")) && $_GET["acked"] != -1) {
    continue;
    }
?>
<tr>
<th> 

<A HREF="<?php echo
"$ntop_link/plugins/rrdPlugin?action=list&key=interfaces/eth0&title=interface%20eth0";?>" target="_blank"> 
<?php echo $ip;?></A> </th><td> <?php echo $rrd_names_global[$alert->get_what()];?></td>
<td> <?php echo $alert->get_anomaly_time();?></td>
<td> <?php echo ($alert->get_count())/$perl_interval;?> </td>
<td><font color="red"><?php echo ($alert->get_over()/$rrd_temp->get_col($alert->get_what(),"threshold"))*100;?>%</font>/<?php echo $alert->get_over();?></td>
<td align="center"><input type="checkbox" name="ack,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
<td align="center"><input type="checkbox" name="del,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
</tr>
<?php }}

?>
<tr><th colspan="8"><hr noshade></th></tr>
<tr>
<th> Host </th><th> What </th><th> When </th>
<th> Not acked count (hours)</th><th> Over threshold (absolute)</th>
<th align="center">Ack</th>
<th align="center">Delete</th>
</tr>
<?php

$perl_interval = 4; // Host perl is being executed every 15 minutes
if ($alert_list = RRD_anomaly::get_list($conn, $where_clause, "order by
anomaly_time desc")) {
    foreach($alert_list as $alert) {
    $ip = $alert->get_ip();
    if($rrd_list_temp = RRD_conf::get_list($conn, "where ip = '$ip'")) {
    $rrd_temp = $rrd_list_temp[0];
    }
    if(($alert->get_count() / $perl_interval) < ($rrd_temp->get_col($alert->get_what(), "persistence")) && $_GET["acked"] != -1) {
    continue;
    }


?>
<tr>
<th>
<A HREF="<?php echo "$ntop_link/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A></th><td> <?php echo $rrd_names[$alert->get_what()];?></td>
<td> <?php echo $alert->get_anomaly_time();?></td>
<td> <?php echo ($alert->get_count())/$perl_interval;?> </td>
<td><font color="red"><?php echo ($alert->get_over()/$rrd_temp->get_col($alert->get_what(),"threshold"))*100;?>%</font>/<?php echo $alert->get_over();?></td>
<td align="center"><input type="checkbox" name="ack,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
<td align="center"><input type="checkbox" name="del,<?php echo $ip?>,<?php
echo $alert->get_what();?>"></input></td>
</tr>
<?php }}?>
<tr>
<td align="center" colspan="7">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    </table>
    <br/>
    <table width="100%">
    <tr>
    <td colspan="8">
    <table width="100%">
    <tr><th colspan="6"><u>OS Changes</u> <a name="OS" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#OS" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>  
        &nbsp;&nbsp;[ <a href="os.php" target="_blank"> Get list </a> ]
    </th></tr>
    <tr><th> Host </th><th colspan="1"> OS </th><th colspan="1"> Previous OS
    </th><th> When </th><th> Ack </th><th> Ignore </th></tr>
<form action="handle_os.php" method="GET">
<?php
if ($host_os_list = Host_os::get_list($conn, "where anom = 1", "")) {
    foreach($host_os_list as $host_os) {
    $ip = $host_os->get_ip();
    $os_time = $host_os->get_os_time();
    $os = $host_os->get_os();
    if(ereg("\|",$os)){
    $os = ereg_replace("\|", " or ", $os);
    }
    $previous = $host_os->get_previous();
    if(ereg("\|",$previous)){
    $previous = ereg_replace("\|", " or ", $previous);
    }
    
    ?>

<tr><th>
<A HREF="<?php echo "$ntop_link/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A>
</th>
<td colspan="1"><font color="red"><?php echo $os;?></font></td>
<td colspan="1"><?php echo $previous;?></td>
<td colspan="1"><?php echo $os_time;?></td>
<td>
<?php $encoded = base64_encode("ack" . $os);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>
<td>
<?php $encoded = base64_encode("ignore" . $previous);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>

<?php
}}
?>
<tr>
<td align="center" colspan="6">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    </table>
    <br/>

    <!-- Mac detection -->
    <table width="100%">
    <tr>
    <td colspan="8">
    <table width="100%">
    <tr><th colspan="6"><u>Mac Changes</u> <a name="Mac" 
        href="<?php echo $_SERVER["PHP_SELF"]?>?#Mac" title="Fix"><img
        src="../pixmaps/Hammer2.png" width="24" border="0"></a>  
        &nbsp;&nbsp;[ <a href="mac.php" target="_blank"> Get list </a> ]
    </th></tr>
    <tr><th> Host </th><th colspan="1"> Mac </th><th colspan="1"> Previous Mac
    </th><th> When </th><th> Ack </th><th> Ignore </th></tr>
<form action="handle_mac.php" method="GET">
<?php
if ($host_mac_list = Host_mac::get_list($conn, "where anom = 1", "")) {
    foreach($host_mac_list as $host_mac) {
    $ip = $host_mac->get_ip();
    $mac_time = $host_mac->get_mac_time();
    $mac = $host_mac->get_mac();
    if(ereg("\|",$mac)){
    $mac = ereg_replace("\|", " or ", $mac);
    }
    $previous = $host_mac->get_previous();
    if(ereg("\|",$previous)){
    $previous = ereg_replace("\|", " or ", $previous);
    }
    
    ?>

<tr><th>
<A HREF="<?php echo "$ntop_link/$ip.html";?>" target="_blank" title="<?php
echo $ip;?>">
<?php echo Host::ip2hostname($conn, $ip);?></A>
</th>
<td colspan="1"><font color="red"><?php echo $mac;?></font></td>
<td colspan="1"><?php echo $previous;?></td>
<td colspan="1"><?php echo $mac_time;?></td>
<td>
<?php $encoded = base64_encode("ack" . $mac);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>
<td>
<?php $encoded = base64_encode("ignore" . $previous);?>
<input type="checkbox" name="ip,<?php echo $ip;?>" value="<?php echo
$encoded;?>"></input>
</td>


<?php
}}
?>
<tr>
<td align="center" colspan="6">
<input type="submit" value="OK">
<input type="reset" value="reset">
</td>
</tr>
</form>
    </table>
    </td>
    </tr>
    <!-- end Mac detection -->


</table>

<p>&nbsp;</p>

  <!-- static code -->
<center><h3> Static code. work in progress...</h3></center>
 <table align="center">
  <tr>
    <th colspan="2"></th>
    <th colspan="2">Transmitted</th>
    <th colspan="2">Throughput</th>
  </tr>
  <tr>
    <td align="center" colspan="2"></td>
    <td align="center">Total</td>
    <td align="center">%Avg</td>
    <td align="center">Total</td>
    <td align="center">%Avg</td>
  </tr>
  <tr>
    <td align="center" colspan="2">Internet</td>
    <td align="center">23</td>
    <td align="center">30%</td>
    <td align="center">1,2</td>
    <td align="center">15%</td>
  </tr>
  <tr>
    <td align="center" colspan="2">DMZ</td>
    <td align="center">
      <a href="<?php echo $ntop_link?>/IpL2R.html"><font color="red">46</font></a></td>
    <td align="center">
      <a href="<?php echo $ntop_link?>/IpL2R.html"><font color="red">400%</font></a></td>
    <td align="center">
      <a href="<?php echo $ntop_link?>/thptStats.html"><font color="red">9,3</font></a></td>
    <td align="center">
      <a href="<?php echo $ntop_link?>/thptStats.html"><font color="red">200%</font></a></td>
  </tr>
  <tr>
    <td align="center" colspan="2">Internal</td>
    <td align="center">459</td>
    <td align="center">-20%</td>
    <td align="center">60</td>
    <td align="center">-10%</td>
  </tr>
 


 
  <tr><th colspan="6" bgcolor="silver">Services</th></tr>
  <tr>
    <th colspan="2"></th>
    <th colspan="2">Latency (seg)</th>
    <th colspan="2">RTT (ms)</th>
  </tr>
  <tr>
    <td align="center">Host</td>
    <td align="center">Protocol</td>
    <td align="center">Max</td>
    <td align="center">%Avg</td>
    <td align="center">Max</td>
    <td align="center">%Avg</td>
  </tr>
  <tr>
    <td align="center">www.ipsoluciones.com</td>
    <td align="center">http</td>
    <td align="center">
      <a href="<?php echo $stats_link?>/stats/web/www.ipsoluciones.com.html">5,6</a></td>
    <td align="center">2,30%</td>
    <td align="center">
      <a href="<?php echo $stats_link?>/stats/ping/www.ipsoluciones.com.html">5</a></td>
    <td align="center">-20%</td>
  </tr>
  <tr>
    <td align="center">script.ipsoluciones.com</td>
    <td align="center">http</td>
    <td align="center">9,2</td>
    <td align="center">10%</td>
    <td align="center">4</td>
    <td align="center">-10%</td>
  </tr>
  <tr>
    <td align="center">mail.ipsoluciones.com</td>
    <td align="center">smtp</td>
    <td align="center"><a href="<?php echo $stats_link?>/stats/smtp/mail.ipsoluciones.com.html">22,3</a></td>
    <td align="center"><a href="<?php echo $stats_link?>/stats/smtp/mail.ipsoluciones.com.html">89,20%</a></td>
    <td align="center">3</td>
    <td align="center">8%</td>
  </tr>
  <tr>
    <td align="center">pop.ipsoluciones.com</td>
    <td align="center">pop</td>
    <td align="center">1,3</td>
    <td align="center">-6%</td>
    <td align="center">4</td>
    <td align="center">9%</td>
  </tr>
  <tr>
    <td align="center">ftp.ipsoluciones.com</td>
    <td align="center">ftp</td>
    <td align="center">
      <a href="<?php echo $stats_link?>/stats/ftp/ftp.ipsoluciones.com.html">2,3</a></td>
    <td align="center">9,20%</td>
    <td align="center">4</td>
    <td align="center">8%</td>
  </tr>

  <tr><th colspan="6" bgcolor="silver">Transactions</th></tr>
  <tr>
    <th colspan="2">Tipo</th>
    <th colspan="2">Total</th>
    <th colspan="2">%Average</th>
  </tr>
  <tr>
    <td align="center" colspan="2"><font color="blue">web</font></td>
    <td align="center" colspan="2"><font color="blue">1400</font></td>
    <td align="center" colspan="2"><font color="blue">30%</font></td>
  </tr>
  <tr>
    <td align="center" colspan="2"><font color="blue">mail</font></td>
    <td align="center" colspan="2"><font color="red"><A HREF="<?php echo $mailstats_link?>/mailscanner-mrtg/mail/mail.html"><font color="red" >1623</font></A></font></td>
    <td align="center" colspan="2"><font color="red">140%</font></td>
  </tr>
  <tr>
    <td align="center" colspan="2"><font color="blue">virus</font></td>
    <td align="center" colspan="2"><font color="red"><A HREF="<?php echo $mailstats_link?>/mailscanner-mrtg/virus/virus.html"><font color="red">40</font></A></font></td>
    <td align="center" colspan="2"><font color="red">102%</font></td>
  </tr>
<FORM name="temp" action="">
  <tr><th colspan="6" bgcolor="silver">Profile anomalies</th></tr>
  <tr>
    <td colspan="3" align="center">Show anomalies</td>
    <td align="center"><A HREF="">Acknowledged</A></td>
    <td align="center"><A HREF="">Not Acknowledged</A></td>
    <td align="center"><A HREF="">All</A></td>
  </tr>
  <tr>
    <th bgcolor="silver"> Date </th>
    <th colspan="2" bgcolor="silver"> System </th>
    <th colspan="2" bgcolor="silver"> Anomaly </th>
    <th bgcolor="silver"> Ack </th>
  </tr>
  <tr>
    <td align="center"> Jul-03 16:35 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.97">golgotha</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $ntop_link?>/sortDataThpt.html?col=3">
          <font color="red"><u>Over 600% traffic transmitted</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jul-03 04:10 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.203">vixen</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $ntop_link?>/192.168.1.203.html">
          <font color="green"><u>New port 442 used 100MB</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jul-02 18:22 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.7">kaneda</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $ntop_link?>/192.168.1.7.html">
          <font color="orange"><u>620% more connections established</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jul-02 09:50 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.97">golgotha</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $stats_link?>/stats/system/load.html">
          <font color="orange"><u>System load too high. 300% over average</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jul-01 19:50 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr
        ?>/stats/frameoptions.php?ip=192.168.1.40">Router_Mad</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $opennms_link?>/element/node.jsp?node=3">
          <font color="red"><u>Smtp availability under 97%</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jun-30 17:03 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.97">golgotha</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $ntop_link?>/dataHostTraffic.html">
          <font color="orange"><u>Traffic at strange hours</u></font></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
    <td align="center"> Jun-28 12:21 </td>
    <td colspan="2" align="center">
        <a href="<?php echo $rootaddr ?>/stats/frameoptions.php?ip=192.168.1.97">golgotha</a></td>
    <td colspan="2" align="left">
        <a href="<?php echo $ntop_link?>/localHostsInfo.html"><u>OS Change: Linux 2.4.1</u></a></td>
    <td align="center"><input type="checkbox"></input> 
  </tr>
  <tr>
  <td bgcolor="silver" align="center" colspan="6"><INPUT TYPE="submit" NAME="Aceptar"
  VALUE="Aceptar"></INPUT></td>
  </TR>
  </FORM>
  <!-- end static code -->





  </table>

<?php
$db->close($conn);
?>

</body>
</html>


