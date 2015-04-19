<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAlarms");
?>
<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

<?php
    if (!$backlog_id = $_GET["backlog_id"]) {
        echo gettext("Backlog ID required"); 
        exit();
    }
    if ($_GET["alert_id"]) {
        $alert_id = $_GET["alert_id"];
    }
?>
  <h1 align="center"> <?php echo gettext("Alarms/Alerts"); ?> </h1>

<?php
require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');
require_once ('common.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_os.inc');
require_once ('classes/Alarm.inc');
require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');
require_once ('classes/Port.inc');
require_once ('classes/Util.inc');

$conf = new ossim_conf();
$acid_link = $conf->get_conf("acid_link");
$acid_prefix = $conf->get_conf("alert_viewer");

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

if (!$show_all = $_GET["show_all"]) {
    $show_all = 0;
}

?>
    <table width="100%">
   
       <tr>
         <td colspan="9"><a href="alarm_console.php"> <?php echo gettext("Back to main"); ?> </a></td>
       </tr>
   
       <tr>
        <td></td>
        <th>#</th>
        <th> <?php echo gettext("Id"); ?> </th>
        <th> <?php echo gettext("Alarm"); ?> </th>
        <th> <?php echo gettext("Risk"); ?> </th>
        <th> <?php echo gettext("Date"); ?> </th>
        <th> <?php echo gettext("Source"); ?> </th>
        <th> <?php echo gettext("Destination"); ?> </th>
        <th> <?php echo gettext("Correlation Level"); ?> </th>
        <th> <?php echo gettext("Action"); ?> </th>
      </tr>

<?php
    $have_scanmap = $conf->get_conf("have_scanmap3d");
    if($have_scanmap == 1 && $show_all){
       // Generate scanmap datafile
       $base_dir = $conf->get_conf("base_dir");

       if(!file_exists("$base_dir/tmp/$backlog_id.txt")){
            $backlog_file = fopen("$base_dir/tmp/$backlog_id.txt","w");
            if(!$backlog_file) $have_scanmap = 0;
       } else {
       $have_scanmap = 0;
       }
    } else {
    $have_scanmap = 0;
    }

    if ($alarm_list = Alarm::get_alerts($conn, $backlog_id, $show_all, $alert_id))
    {
        $count_alerts = 0;
        $count_alarms = 0;
        foreach ($alarm_list as $alarm) {

            $id  = $alarm->get_plugin_id();
            $sid = $alarm->get_plugin_sid();
            $backlog_id = $alarm->get_backlog_id();
            $risk = $alarm->get_risk();
            $snort_sid = $alarm->get_snort_sid();
            $snort_cid = $alarm->get_snort_cid();

            /* get plugin_id and plugin_sid names */

            /*
             * never used?
             *
            $plugin_id_list = Plugin::get_list($conn, "WHERE id = $id");
            $id_name = $plugin_id_list[0]->get_name();
             */

            $sid_name = "";
            if ($plugin_sid_list = Plugin_sid::get_list
                ($conn, "WHERE plugin_id = $id AND sid = $sid")) {
                $sid_name = $plugin_sid_list[0]->get_name();
            } else {
                $sid_name = "Unknown (id=$id sid=$sid)";
            }
        
?>
      <tr>
        <?php
            $name = ereg_replace("directive_alert: ", "", $sid_name);
            if ($alarm->get_alarm())
						    $name = Util::translate_alarm($conn, $name, $alarm);
                $name = "<b>$name</b>";
        ?>

        <!-- expand alarms -->
        <td><?php 
            $aid = $alarm->get_alert_id();
            if (($_GET["alert_id"] == $aid)) {
                $href = $_SERVER["PHP_SELF"] . "?backlog_id=$backlog_id&show_all=0";
                $img = "../pixmaps/arrow.gif";
                echo "&nbsp;<a href=\"$href\"><img src=\"$img\" border=\"0\"/></a>";
            } elseif (($show_all == 0) or ($alarm->get_alarm())) {
                $href = $_SERVER["PHP_SELF"] .
                    "?backlog_id=$backlog_id&show_all=1&alert_id=$aid";
                $img = "../pixmaps/arrow2.gif";
                echo "&nbsp;<a href=\"$href\"><img src=\"$img\" border=\"0\"/></a>";
            }
        ?></td>
        <!-- end expand alarms -->

        <!-- id & name alert -->
        <td><?php 
            if ($alarm->get_alarm())
                echo "<b>" . ++$count_alarms . "</b>";
            else
                echo ++$count_alerts;
        ?></td>
        <td><?php echo $aid ?></td>
        <td <?php if ($alarm->get_alarm()) echo " bgcolor=\"#eeeeee\"" ?>>
        <?php 
            if (($snort_sid > 0) and ($snort_cid)) {
                $href = "$acid_link/" . $acid_prefix . 
                    "_qry_alert.php?submit=%230-%28" . 
                    "$snort_sid-$snort_cid%29";
                echo "&nbsp;&nbsp;<a href=\"$href\">$name</a>";
            } else {
                $href = "";
                echo "&nbsp;&nbsp;$name"; 
            }
        ?></td>
        <!-- end id & name alert -->
        
        <!-- risk -->
<?php 
        $orig_date = $alarm->get_timestamp();
        $date = Util::timestamp2date($orig_date);

        $src_ip   = $alarm->get_src_ip();
        $dst_ip   = $alarm->get_dst_ip();
        $src_port = $alarm->get_src_port();
        $dst_port = $alarm->get_dst_port();
        if($have_scanmap){
        fwrite($backlog_file,"$orig_date,$src_ip,$src_port,$dst_ip,$dst_port\n");
        }
        $src_port = Port::port2service($conn, $src_port);
        $dst_port = Port::port2service($conn, $dst_port);


        if ($risk  > 7) {
            echo "<td bgcolor=\"red\"><b>";
            if ($href) echo "<a href=\"$href\">";
            echo "<font color=\"white\">$risk</font>";
            if ($href) echo "</a>";
            echo "</b></td>";
        } elseif ($risk > 4) {
            echo "<td bgcolor=\"orange\"><b>";
            if ($href) echo "<a href=\"$href\">";
            echo "<font color=\"black\">$risk</font>";
            if ($href) echo "</a>";
            echo "</b></td>";
        } elseif ($risk > 2) {
            echo "<td bgcolor=\"green\"><b>";
            if ($href) echo "<a href=\"$href\">";
            echo "<font color=\"white\">$risk</font>";
            if ($href) echo "</a>";
            echo "</b></td>";
        } else {
            echo "<td><b>";
            if ($href) echo "<a href=\"$href\">";
            echo "$risk";
            if ($href) echo "</a>";
            echo "</b></td>";
        }
?>
        <!-- end risk -->

        <td nowrap>
          <a href="<?php echo get_acid_date_link($date, $src_ip, "ip_src") ?>">
            <font color="black"><?php echo $date ?></font>
          </a>
        </td>

<?php
    $src_link = "../report/index.php?host=$src_ip&section=alerts";
    $dst_link = "../report/index.php?host=$dst_ip&section=alerts";
    $src_name = Host::ip2hostname($conn, $src_ip);
    $dst_name = Host::ip2hostname($conn, $dst_ip);
    $src_img  = Host_os::get_os_pixmap($conn, $src_ip);
    $dst_img  = Host_os::get_os_pixmap($conn, $dst_ip);

?>
        <!-- src & dst hosts -->
        <td bgcolor="#eeeeee" nowrap>
            <?php echo "<a href=\"$src_link\">$src_name</a>:$src_port $src_img"; ?></td>
        <td bgcolor="#eeeeee" nowrap>
            <?php echo "<a href=\"$dst_link\">$dst_name</a>:$dst_port $dst_img"; ?></td>
        <!-- src & dst hosts -->

        <td><?php echo $alarm->get_rule_level() ?></td>
        <td><a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete=<?php 
            echo $alarm->get_alert_id() ?>">Ack</a></td>
      </tr>

<?php
    # Alarm summary
    if ((!$show_all) or ($risk > 1)) {
        $summary = Alarm::get_alarm_stats($conn, $backlog_id, $aid);
        $summ_count = $summary["count"];
        $summ_dst_ips = $summary["dst_ips"];
        $summ_types = $summary["types"];
        $summ_dst_ports = $summary["dst_ports"];
        echo "
            <tr>
            <td colspan=\"3\"></td>
            <td colspan=\"6\">
              <b>" . gettext("Alarm Summary") ."</b> [ ";
               printf(gettext("Total Alerts: %d"), $summ_count); echo "&nbsp;-&nbsp;";
               printf(gettext("Unique Dst IPAddr: %d"), $summ_dst_ips); echo "&nbsp;-&nbsp;";  
               printf(gettext("Unique Types: %d"), $summ_types); echo "&nbsp;-&nbsp;";  
               printf(gettext("Unique Dst Ports: %d"), $summ_dst_ports); echo " ] ";
              if($conf->get_conf("have_scanmap3d")){
              echo
              "
              - [ <a href=\"visualize.php?backlog_id=$backlog_id\"> " . gettext("Visualize alarm") . " </a> ]
              ";
              }
              echo
              "
            </td>
        ";
/*
        echo "
          <tr>
            <td></td>
            <td colspan=\"3\" bgcolor=\"#eeeeee\">&nbsp;</td>
            <td colspan=\"5\">
              <table width=\"100%\">
                <tr>
                  <th colspan=\"8\">Alarm summary</th>
                </tr>
                <tr>
                  <td>Total Alerts: </td>
                  <td>" . $summary["count"] . "</td>
                  <td>Unique Dst IPAddr: </td>
                  <td>" . $summary["dst_ips"] . "</td>
                  <td>Unique Types: </td>
                  <td>" . $summary["types"] . "</td>
                  <td>Unique Dst Ports: </td>
                  <td>" . $summary["dst_ports"] . "</td>
                </tr>
               </table>
            </td>
            <td bgcolor=\"#eeeeee\">&nbsp;</td>
          </tr>
          <tr><td colspan=\"10\"></td></tr>
        ";
*/
    }
?>


<?php
        } /* foreach alarm_list */
    } /* if alarm_list */
?>
    </table>


<?php
if($have_scanmap) fclose($backlog_file);
$db->close($conn);
?>

</body>
</html>


