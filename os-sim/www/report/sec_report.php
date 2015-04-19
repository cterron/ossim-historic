<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsSecurityReport");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<?php
    require_once 'classes/Security.inc';

    if (GET('type') == 'alarm') {
    ?>
        <h1> <?php echo gettext("Alarm Security report"); ?> </h1>
    <?php
        $report_type = "alarm";
    } else {
        $report_type = "event";
    ?>
        <h1> <?php echo gettext("Security report"); ?> </h1>
    <?php

    }

    require_once('ossim_conf.inc');
    $path_conf = $GLOBALS["CONF"];
    $jpgraph_path = $path_conf->get_conf("jpgraph_path");

    if (!is_readable($jpgraph_path)) {
            $error = new OssimError();
            $error->display("JPGRAPH_PATH");
    }


    require_once ('ossim_db.inc');
    require_once ('classes/Host.inc');
    require_once ('classes/Host_os.inc');
    require_once ('jgraphs/jgraphs.php');

    require_once ('classes/SecurityReport.inc');
    $security_report = new SecurityReport();

    $server = $_SERVER["SERVER_ADDR"];
    $file   = $_SERVER["REQUEST_URI"];

    /* database connect */
    $db = new ossim_db();
    $conn = $db->connect();


    /* Number of hosts to show */
    $NUM_HOSTS = 10;


    ##############################
    # Top attacked hosts
    ##############################
    if (GET('section') == 'attacked') 
    {
        ip_max_occurrences("ip_dst");
    }

    ##############################
    # Top attacker hosts
    ##############################
    elseif (GET('section') == 'attacker') {
        ip_max_occurrences("ip_src");
    }

    ##############################
    # Top events received
    ##############################
    elseif (GET('section') == 'events_recv') {
        event_max_occurrences();
    }

    ##############################
    # Top events risk
    ##############################
    elseif (GET('section') == 'events_risk') {
        event_max_risk();
    }

    ##############################
    # Top used destination ports
    ##############################
    elseif (GET('section') == 'dest_ports') {
        port_max_occurrences();
    }

    /* Top data traffic */
    elseif (GET('section') == 'traffic') {
        echo "Working on...";
    }

    /* Top throughput */
    elseif (GET('section') == 'throughput') {
        echo "Working on...";
    }

    /* Top used services */
    elseif (GET('section') == 'services') {
        echo "Working on...";
    }

    ###############################
    # Top less stable services 
    ###############################
    elseif (GET('section') == 'availability') {
        less_stable_services();
    }

    elseif (GET('section') == 'all') {
        ip_max_occurrences("ip_dst");
        echo "<br/><br/>";
        ip_max_occurrences("ip_src");
        echo "<br/><br/>";
        port_max_occurrences();
        echo "<br/><br/>";
        event_max_occurrences();
        echo "<br/><br/>";
        event_max_risk();
        // echo "<br/>";
        // less_stable_services();
    }

    $db->close($conn);
?>
   
</body>
</html>



<?php


    /* 
     * return the list of host with max occurrences 
     * as dest or source
     * pre: type is "ip_src" or "ip_dst"
     */
    function ip_max_occurrences($target)
    {
        global $NUM_HOSTS;
        global $security_report;
        global $report_type;
    
        /* ossim framework conf */
        $conf = $GLOBALS["CONF"];
        $acid_link = $conf->get_conf("acid_link");
        $ossim_link = $conf->get_conf("ossim_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $report_graph_type = $conf->get_conf("report_graph_type");

        if (!strcmp($target, "ip_src")) {
            if ($report_type == "alarm") {
                $target = "src_ip";
            }
            $title = _("Attacker hosts");
        } elseif (!strcmp($target, "ip_dst")) {
            if ($report_type == "alarm") {
                $target = "dst_ip";
            }
            $title = _("Attacked hosts");
        }
?>
        <h2>Top <?php echo "$NUM_HOSTS $title" ?></h2>
        <table align="center">
        <tr><td valign="top">
        <table align="center">
          <tr>
            <th> <?php echo gettext("Host"); ?> </th>
            <th> <?php echo gettext("Occurrences"); ?> </th>
          </tr>
<?php
            $list = $security_report->AttackHost($security_report->ossim_conn, 
                                                 $target, $NUM_HOSTS, $report_type);
            foreach ($list as $l) {

                $ip = $l[0];
                $occurrences = number_format($l[1], 0, ",", ".");
                $hostname = Host::ip2hostname($security_report->ossim_conn, $ip);
                $os_pixmap = Host_os::get_os_pixmap($security_report->ossim_conn, $ip);
                if ($report_type == "alarm") {
                    if ($target == "src_ip") {
                        $link = "$ossim_link/control_panel/alarm_console.php?src_ip=" . $ip; 
                    } elseif ($target == "dst_ip") {
                        $link = "$ossim_link/control_panel/alarm_console.php?dst_ip=" . $ip; 
                    } else {
                        $link = "$ossim_link/control_panel/alarm_console.php?src_ip=" . $ip . "&dst_ip=" . $ip; 
                    }
                } else {
                $link = "$acid_link/".$acid_prefix."_stat_alerts.php?&" . 
                    "num_result_rows=-1&" .
                    "submit=Query+DB&" . 
                    "current_view=-1&" .
                    "ip_addr[0][1]=$target&" . 
                    "ip_addr[0][2]==&" . 
                    "ip_addr[0][3]=$ip&" . 
                    "ip_addr_cnt=1&" . 
                    "sort_order=time_d";
                }
?>
          <tr>
            <td>
              <a title="<?php echo $ip ?>" 
                 href="<?php echo $link ?>"><?php echo $hostname ?></a>
              <?php echo $os_pixmap ?>
            </td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
        }
?>
        </table>
        </td>
        <td valign="top">
<?php
        if ($report_graph_type == "applets") {
            jgraph_attack_graph($target, $NUM_HOSTS);
        } else {
?>
        <img src="graphs/attack_graph.php?target=<?php 
                 echo $target ?>&hosts=<?php echo $NUM_HOSTS ?>&type=<?php echo $report_type ?>" 
                 alt="attack_graph"/>
<?php
        }
?>
        </td>                 
        </tr>
        </table>
<?php
    }

    /* 
     * return the event with max occurrences
     */
    function event_max_occurrences()
    {
        global $NUM_HOSTS;
        global $security_report;
        global $report_type;
    
        /* ossim framework conf */
        $conf = $GLOBALS["CONF"];
        $acid_link = $conf->get_conf("acid_link");
        $ossim_link = $conf->get_conf("ossim_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $report_graph_type = $conf->get_conf("report_graph_type");
?>
        <?php if($report_type == "alarm") { ?>
        <h2>Top <?php echo "$NUM_HOSTS Alarms" ?></h2>
        <?php } else { ?>
        <h2>Top <?php echo "$NUM_HOSTS Events" ?></h2>
        <?php } ?>
        <table align="center">
          <tr>
            <?php if($report_type == "alarm") { ?>
            <th> <?php echo gettext("Alarm"); ?> </th>
            <?php } else { ?>
            <th> <?php echo gettext("Event"); ?> </th>
            <?php } ?>
            <th> <?php echo gettext("Occurrences"); ?> </th>
          </tr>
<?php
            $list = $security_report->Events($NUM_HOSTS, $report_type);
            foreach ($list as $l)
            {
                $event = $l[0];
                $short_event = SecurityReport::Truncate($event,60);
                $occurrences = number_format($l[1], 0, ",", ".");
?>
          <tr>
             <?php
                if ($report_type == "alarm") {
                    $link = "$ossim_link/control_panel/alarm_console.php"; 
                } else {
                    $link = "$acid_link/".$acid_prefix."_qry_main.php?new=1&" . 
                    "sig[0]==&" . 
                    "sig[1]=".urlencode($event)."&" . 
                    "sig[2]==&" . 
                    "submit=Query+DB&" . 
                    "num_result_rows=-1&" . 
                    "sort_order=time_d";
                }

             ?>
            <td><a href="<?php echo $link ?>"><?php echo $short_event ?></a></td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
        }
?>
        <tr>
          <td colspan="2">
            <br/>
<?php
        if ($report_graph_type == "applets") {
            jgraph_nbevents_graph();
        } else {
?>
            <img src="graphs/events_received_graph.php?hosts=<?php 
                 echo $NUM_HOSTS ?>&type=<?php echo $report_type ?>" alt="events graph"/>
<?php
        }
?>
          </td>
        <tr/>
        </table>
<?php
    }

    
    /* 
     * return a list of events ordered by risk
     */
    function event_max_risk()
    {
        global $NUM_HOSTS;
        global $security_report;
        global $report_type;
        require_once ('sec_util.php');
?>
        <?php if($report_type == "alarm") { ?>
        <h2>Top <?php echo "$NUM_HOSTS Alarms by Risk" ?></h2>
        <?php } else { ?>
        <h2>Top <?php echo "$NUM_HOSTS Events by Risk" ?></h2>
        <?php } ?>

        <table align="center">
          <tr>
            <?php if($report_type == "alarm") { ?>
            <th> <?php echo gettext("Alarm"); ?> </th>
            <?php } else { ?>
            <th> <?php echo gettext("Event"); ?> </th>
            <?php } ?>
            <th> <?php echo gettext("Risk"); ?> </th>
          </tr>
<?php
            $list = $security_report->EventsByRisk($NUM_HOSTS, $report_type);
            foreach ($list as $l) {
                $event = $l[0];
                $risk  = $l[1];
?>
          <tr>
            <td><?php echo $event ?></a></td>
            <?php echo_risk($risk); ?>
          </tr>
<?php
        }
?>
        </table>
        <br/>
<?php
    }

    /* 
     * return the list of ports with max occurrences 
     */
    function port_max_occurrences()
    {
        global $NUM_HOSTS;
        global $security_report;
        global $report_type;
    
        /* ossim framework conf */
        $conf = $GLOBALS["CONF"];
        $acid_link = $conf->get_conf("acid_link");
        $ossim_link = $conf->get_conf("ossim_link");
        $acid_prefix = $conf->get_conf("event_viewer");
        $report_graph_type = $conf->get_conf("report_graph_type");
        
?>
        <h2>Top <?php echo "$NUM_HOSTS" ?> Used Ports</h2>
        <table align="center">
          <tr>
            <td valign="top">
        <table align="center">
          <tr>
            <th>Port</th>
            <th>Service</th>
            <th>Occurrences</th>
          </tr>
<?php
            $list = $security_report->Ports($NUM_HOSTS, $report_type);
            foreach ($list as $l)
            {

                $port = $l[0];
                $service = $l[1];
                $occurrences = number_format($l[2], 0, ",", ".");
?>
          <tr>
            <td>
              <?php 
                $link = "$acid_link/".$acid_prefix."_stat_uaddr.php?" . 
                    "tcp_port[0][0]=+&" . 
                    "tcp_port[0][1]=layer4_dport&" . 
                    "tcp_port[0][2]==&" . 
                    "tcp_port[0][3]=$port&" . 
                    "tcp_port[0][4]=+&" . "tcp_port[0][5]=+&" . 
                    "tcp_port_cnt=1&" . 
                    "layer4=TCP&" . 
                    "num_result_rows=-1&" . 
                    "current_view=-1&" . 
                    "addr_type=1&" . 
                    "sort_order=occur_d";
                echo "<a href=\"$link\">$port</a>";
              ?>
            </td>
            <td><?php echo $service ?></td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
        }

        echo "</table>\n";
?>
            </td>
            <td valign="top">
<?php
        if ($report_graph_type == "applets") {
            jgraph_ports_graph();
        } else {
?>
              <img src="graphs/ports_graph.php?ports=<?php 
                   echo $NUM_HOSTS ?>&type=<?php echo $report_type ?>"/>
<?php
        }
?>
            </td>
          </tr>
        </table>
            
<?php

    }

    /* 
     * return the list of less stabe services
     */
    function less_stable_services()
    {
        global $NUM_HOSTS;
    
        /* opennms db connect */
        $opennms_db = new ossim_db();
        $opennms_conn = $opennms_db->opennms_connect();

        $query = OssimQuery("SELECT servicename, count(servicename) 
            FROM ifservices ifs, service s 
            WHERE ifs.serviceid = s.serviceid AND ifs.status = 'D' 
            GROUP BY servicename ORDER BY count(servicename) DESC 
            LIMIT $NUM_HOSTS");

        $rs = &$opennms_conn->Execute($query);
        
        if (!$rs) {
            print $opennms_conn->ErrorMsg();
        } else {
?>
        <h2>Top <?php echo "$NUM_HOSTS" ?> less stabe services</h2>
        <table align="center">
          <tr>
            <th> <?php echo gettext("Service"); ?> </th>
            <th> <?php echo gettext("Ocurrences"); ?> </th>
          </tr>
<?php
            while (!$rs->EOF) {

                $service = $rs->fields["servicename"];
                $occurrences = 
                    number_format($rs->fields["count"], 0, ",", ".");
?>
          <tr>
            <td><?php echo $service ?></td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
                $rs->MoveNext();
            }
        }
        $opennms_db->close($opennms_conn);
        echo "</table><br/>\n";
    }

?>
