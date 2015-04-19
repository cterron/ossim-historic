<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelMetrics");
$user = Session::get_session_user();
?>

<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>
<!--
  <h1 align="center"> <?php echo gettext("Metrics"); ?> </h1>
  -->
<?php

require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Control_panel_host.inc');
require_once ('classes/Control_panel_net.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Net.inc');
require_once ('classes/Net_group.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Net_qualification.inc');
require_once ('acid_funcs.inc');
require_once ('classes/Util.inc');
require_once ('common.inc'); // TODO: move functions to Util.inc



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

/* get conf */
$framework_conf = new ossim_conf();
$THRESHOLD = $framework_conf->get_conf("threshold");
$graph_link = $framework_conf->get_conf("graph_link");
$acid_link = $framework_conf->get_conf("acid_link");
$use_svg_graphics = $framework_conf->get_conf("use_svg_graphics");

/* range select (day, week, month or year) */
if (array_key_exists('range', $_GET))
    $range = mysql_escape_string($_GET["range"]);
else
    $range = 'day';


/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

/* get host & net lists */
$hosts_order_by_c = Control_panel_host::get_metric_list($conn, $range, 'compromise');
$hosts_order_by_a = Control_panel_host::get_metric_list($conn, $range, 'attack');
$nets_order_by_c = Control_panel_net::get_metric_list($conn, $range, 'compromise');
$nets_order_by_a = Control_panel_net::get_metric_list($conn, $range, 'attack');

$net_groups = Net_group::get_list($conn);

if (is_array($nets_order_by_c)) {
foreach($nets_order_by_c as $temp_net){
    $net_name = $temp_net->get_net_name();
    if($net_groups){
    foreach($net_groups as $net_group){
        $ng_name = $net_group->get_name();
        $net_group_array[$ng_name]["name"] = $ng_name;
        if(Net_group::isNetInGroup($conn, $ng_name, $net_name)){
            $net_group_array[$ng_name]["time_range"] = $temp_net->get_time_range(); 
            $net_group_array[$ng_name]["max_c"] += $temp_net->get_max_c(); 
            $net_group_array[$ng_name]["max_a"] += $temp_net->get_max_a(); 
            if($net_group_array[$ng_name]["max_c_date"] < $temp_net->get_max_c_date()){
                $net_group_array[$ng_name]["max_c_date"] = $temp_net->get_max_c_date();
            }
            if($net_group_array[$ng_name]["max_a_date"] < $temp_net->get_max_a_date()){
                $net_group_array[$ng_name]["max_a_date"] = $temp_net->get_max_a_date();
            }
        }
    }
    }
}
}

function ordenar_c($a, $b){
     return ($a["max_c"] < $b["max_c"]) ? True : False;
}

function ordenar_a($a, $b){
     return ($a["max_a"] < $b["max_a"]) ? True : False;
}


/* get global values */
$query = "SELECT * FROM control_panel 
    WHERE id = 'global_$user' AND time_range = '$range';";
if (!$rs_global = &$conn->Execute("$query"))
    print $conn->ErrorMsg();

?>

  <table align="center" width="100%">
    <tr><td colspan="2">
      [<a
      <?php if ($range == 'day') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day"> <?php echo gettext("Last Day"); ?> </a>]
      [<a 
      <?php if ($range == 'week') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=week"> <?php echo gettext("Last Week"); ?> </a>]
      [<a 
      <?php if ($range == 'month') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month"> <?php echo gettext("Last Month"); ?> </a>]
      [<a 
      <?php if ($range == 'year') echo "class=\"selected\"" ?>
      href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year"> <?php echo gettext("Last Year"); ?> </a>]
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

        $image2 = "$graph_link?ip=global_$user&what=attack&start=$start&" . 
            "end=N&type=global&zoom=0.85";
        $image1 = "$graph_link?ip=global_$user&what=compromise&start=$start&" . 
            "end=N&type=global&zoom=0.85";

?>
      <img src="<?php echo "$image1"; ?>">
      <!-- <img src="<?php // echo "$image2"; ?>"> -->
    </td>
    <td>
      <table align="center">
        <tr><td colspan="3"></td></tr>
        <tr>
          <th> <?php echo gettext("Riskmeter"); ?> </th>
          <th> <?php echo gettext("Service Level"); ?> </th>
        </tr>
        <tr>
          <td><a href="../riskmeter/index.php">
            <img border="0" src="../pixmaps/riskmeter.png"/></a>
          </td>
            <?php 
            $image = graph_image_link("level_$user", "level", "attack",
                              $start, "N", 1, $range);
            $sec_level = ($rs_global->fields["c_sec_level"] + 
                              $rs_global->fields["a_sec_level"]) / 2;
                $sec_level = sprintf("%.2f", $sec_level);
                if ($use_svg_graphics) {
                    echo " 
            <td>
                <a href=\"$image\">
                 <embed src=\"svg_level.php?sl=$sec_level&scale=0.8\"  
                        pluginspage=\"http://www.adobe.com/svg/viewer/install/\"
                        type=\"image/svg+xml\" height=\"85\" width=\"100\" /> 
                </a>
            </td>
                ";
            } else {
                if ($sec_level >= 95) {
                    $bgcolor = "green";
                    $fontcolor = "white";
                } elseif ($sec_level >= 90) {
                    $bgcolor = "#CCFF00";
                    $fontcolor = "black";
                } elseif ($sec_level >= 85) {
                    $bgcolor = "#FFFF00";
                    $fontcolor = "black";
                } elseif ($sec_level >= 80) {
                    $bgcolor = "orange";
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
            </td>";
            }
            ?>
        </tr>
        <tr><td colspan="2"></td></tr>
      </table>
    </td>
    </tr>

    <tr><th colspan="6"> <?php echo gettext("Global"); ?> </th></tr>
    <tr>
      <!-- Global C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2"> <?php echo gettext("Global"); ?> </th>
            <th> <?php echo gettext("Max C date"); ?> </th>
            <th> <?php echo gettext("Max C"); ?> </th>
            <th> <?php echo gettext("Current C"); ?> </th>
          </tr>
          <tr>
<?php
    $image = graph_image_link("global_$user", "global", "compromise",
                              $start, "N", 1, $range);
    
?>
            <td nowrap><b> <?php echo gettext("GLOBAL SCORE"); ?> </b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $rs_global->fields["max_c"] / $THRESHOLD;
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: C level exceeded (Global)&" .
            "priority=$priority&" .
            "target=Global&" .
            "metric_type=Compromise&" .
            "metric_value=" . $rs_global->fields["max_c"] ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
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
            <th colspan="2"> <?php echo gettext("Global"); ?> </th>
            <th> <?php echo gettext("Max A date"); ?> </th>
            <th> <?php echo gettext("Max A"); ?> </th>
            <th> <?php echo gettext("Current A"); ?> </th>
          </tr>
          <tr>
<?php
    $image = graph_image_link("global_$user", "global", "attack",
                              $start, "N", 1, $range);
    
?>
            <td nowrap><b> <?php echo gettext("GLOBAL SCORE"); ?> </b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $rs_global->fields["max_a"] / $THRESHOLD;
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: A level exceeded (Global)&" .
            "priority=$priority&" .
            "target=Global&" .
            "metric_type=Attack&" .
            "metric_value=" . $rs_global->fields["max_a"] ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
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

<?php 
    if (is_array($net_group_array)){
    ?>
    </tr>
    <tr><th colspan="6"> <?php echo gettext("Groups"); ?> </th></tr>
    <tr>

      <!-- Group C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2"> <?php echo gettext("Group"); ?> </th>
            <th> <?php echo gettext("Max C date"); ?> </th>
            <th> <?php echo gettext("Max C"); ?> </th>
            <th> <?php echo gettext("Current C"); ?> </th>
          </tr>
<?php
    usort($net_group_array, "ordenar_c");
    $temporary = current($net_group_array);
    while($temporary){
        $image = graph_image_link($temporary["name"], "net", "compromise",
                              $start, "N", 1, $range);
?>
          <tr>
            <td class="left" nowrap>
            <?php if($_GET["expand"] && $_GET["expand"] == $temporary["name"]){ ?>
            <a href="<?php echo $_SERVER["PHP_SELF"]?>?range=<?php echo $range;?>">-</a>
            <?php } else { ?>
            <a href="<?php echo $_SERVER["PHP_SELF"]?>?expand=<?php echo $temporary["name"] . "&range=" . $range;?>">+</a>
            <?php } ?>
            <b><?php echo Util::beautify($temporary["name"]) ?></b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $temporary["max_c"] / 
                Net_group::netthresh_c($conn, $temporary["name"]);
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: C level exceeded (Group " .
                $temporary["name"] .")&" .
            "priority=$priority&" .
            "target=Net " . $temporary["name"] . "&" .
            "metric_type=Compromise&" .
            "metric_value=" . $temporary["max_c"] ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
            </td>
            <td nowrap><font size="-2"><?php echo $temporary["max_c_date"] ?></font></td>
            <?php 
                echocolor($temporary["max_c"], 
                            Net_group::netthresh_c($conn, $temporary["name"]),
                            get_acid_date_link($temporary["max_c_date"]));
                echocolor(Net_group::get_compromise($conn, $temporary["name"]),
                            Net_group::netthresh_c($conn, $temporary["name"]),
                            $image);
            $temporary = next($net_group_array);
            ?>
          </tr>
<?php } ?>

        </table>
      </td>
      <!-- end Group C levels -->
<?php } ?>
<?php 
    if (is_array($net_group_array)){
?>
      <!-- Group A levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2"> <?php echo gettext("Group"); ?> </th>
            <th> <?php echo gettext("Max A date"); ?> </th>
            <th> <?php echo gettext("Max A"); ?> </th>
            <th> <?php echo gettext("Current A"); ?> </th>
          </tr>
<?php
    usort($net_group_array, "ordenar_a");
    $temporary = current($net_group_array);
    while($temporary){
        $image = graph_image_link($temporary["name"], "net", "compromise",
                              $start, "N", 1, $range);
?>
          <tr>
            <td class="left" nowrap>
            <?php if($_GET["expand"] && $_GET["expand"] == $temporary["name"]){ ?>
            <a href="<?php echo $_SERVER["PHP_SELF"]?>?range=<?php echo
            $range;?>">-</a>
            <?php } else { ?>
            <a href="<?php echo $_SERVER["PHP_SELF"]?>?expand=<?php echo $temporary["name"] . "&range=" . $range;?>">+</a>
            <?php } ?>
            <b><?php echo Util::beautify($temporary["name"]) ?></b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $temporary["max_a"] / 
                Net_group::netthresh_a($conn, $temporary["name"]);
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: A level exceeded (Group " .
                $temporary["name"] .")&" .
            "priority=$priority&" .
            "target=Net " . $temporary["name"] . "&" .
            "metric_type=Compromise&" .
            "metric_value=" . $temporary["max_c"] ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
            </td>
            <td nowrap><font size="-2"><?php echo $temporary["max_c_date"] ?></font></td>
            <?php 
                echocolor($temporary["max_a"], 
                            Net_group::netthresh_a($conn, $temporary["name"]),
                            get_acid_date_link($temporary["max_a_date"]));
                echocolor(Net_group::get_attack($conn, $temporary["name"]), 
                            Net_group::netthresh_a($conn, $temporary["name"]),
                            $image);
            $temporary = next($net_group_array);
            ?>
          </tr>
<?php } ?>

        </table>
      </td>
      <!-- end Group A levels -->
<?php } ?>

    </tr>
    <tr><th colspan="6"> <?php echo gettext("Networks"); ?> </th></tr>
    <tr>
    <td>
      <!-- Net C levels -->
     <table width="100%">
          <tr>
            <th colspan="2"> <?php echo gettext("Network"); ?> </th>
            <th> <?php echo gettext("Max C date"); ?> </th>
            <th> <?php echo gettext("Max C"); ?> </th>
            <th> <?php echo gettext("Current C"); ?> </th>
          </tr>

<?php
    if ($nets_order_by_c)
    foreach ($nets_order_by_c as $net) {
        if (!Net_group::isNetInGroup($conn, $_GET["expand"], $net->get_net_name()))
        if (($net->get_max_c() < Net::netthresh_c($conn, $net->get_net_name()))
        && ($net->get_max_a() < Net::netthresh_a($conn, $net->get_net_name()))
        && (Net_group::isNetInAnyGroup($conn, $net->get_net_name()))){continue;}
        $image = graph_image_link($net->get_net_name(), "net", "compromise",
                              $start, "N", 1, $range);
?>
          <tr>
            <td class="left" nowrap><b><?php echo Util::beautify($net->get_net_name()); ?></b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $net->get_max_c() / 
                Net::netthresh_c($conn, $net->get_net_name());
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: C level exceeded (Net " .
                $net->get_net_name() .")&" .
            "priority=$priority&" .
            "target=Net " . $net->get_net_name() . "&" .
            "metric_type=Compromise&" .
            "metric_value=" . $net->get_max_c() ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_c_date() ?></font></td>
            <?php 
                echocolor($net->get_max_c(), 
                            Net::netthresh_c($conn, $net->get_net_name()),
                            get_acid_date_link($net->get_max_c_date()));
                if ($net_list = Net_qualification::get_list($conn, 
                                        "WHERE net_name = '" . 
                                        $net->get_net_name() . "'"))
                {
                    echocolor($net_list[0]->get_compromise(), 
                            Net::netthresh_c($conn, $net->get_net_name()),
                            $image);
                }
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
            <th colspan="2"> <?php echo gettext("Network"); ?> </th>
            <th> <?php echo gettext("Max A date"); ?> </th>
            <th> <?php echo gettext("Max A"); ?> </th>
            <th> <?php echo gettext("Current A"); ?> </th>
          </tr>
<?php 
    if ($nets_order_by_a)
    foreach ($nets_order_by_a as $net) { 
        if (!Net_group::isNetInGroup($conn, $_GET["expand"], $net->get_net_name()))
        if (($net->get_max_a() < Net::netthresh_a($conn, $net->get_net_name()))
        && ($net->get_max_c() < Net::netthresh_c($conn, $net->get_net_name()))
        && (Net_group::isNetInAnyGroup($conn, $net->get_net_name()))){continue;}
        $image = graph_image_link($net->get_net_name(), "net", "attack",
                              $start, "N", 1, $range);
?>
          <tr>
            <td class="left" nowrap><b><?php echo Util::beautify($net->get_net_name()); ?></b></td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img 
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $net->get_max_a() / 
                Net::netthresh_a($conn, $net->get_net_name());
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: A level exceeded (Net " .
                $net->get_net_name() . ")&" .
            "priority=$priority&" .
            "target=Net " . $net->get_net_name() . "&" .
            "metric_type=Attack&" .
            "metric_value=" . $net->get_max_a() ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
            </td>
            <td nowrap><font size="-2"><?php echo $net->get_max_a_date() ?></font>
            <?php 
                echocolor($net->get_max_a(), 
                            Net::netthresh_a($conn, $net->get_net_name()),
                            get_acid_date_link($net->get_max_c_date()));
                if ($net_list = Net_qualification::get_list($conn, 
                                        "WHERE net_name = '" . 
                                        $net->get_net_name() . "'"))
                {
                    echocolor($net_list[0]->get_attack(), 
                            Net::netthresh_a($conn, $net->get_net_name()),
                            $image);
                }
            ?>
          </tr>
<?php } ?>
        </table>
      </td>
    </tr>
    <!-- end net A levels -->

    <tr><th colspan="6"> <?php echo gettext("Hosts"); ?> </th></tr>
    <tr>
      
      <!-- host C levels -->
      <td valign="top">
        <table width="100%">
          <tr>
            <th colspan="2"> <?php echo gettext("Host"); ?> </th>
            <th> <?php echo gettext("Max C date"); ?> </th>
            <th> <?php echo gettext("Max C"); ?> </th>
            <th> <?php echo gettext("Current C"); ?> </th>
          </tr>
          
<?php 
    if ($hosts_order_by_c)
    foreach ($hosts_order_by_c as $host) { 
        $host_ip = $host->get_host_ip();
        $image = graph_image_link($host->get_host_ip(), "host", "compromise",
                                  $start, "N", 1, $range); 
?>
          <tr>
            <td nowrap class="left"><a href="../report/index.php?host=<?php 
                echo $host_ip ?>&section=metrics"><?php 
                   echo Host::ip2hostname($conn, $host_ip) ?></a>
            <?php echo Host_os::get_os_pixmap($conn, $host_ip); ?>
            </td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $host->get_max_c() / 
                Host::ipthresh_c($conn, $host->get_host_ip());
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: C level exceeded (Host $host_ip)&" .
            "priority=$priority&" .
            "target=Host $host_ip&" .
            "metric_type=Compromise&" .
            "metric_value=" . $host->get_max_c() ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
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
          <th colspan="2"> <?php echo gettext("Host"); ?> </th>
          <th> <?php echo gettext("Max A date"); ?> </th>
          <th> <?php echo gettext("Max A"); ?> </th>
          <th> <?php echo gettext("Current A"); ?> </th>
        </tr>
<?php 
    if ($hosts_order_by_a)
    foreach ($hosts_order_by_a as $host) { 
        $host_ip = $host->get_host_ip();
        $image = graph_image_link($host->get_host_ip(), "host", "attack",
                                  $start, "N", 1, $range); 
    ?>
          <tr>
            <td nowrap class="left"><a href="../report/index.php?host=<?php 
                echo $host_ip ?>&section=metrics"><?php 
                   echo Host::ip2hostname($conn, $host_ip) ?></a>
            <?php echo Host_os::get_os_pixmap($conn, $host_ip); ?>
            </td>
            <td nowrap>
              <a href="<?php echo $image ?>"><img
                 src="../pixmaps/graph.gif" border="0"/></a>
        <?php 
            $priority = $host->get_max_a() / 
                Host::ipthresh_a($conn, $host->get_host_ip());
            if ($priority > 10) $priority = 10;
        ?>
        <a href="<?php echo "../incidents/incident.php?insert=1&" .
            "ref=Metric&" .
            "title=Metric Threshold: A level exceeded (Host $host_ip)&" .
            "priority=$priority&" .
            "target=Host $host_ip&" .
            "metric_type=Attack&" .
            "metric_value=" . $host->get_max_a() ?>">
            <img src="../pixmaps/incident.png" width="12" alt="i" border="0"/>
            </a>
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


