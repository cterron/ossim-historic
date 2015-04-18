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
require_once ('classes/Host_os.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Net_qualification.inc');
require_once ('classes/Conf.inc');
require_once ('acid_funcs.inc');
require_once ('common.inc');


$mrtg_link = $conf->get_conf("mrtg_link");



function echo_values($val, $max, $ip, $date, $target) {

    global $acid_link;

    if ($val / $max > 5) {
?>
        <td bgcolor="red">
          <a href="<?php echo get_acid_date_link($date, $ip, $target) ?>">
            <font color="white"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } elseif ($val / $max > 3) {
?>
        <td bgcolor="orange">
          <a href="<?php echo get_acid_date_link($date, $ip, $target) ?>">
            <font color="black"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } elseif ($val / $max > 1) {
?>
        <td bgcolor="green">
          <a href="<?php echo get_acid_date_link($date, $ip, $target) ?>">
            <font color="white"><b><?php echo $val ?></b></font>
          </a>
        </td>
<?php
    } else {
?>
        <td>
          <a href="<?php echo get_acid_date_link($date, $ip, $target) ?>">
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


/* get global values */
$query = "SELECT * FROM control_panel 
    WHERE id = 'global' AND time_range = '$range';";
if (!$rs_global = &$conn->Execute("$query"))
    print $conn->ErrorMsg();

?>

  <table align="center" width="100%">
    <tr><td colspan="2">
      [<a
      <?php if ($range == 'day') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day">Last Day</a>]
      [<a 
      <?php if ($range == 'week') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=week">Last Week</a>]
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
            $start = "N-1D";
        } elseif ($range == 'week') {
            $start = "N-7D";
        } elseif ($range == 'month') {
            $start = "N-1M";
        } elseif ($range == 'year') {
            $start = "N-1Y";
        }

        $image2 = "$graph_link?ip=global&what=attack&start=$start&" . 
            "end=N&type=global&zoom=0.85";
        $image1 = "$graph_link?ip=global&what=compromise&start=$start&" . 
            "end=N&type=global&zoom=0.85";

?>
      <img src="<?php echo "$image1"; ?>">
      <!-- <img src="<?php // echo "$image2"; ?>"> -->
    </td>
    <td>
      <table align="center">
        <tr><td colspan="2"></td></tr>
        <tr>
          <th>Riskmeter</th>
          <th>Service Level</th>
        </tr>
        <tr>
          <td><a href="../riskmeter/index.php">
            <img border="0" src="../pixmaps/riskmeter.png"/></a>
          </td>
            <?php 
            $image = graph_image_link("level", "level", "attack",
                              $start, "N", 1, $range);
            $sec_level = ($rs_global->fields["c_sec_level"] + 
                              $rs_global->fields["a_sec_level"]) / 2;
                $sec_level = sprintf("%.2f", $sec_level);
                if ($sec_level >= 95) {
                    $bgcolor = "green";
                    $fontcolor = "white";
                } elseif ($sec_level >= 90) {
                    $bgcolor = "#CCFF00";
                    $fontcolor = "black";
                } elseif ($sec_level >= 85) {
                    $bgcolor = "orange";
                    $fontcolor = "black";
                } elseif ($sec_level >= 80) {
                    $bgcolor = "#FFFF00";
                    $fontcolor = "black";
                } elseif ($sec_level >= 75) {
                    $bgcolor = "#FF3300";
                    $fontcolor = "white";
                } else {
                    $bgcolor = "red";
                    $fontcolor = "white";
                }
                echo "
            <td bgcolor=\"$bgcolor\">
              <b>
                <a href=\"$image\">
                  <font size=\"+1\"color=\"$fontcolor\">$sec_level%</font>
                </a>
              </b>
            </td>
                ";
            ?>
        </tr>
        <tr><td colspan="2"></td></tr>
      </table>
    </td>
    </tr>

    <tr><th colspan="6">Global</th></tr>
    <tr>
      <!-- Global C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Global</th>
            <th>Max C date</th>
            <th>Max C</th>
            <th>Current C</th>
          </tr>
          <tr>
<?php
    $image = graph_image_link("global", "global", "compromise",
                              $start, "N", 1, $range);
    
?>
            <td nowrap><b>GLOBAL SCORE</b></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2">
              <?php echo $rs_global->fields["max_c_date"] ?>
            </font></td>
<?php
            echocolor($rs_global->fields["max_c"], $THRESHOLD,
                get_acid_date_link($rs_global->fields["max_c_date"]));
            echocolor(Host_qualification::get_global_compromise($conn), 
                      $THRESHOLD, $image);
?>
          </tr>
        </table>
      </td>
      <!-- End Global C levels -->

      <!-- Global A levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Global</th>
            <th>Max A date</th>
            <th>Max A</th>
            <th>Current A</th>
          </tr>
          <tr>
<?php
    $image = graph_image_link("global", "global", "attack",
                              $start, "N", 1, $range);
    
?>
            <td nowrap><b>GLOBAL SCORE</b></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2">
              <?php echo $rs_global->fields["max_a_date"] ?>
            </font></td>
<?php
            echocolor($rs_global->fields["max_a"], $THRESHOLD,
                get_acid_date_link($rs_global->fields["max_a_date"]));
            echocolor(Host_qualification::get_global_attack($conn), 
                      $THRESHOLD, $image);
?>
          </tr>
        </table>
      </td>
      <!-- End Global A levels -->

    </tr>
    
    <tr><th colspan="6">Networks</th></tr>
    <tr>

      <!-- Net C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Network</th>
            <th>Max C date</th>
            <th>Max C</th>
            <th>Current C</th>
          </tr>
<?php 
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) {
        $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1, $range);
?>
          <tr>
            <td nowrap><b><?php echo $net->get_net_name(); ?></b></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_c_date() ?></font></td>
            <?php 
                  echocolor($net->get_max_c(), 
                            Net::netthresh_c($conn, $net->get_net_name()),
                            get_acid_date_link($net->get_max_c_date()));
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
            <th>Max A date</th>
            <th>Max A</th>
            <th>Current A</th>
          </tr>
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
        $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1, $range);
?>
          <tr>
            <td nowrap><b><?php echo $net->get_net_name(); ?></b></td>
            <td>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_a_date() ?></font>
            <?php 
                  echocolor($net->get_max_a(), 
                            Net::netthresh_a($conn, $net->get_net_name()),
                            get_acid_date_link($net->get_max_c_date()));
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

    <tr><th colspan="6">Hosts</th></tr>
    <tr>
      
      <!-- host C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2">Host</th>
            <th>Max C date</th>
            <th>Max C</th>
            <th>Current C</th>
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
            <?php echo Host_os::get_os_pixmap($conn, $host_ip); ?>
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
                        $host->get_max_c_date(),
                        "ip_src");
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
          <th>Max A date</th>
          <th>Max A</th>
          <th>Current A</th>
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
            <?php echo Host_os::get_os_pixmap($conn, $host_ip); ?>
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
                        $host->get_max_a_date(),
                        "ip_dst");
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


