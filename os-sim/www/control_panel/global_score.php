<html>
<head>
  <title> Control Panel </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center">Metrics</h1>

<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Control_panel_host.inc');
require_once ('classes/Control_panel_net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Net_qualification.inc');
require_once ('classes/Conf.inc');
require_once ('acid_funcs.inc');
require ('common.inc');


$mrtg_link = $conf->get_conf("mrtg_link");



function echo_values($val, $max, $ip, $date) {

    global $acid_link;

    if ($val / $max > 5) {
?>
        <td bgcolor="red">
          <a href="<?php echo get_acid_date_link($ip, $date) ?>">
            <font color="white"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } elseif ($val / $max > 3) {
?>
        <td bgcolor="orange">
          <a href="<?php echo get_acid_date_link($ip, $date) ?>">
            <font color="black"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } elseif ($val / $max > 1) {
?>
        <td bgcolor="green">
          <a href="<?php echo get_acid_date_link($ip, $date) ?>">
            <font color="white"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } else {
?>
        <td>
          <a href="<?php echo get_acid_date_link($ip, $date) ?>">
            <font color="black"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } 
}


$acid_link = $conf->get_conf("acid_link");
if (!$range = mysql_escape_string($_GET["range"]))  $range = 'day';

/* get conf */
$conf = new ossim_conf();
$graph_link = $conf->get_conf("graph_link");

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

$framework_conf = Conf::get_conf($conn);
$THRESHOLD = $framework_conf->get_threshold();

/* get host & net lists */
$hosts_order_by_c = Control_panel_host::get_metric_list($conn, $range, 'compromise');
$hosts_order_by_a = Control_panel_host::get_metric_list($conn, $range, 'attack');
$nets_order_by_c = Control_panel_net::get_metric_list($conn, $range, 'compromise');
$nets_order_by_a = Control_panel_net::get_metric_list($conn, $range, 'attack');


function get_acid_date_link($ip, $date)
{
    global $acid_link;
    
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

    return "$acid_link/acid_qry_main.php?new=1&time[0][1]=>=&time[0][2]=". $regs[2] .
            "&time[0][3]=" . $regs[3] .
            "&time[0][4]=" . $regs[1] .
            "&time[0][5]=" . $regs[4] .
            "&time[0][6]=" . $regs[5] .
            "&ip_addr[0][1]=ip_dst&ip_addr[0][2]==&ip_addr[0][3]=$ip&".
            "sort_order=time_d&".
            "submit=Query+DB&num_result_rows=-1&time_cnt=1&ip_addr_cnt=1";
}

?>

  <table align="center" width="100%">
    <tr><td colspan="2">
      [<a
      <?php if ($range == 'day') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day">Last Day</a>]
      [<a 
      <?php if ($range == 'month') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month">Last Month</a>]
      [<a 
      <?php if ($range == 'year') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year">Last Year</a>]
    </td></tr>
    <tr><td>
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
      <!-- <img src="<?php // echo "$image2"; ?>"> -->
    </td>
    <td>
      <table align="center">
        <tr><td colspan="2"></td></tr>
        <tr>
          <th colspan="2"><a href="../riskmeter/index.php">Riskmeter - Real Time Monitoring</a></th>
        </tr>
        <tr><td colspan="2"></td></tr>
        <tr>
          <td bgcolor="#eeeeee"><b>Global compromise</b></td>
<?php
            $compromise = Host_qualification::get_global_compromise($conn);
            if ($compromise / $THRESHOLD > 5)
                echo "<td bgcolor=\"red\"><font color=\"white\"><b>$compromise</b></font></td>";
            elseif ($compromise / $THRESHOLD > 3)
                echo "<td bgcolor=\"orange\"><font color=\"black\"><b>$compromise</b></font></td>";
            elseif ($compromise / $THRESHOLD > 1)
                echo "<td bgcolor=\"green\"><font color=\"white\"><b>$compromise</b></font></td>";
            else
                echo "<td>$compromise</td>";
?>
        </tr>
        <tr><td colspan="2"></td></tr>
        <tr>
          <td bgcolor="eeeeee"><b>Global attack</b></td>
<?php
            $attack = Host_qualification::get_global_attack($conn);
            if ($attack / $THRESHOLD > 5)
                echo "<td bgcolor=\"red\"><font color=\"white\"><b>$attack</b></font></td>";
            elseif ($attack / $THRESHOLD > 3)
                echo "<td bgcolor=\"orange\"><font color=\"black\"><b>$attack</b></font></td>";
            elseif ($attack / $THRESHOLD > 1)
                echo "<td bgcolor=\"green\"><font color=\"white\"><b>$attack</b></font></td>";
            else
                echo "<td>$attack</td>";
?>
        </tr>
        <tr><td colspan="2"></td></tr>
      </table>
    </td>
    </tr>

  <tr><th colspan="6">Global Score - Networks</th></tr>
    <tr>

      <!-- Net C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Network</th>
            <th>Date Max C</th>
            <th>C Max</th>
            <th>C Actual</th>
          </tr>
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) {
        $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1, $range);
?>
          <tr>
            <td nowrap><a href=""><?php echo $net->get_net_name(); ?></a></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_c_date() ?></font></td>
            <?php 
                  echocolor($net->get_max_c(), 
                            Net::netthresh_c($conn, $net->get_net_name()),
                            $image);
                  $net_list = Net_qualification::get_list($conn, 
                                        "WHERE net_name = '" . 
                                        $net->get_net_name() . "'");
                  echocolor($net_list[0]->get_compromise(), 
                            Net::netthresh_c($conn, $net->get_net_name()),
                            $image);
            ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <!-- end net C levels -->


      <!-- Net A levels --> 
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Network</th>
            <th>Date Max A</th>
            <th>A Max</th>
            <th>A Actual</th>
          </tr>
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
        $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1, $range);
?>
          <tr>
            <td nowrap><a href=""><?php echo $net->get_net_name(); ?></a></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_a_date() ?></font>
            <?php 
                  echocolor($net->get_max_a(), 
                            Net::netthresh_a($conn, $net->get_net_name()),
                            $image);
                  $net_list = Net_qualification::get_list($conn, 
                                        "WHERE net_name = '" . 
                                        $net->get_net_name() . "'");
                  echocolor($net_list[0]->get_attack(), 
                            Net::netthresh_a($conn, $net->get_net_name()),
                            $image);
            ?>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>
    <!-- end net A levels -->

    <tr><th colspan="6">Global Score - Hosts</th></tr>
    <tr>
      
      <!-- host C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Host</th>
            <th>Date Max C</th>
            <th>C Max</th>
            <th>C Actual</th>
          </tr>
          
<?php 
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) { 
        $host_ip = $host->get_host_ip();
        $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                                  $start, "N", 1, $range); 
?>
          <tr>
            <td nowrap><a href="../report/index.php?host=<?php 
                echo $host_ip ?>&section=metrics"><?php 
                   echo Host::ip2hostname($conn, $host_ip) ?></a>
            </td>
            <td>
              <a href="<?php echo $image ?>"><img
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $host->get_max_c_date() ?></font></td>
        <?php
            echo_values($host->get_max_c(),
                        Host::ipthresh_c($conn, $host->get_host_ip()),
                        $host->get_host_ip(),
                        $host->get_max_c_date());
            $host_list = Host_qualification::get_list($conn, 
                                        "WHERE host_ip = '" . 
                                        $host->get_host_ip() . "'");
            if ($host_list)
                echocolor($host_list[0]->get_compromise(), 
                          Host::ipthresh_c($conn, $host->get_host_ip()),
                          $image);
            else
                echocolor(0, 
                          Host::ipthresh_c($conn, $host->get_host_ip()), 
                          $image);
        ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <!-- end host C levels -->


      <!-- host A levels -->
      <td valign="top">
        <table width="100%">
        <tr>
          <th colspan="2">Host</th>
          <th>Date Max A</th>
          <th>A Max</th>
          <th>A Actual</th>
        </tr>
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) { 
        $host_ip = $host->get_host_ip();
        $image = graph_image_link($host->get_host_ip(), "host", "attack",
                                  $start, "N", 1, $range); 
    ?>
          <tr>
            <td nowrap><a href="../report/index.php?host=<?php 
                echo $host_ip ?>&section=metrics"><?php 
                   echo Host::ip2hostname($conn, $host_ip) ?></a>
            </td>
            <td>
              <a href="<?php echo $image ?>"><img
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $host->get_max_a_date(); ?></font></td>
        <?php
            echo_values($host->get_max_a(),
                        Host::ipthresh_a($conn,$host->get_host_ip()),
                        $host->get_host_ip(),
                        $host->get_max_a_date());
            $host_list = Host_qualification::get_list($conn, 
                                        "WHERE host_ip = '" . 
                                        $host->get_host_ip() . "'");
            if ($host_list)
                echocolor($host_list[0]->get_attack(), 
                          Host::ipthresh_a($conn, $host->get_host_ip()),
                          $image);
            else
                echocolor(0, 
                          Host::ipthresh_a($conn, $host->get_host_ip()), 
                          $image);
        ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <!-- end host A levels -->
     
    </tr>
  </table>

<?php
$db->close($conn);
?>

</body>
</html>


