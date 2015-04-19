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

  <h1> <?php echo gettext("Security report"); ?> </h1>

<?php
    
    require_once ('ossim_db.inc');
    require_once ('ossim_conf.inc');
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
    if ($_GET["section"] == 'attacked') 
    {
        ip_max_occurrences("ip_dst");
    }

    ##############################
    # Top attacker hosts
    ##############################
    elseif ($_GET["section"] == 'attacker') {
        ip_max_occurrences("ip_src");
    }

    ##############################
    # Top alerts received
    ##############################
    elseif ($_GET["section"] == 'alerts_recv') {
        alert_max_occurrences();
    }

    ##############################
    # Top alerts risk
    ##############################
    elseif ($_GET["section"] == 'alerts_risk') {
        alert_max_risk();
    }

    ##############################
    # Top used destination ports
    ##############################
    elseif ($_GET["section"] == 'dest_ports') {
        port_max_occurrences();
    }

    /* Top data traffic */
    elseif ($_GET["section"] == 'traffic') {
        echo "Working on...";
    }

    /* Top throughput */
    elseif ($_GET["section"] == 'throughput') {
        echo "Working on...";
    }

    /* Top used services */
    elseif ($_GET["section"] == 'services') {
        echo "Working on...";
    }

    ###############################
    # Top less stable services 
    ###############################
    elseif ($_GET["section"] == 'availability') {
        less_stable_services();
    }

    elseif ($_GET["section"] == 'all') {
        ip_max_occurrences("ip_dst");
        echo "<br/><br/>";
        ip_max_occurrences("ip_src");
        echo "<br/><br/>";
        port_max_occurrences();
        echo "<br/><br/>";
        alert_max_occurrences();
        echo "<br/><br/>";
        alert_max_risk();
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
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");
        $report_graph_type = $conf->get_conf("report_graph_type");
        
        if (!strcmp($target, "ip_src"))
            $title = "Attacker hosts";
        elseif (!strcmp($target, "ip_dst"))
            $title = "Attacked hosts";
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
            $list = $security_report->AttackHost($target, $NUM_HOSTS);
            foreach ($list as $l) {

                $ip = $l[0];
                $occurrences = number_format($l[1], 0, ",", ".");
                $hostname = Host::ip2hostname(
                    $security_report->ossim_conn, $ip);
                $os_pixmap = Host_os::get_os_pixmap(
                    $security_report->ossim_conn, $ip);
                $link = "$acid_link/acid_stat_alerts.php?&" . 
                    "num_result_rows=-1&" .
                    "submit=Query+DB&" . 
                    "current_view=-1&" .
                    "ip_addr[0][1]=$target&" . 
                    "ip_addr[0][2]==&" . 
                    "ip_addr[0][3]=$ip&" . 
                    "ip_addr_cnt=1&" . 
                    "sort_order=time_d";
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
                 echo $target ?>&hosts=<?php echo $NUM_HOSTS ?>" 
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
     * return the alert with max occurrences
     */
    function alert_max_occurrences()
    {
        global $NUM_HOSTS;
        global $security_report;
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");
        $report_graph_type = $conf->get_conf("report_graph_type");
?>
        <h2>Top <?php echo "$NUM_HOSTS Alerts" ?></h2>
        <table align="center">
          <tr>
            <th> <?php echo gettext("Alert"); ?> </th>
            <th> <?php echo gettext("Occurrences"); ?> </th>
          </tr>
<?php
            $list = $security_report->Alerts();
            foreach ($list as $l)
            {
                $alert = $l[0];
                $short_alert = SecurityReport::Truncate($alert,60);
                $occurrences = number_format($l[1], 0, ",", ".");
?>
          <tr>
             <?php
               $link = "$acid_link/acid_qry_main.php?new=1&" . 
                    "sig[0]==&" . 
                    "sig[1]=$alert&" . 
                    "sig[2]==&" . 
                    "submit=Query+DB&" . 
                    "num_result_rows=-1&" . 
                    "sort_order=time_d";
             ?>
            <td><a href="<?php echo $link ?>"><?php echo $short_alert ?></a></td>
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
            jgraph_nbalerts_graph();
        } else {
?>
            <img src="graphs/alerts_received_graph.php?hosts=<?php 
                 echo $NUM_HOSTS ?>" alt="alerts graph"/>
<?php
        }
?>
          </td>
        <tr/>
        </table>
<?php
    }

    
    /* 
     * return a list of alerts ordered by risk
     */
    function alert_max_risk()
    {
        global $NUM_HOSTS;
        global $security_report;
        require_once ('sec_util.php');
?>
        <h2>Top <?php echo "$NUM_HOSTS Alerts by Risk" ?></h2>
        <table align="center">
          <tr>
            <th>Alert</th>
            <th>Risk</th>
          </tr>
<?php
            $list = $security_report->AlertsByRisk();
            foreach ($list as $l) {
                $alert = $l[0];
                $risk  = $l[1];
?>
          <tr>
            <td><?php echo $alert ?></a></td>
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
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");
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
            $list = $security_report->Ports();
            foreach ($list as $l)
            {

                $port = $l[0];
                $service = $l[1];
                $occurrences = number_format($l[2], 0, ",", ".");
?>
          <tr>
            <td>
              <?php 
                $link = "$acid_link/acid_stat_uaddr.php?" . 
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
                   echo $NUM_HOSTS ?>"/>
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
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");

        /* opennms db connect */
        $opennms_db = new ossim_db();
        $opennms_conn = $opennms_db->opennms_connect();

        $query = "SELECT servicename, count(servicename) 
            FROM ifservices ifs, service s 
            WHERE ifs.serviceid = s.serviceid AND ifs.status = 'D' 
            GROUP BY servicename ORDER BY count(servicename) DESC 
            LIMIT $NUM_HOSTS;";

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
