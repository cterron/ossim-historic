<html>
<head>
  <title> Riskmeter </title>
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
require_once ('acid_funcs.inc');
require_once ('common.inc');

if (!$range = $_GET["range"])  $range = 'day';

/* get conf */
$conf = new ossim_conf();
$mrtg_link = $conf->get_conf("mrtg_link");
$graph_link = $conf->get_conf("graph_link");
$acid_link = $conf->get_conf("acid_link");

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

  <table align="center">
    <tr><td colspan="8">
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day">Last Day</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month">Last Month</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year">Last Year</a>]
    </td></tr>
    <tr><td colspan="8">
<?php
        if ($range == 'day') {
            $image1 = "$graph_link?ip=global&what=attack&start=N-24h&end=N&type=global&zoom=0.85";
            $image2 = "$graph_link?ip=global&what=compromise&start=N-24h&end=N&type=global&zoom=0.85";
            $start = "N-1D";
        } elseif ($range == 'month') {
            $image1 = "$graph_link?ip=global&what=attack&start=N-1M&end=N&type=global&zoom=0.85";
            $image2 = "$graph_link?ip=global&what=compromise&start=N-1M&end=N&type=global&zoom=0.85";
            $start = "N-1M";
        } elseif ($range == 'year') {
            $image1 = "$graph_link?ip=global&what=attack&start=N-1Y&end=N&type=global&zoom=0.85";
            $image2 = "$graph_link?ip=global&what=compromise&start=N-1Y&end=N&type=global&zoom=0.85";
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
<?php foreach ($hosts_order_by_c as $host) { ?>
          <tr>
            <td><a href="<?php echo get_acid_info($host->get_host_ip(), 
                                                  $acid_link); ?>">
            <?php echo Host::ip2hostname($conn, $host->get_host_ip()); ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php
    foreach ($hosts_order_by_c as $host) {
    $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                              $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_max_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_c as $host) { 
    $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                              $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_min_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_c as $host) { 
    $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                              $start, "N", 1); 
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_avg_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_a as $host) { ?>
          <tr>
            <td><a href="<?php echo get_acid_info($host->get_host_ip(), 
                                                  $acid_link); ?>">
            <?php echo Host::ip2hostname($conn, $host->get_host_ip()); ?></a>
            </td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_a as $host) {
    $image = graph_image_link($host->get_host_ip(), "host", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_max_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_a as $host) { 
    $image = graph_image_link($host->get_host_ip(), "host", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_min_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($hosts_order_by_a as $host) { 
    $image = graph_image_link($host->get_host_ip(), "host", "attack",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $host->get_avg_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>

    <tr><th colspan="8">Compromise and Attack level - Top 5 Nets</th></tr>
    <tr>
      <th>Net</th>
      <th>Max C</th>
      <th>Min C</th>
      <th>Avg C</th>
      <th>Net</th>
      <th>Max A</th>
      <th>Min A</th>
      <th>Avg A</th>
    </tr>
    <tr>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_c as $net) { ?>
          <tr>
            <td><?php echo $net->get_net_name(); ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_max_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_min_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_c as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_avg_c(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_a as $net) { ?>
          <tr>
            <td><?php echo $net->get_net_name(); ?>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_max_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_min_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
      <td>
        <table width="100%">
<?php foreach ($nets_order_by_a as $net) { 
    $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1);
?>
          <tr>
            <td><a href="<?php echo $image ?>"><?php echo $net->get_avg_a(); ?></a></td>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>


  </table>



<?php
$db->close($conn);
?>

</body>
</html>


