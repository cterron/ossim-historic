<html>
<head>
  <title> Riskmeter </title>
  <meta http-equiv="refresh" content="5">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/riskmeter.css"/>
</head>

<body>

  <h1 align="center">OSSIM Framework</h1>
  <h2 align="center">Riskmeter</h2>

<?php
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Conf.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Net_qualification.inc');
require_once ('classes/Graph_qualification.inc');
require_once ('classes/Net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Net_host_reference.inc');

$ossim_conf = new ossim_conf();
$mrtg_link = $ossim_conf->get_conf("mrtg_link");

$db = new ossim_db();
$conn = $db->connect();

/* conf */
$conf = Conf::get_conf($conn);

$THRESHOLD_DEFAULT = $conf->get_threshold();
$THRESHOLD_GRAPH_DEFAULT = $conf->get_graph_threshold();
$BAR_LENGTH_LEFT = $conf->get_bar_length_left();
$BAR_LENGTH_RIGHT = $conf->get_bar_length_right();
$BAR_LENGTH = $BAR_LENGTH_LEFT + $BAR_LENGTH_RIGHT;

/*
 * Networks 
 */
$net_stats = Net_qualification::get_list($conn, "", 
                                         "ORDER BY net_name");
$max_level = max(ossim_db::max_val($conn, "compromise", "net_qualification"),
                 ossim_db::max_val($conn, "attack", "net_qualification"));

?>
  <table align="center">

    <tr><td colspan="3"></td></tr>
    <tr><th align="center" colspan="3">Networks</th></tr>
    <tr><td colspan="3"></td></tr>


    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->

<?php

    foreach ($net_stats as $stat) {

        $net = $stat->get_net_name();

        /* get net threshold */
        if ($net_list = Net::get_list($conn, "WHERE name = '$net'")) {
            $threshold_c = $net_list[0]->get_threshold_c();
            $threshold_a = $net_list[0]->get_threshold_a();
        } else {
            $threshold_c = $threshold_a = $THRESHOLD_DEFAULT;
        }

        /* calculate proportional bar width */
        $width_c = ((($compromise = $stat->get_compromise()) / 
                                        $threshold_c) * $BAR_LENGTH_LEFT);
        $width_a = ((($attack = $stat->get_attack()) / 
                                        $threshold_a) * $BAR_LENGTH_LEFT);
?>

    <!-- C & A levels for each net -->
    <tr>
      <td align="center">
        <a href="<?php echo $_SERVER["PHP_SELF"] . 
                       "?net=$net" ?>"><?php echo $net ?></a>
      </td>
      <td align="center">
        <a href="<?php echo "$mrtg_link/net_qualification/" . 
                       strtolower($net) . ".html" ?>" 
           target="new">&nbsp;<img src="../pixmaps/graph.gif" 
                                   border="0"/>&nbsp;</a>
      </td>

      <td>
<?php
    if ($compromise <= $threshold_c) {
?>
        <img src="../pixmaps/solid-blue.jpg" height="8"
             width="<?php echo $width_c?>" title="<?php echo $compromise ?>">
        C=<?php echo round($compromise / $threshold_c * 10); ?>
<?php
    } else {
        if ($width_c >= ($BAR_LENGTH)) { 
            $width_c = $BAR_LENGTH; 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $BAR_LENGTH_LEFT?>" 
             title="<?php echo $compromise ?>">
        <!-- <img src="../pixmaps/solid-blue.jpg" height="10" width="5"> -->
        <img src="../pixmaps/solid-blue.jpg" border="0" height="8" 
             width="<?php echo $width_c - $BAR_LENGTH_LEFT?>"
             title="<?php echo $compromise ?>">
        C=<?php echo round($compromise/$threshold_c*10) ?>
        <img src="<?php echo $icon ?>">
<?php
    }
    if ($attack <= $threshold_a) {
?>
        <br/>
        <img src="../pixmaps/solid-red.jpg" height="8" 
             width="<?php echo $width_a?>" title="<?php echo $attack ?>">
        A=<?php echo round($attack / $threshold_a * 10) ?>
<?php
    } else {
        if ($width_a >= ($BAR_LENGTH)) { 
            $width_a = ($BAR_LENGTH); 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <br/><img src="../pixmaps/solid-red.jpg" height="8" 
                  width="<?php echo $BAR_LENGTH_LEFT?>" 
                  title="<?php echo $attack ?>">
        <img src="../pixmaps/solid-red.jpg" height="8" 
             width="<?php echo $width_a - $BAR_LENGTH_LEFT?>"
             title="<?php echo $attack ?>">
        A=<?php echo round($attack / $threshold_a * 10) ?>
        <img src="<?php echo $icon ?>">
<?php 
    }
?>
      </td>
    </tr>
    <!-- end C & A levels for each net -->
    
<?php
    }
?>

    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->

<?php
/* 
 * Hosts
 */
 
/*
 * Si se pincha en el nombre de red, sólo mostrar las ips
 * que pertenezcan a esa red.
 */
/*
if ($_GET["net"]) {

    $net = $_GET["net"];
    
    foreach (Net_host_reference::get_list($conn, "WHERE net_name = '$net'")
             as $net_host_reference) 
    {
        $ip = $net_host_reference->get_host_ip();

        foreach (Host_qualification::get_list($conn, "WHERE host_ip = '$ip'")
                 as $host_qualification)
        {
            $ip_stats[] = new Host_qualification 
                                ($host_qualification->get_host_ip(),
                                 $host_qualification->get_compromise(),
                                 $host_qualification->get_attack());
        }
    }
} else {
*/
    $ip_stats = Host_qualification::get_list
        ($conn, "", "ORDER BY compromise + attack DESC");
/* } */

#if (count($ip_stats) > 0) {
$max_level = max(ossim_db::max_val($conn, "compromise", "host_qualification"),
                 ossim_db::max_val($conn, "attack", "host_qualification"));
?>


    <tr><td colspan="3"><br/></td></tr>
    <tr><th align="center" colspan="3">Hosts</th></tr>
    <tr><td colspan="3"></td></tr>

    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->

<?php

if ($ip_stats) {
    foreach ($ip_stats as $stat) {
    
        $ip = $stat->get_host_ip();

        /* get host threshold */
        if ($host_list = Host::get_list($conn, "WHERE ip = '$ip'")) {
            $threshold_c = $host_list[0]->get_threshold_c();
            $threshold_a = $host_list[0]->get_threshold_a();
            $hostname = $host_list[0]->get_hostname();
        } else {
            $threshold_c = $threshold_a = $THRESHOLD_DEFAULT;
            $hostname = $ip;
        }

        /* calculate proportional bar width */
        $width_c = ((($compromise = $stat->get_compromise()) / 
                                        $threshold_c) * $BAR_LENGTH_LEFT);
        $width_a = ((($attack = $stat->get_attack()) / 
                                        $threshold_a) * $BAR_LENGTH_LEFT);
?>

    <!-- C & A levels for each IP -->
    <tr>
      <td align="center">
        <a href="frameoptions.php?ip=<?php echo $ip ?>" 
           target="new" title="<?php echo $ip ?>"><?php echo $hostname ?></a>
      </td>
      <td align="center">
        <a href="<?php echo $mrtg_link ?>host_qualification/<?php echo $ip ?>.html" target="new">
        &nbsp;<img src="../pixmaps/graph.gif" border="0"/>&nbsp;</a>
      </td>

      <td>
<?php
    if ($compromise <= $threshold_c) {
?>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $width_c ?>" title="<?php echo $compromise ?>">
        C=<?php echo round($compromise / $threshold_c * 10); ?>
<?php
    } else {
        if ($width_c >= ($BAR_LENGTH)) { 
            $width_c = $BAR_LENGTH; 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $BAR_LENGTH_LEFT?>"
             title="<?php echo $compromise?>">
        <!-- <img src="../pixmaps/solid-blue.jpg" height="10" width="5"> -->
        <img src="../pixmaps/solid-blue.jpg" border="0" height="8" 
             width="<?php echo $width_c - $BAR_LENGTH_LEFT?>"
             title="<?php echo $compromise ?>">
        C=<?php echo round($compromise / $threshold_c * 10) ?>
        <img src="<?php echo $icon ?>">
<?php
    }
    if ($attack <= $threshold_a) {
?>
        <br/><img src="../pixmaps/solid-red.jpg" height="8" 
                  width="<?php echo $width_a?>" 
                  title="<?php echo $attack ?>">
        A=<?php echo round($attack / $threshold_a * 10); ?>
<?php
    } else {
        if ($width_a >= ($BAR_LENGTH)) { 
            $width_a = $BAR_LENGTH; 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <br/><img src="../pixmaps/solid-red.jpg" height="8" 
                  width="<?php echo $BAR_LENGTH_LEFT?>"
             title="<?php echo $attack ?>">
        <img src="../pixmaps/solid-red.jpg" height="8" 
             width="<?php echo $width_a - $BAR_LENGTH_LEFT?>" 
             title="<?php echo $attack ?>">
        A=<?php echo round($attack / $threshold_a * 10) ?>
        <img src="<?php echo $icon ?>">
<?php 
    } /* foreach */
} /* if */
?>
      </td>
    </tr>
    <!-- end C & A levels for each IP -->
    
<?php
    }
?>
    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->


<?php
$graphs_stats = Graph_qualification::get_list($conn);
?>
    <!-- graphs -->
    <tr><td colspan="3"><br/></td></tr>
    <tr><th align="center" colspan="3">Graphs</th></tr>
<!--    <tr><td colspan="3"><br/></td></tr> -->
    
    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->
    
<?php

    $max_level = 
        max(ossim_db::max_val($conn, "compromise", "graph_qualification"),
            ossim_db::max_val($conn, "attack", "graph_qualification"));
    if ($max_level != 0) 
        $barSize = $BAR_LENGTH / $max_level;
    else
        $barSize = 1;
    
    $i = 0;

    if ($graphs_stats) {
    foreach ($graphs_stats as $graph) {

        /* calculate proportional bar width */
        $width_c = ((($compromise = $graph->get_compromise()) / 
                        $THRESHOLD_GRAPH_DEFAULT) * $BAR_LENGTH_LEFT);
        $width_a = ((($attack = $graph->get_attack()) 
                        / $THRESHOLD_GRAPH_DEFAULT) * $BAR_LENGTH_LEFT);
?>
    <tr>
      <td>
        <a href="../graphs/index.php?graph_id=<?php 
            echo $graph->get_graph_id()?>">Graph <?php 
            echo $graph->get_graph_id() ?></a></td><td></td>

      <td>
<?php
    if ($compromise <= $THRESHOLD_GRAPH_DEFAULT) {
?>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $width_c ?>"
             title="<?php echo $compromise ?>">
        C=<?php echo round($compromise / $threshold_c*10); ?>
<?php
    } else {
        if ($width_c >= ($BAR_LENGTH)) { 
            $width_c = $BAR_LENGTH; 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <img src="../pixmaps/solid-blue.jpg" height="8" 
             width="<?php echo $BAR_LENGTH_LEFT?>"
             title="<?php echo $compromise ?>">
        <!-- <img src="../pixmaps/solid-blue.jpg" height="10" width="5"> -->
        <img src="../pixmaps/solid-blue.jpg" border="0" height="8" 
             width="<?php echo $width_c - $BAR_LENGTH_LEFT?>"
             title="<?php echo $compromise ?>">
        C=<?php echo round($compromise/$threshold_c*10); ?>
        <img src="<?php echo $icon ?>">
<?php
    }
    if ($attack <= $THRESHOLD_GRAPH_DEFAULT) {
?>
        <br/><img src="../pixmaps/solid-red.jpg" height="8" 
                  width="<?php echo $width_a?>" title="<?php echo $attack ?>">
        A=<?php echo round($attack/$threshold_a*10); ?>
<?php
    } else {
        if ($width_a >= ($BAR_LENGTH)) { 
            $width_a = $BAR_LENGTH; 
            $icon = "../pixmaps/major-red.gif";
        }else{
            $icon = "../pixmaps/major-yellow.gif";
        }
?>
        <br/><img src="../pixmaps/solid-red.jpg" height="8" 
                  width="<?php echo $BAR_LENGTH_LEFT?>"
                  title="<?php echo $attack ?>">
        <img src="../pixmaps/solid-red.jpg" height="8" 
             width="<?php echo $width_a - $BAR_LENGTH_LEFT?>" 
             title="<?php echo $attack ?>">
        A=<?php echo round($attack/$threshold_a*10); ?>
        <img src="<?php echo $icon ?>">
<?php 
    }
?>
      </td>
    </tr>
<?php
    }
}
?>

    <!-- rule for threshold -->
    <tr>
      <td></td><td></td>
      <td>
        <img src="../pixmaps/gauge-blue.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_LEFT; ?>">
        <img src="../pixmaps/gauge-red.jpg" height="4" 
             width="<?php echo $BAR_LENGTH_RIGHT; ?>">
      </td>
    </tr>
    <!-- end rule for threshold -->

    <!-- end graphs -->

</table>
<br>
</body>
</html>

