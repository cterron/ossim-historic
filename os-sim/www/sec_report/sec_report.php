<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Security report</h1>

<?php
    
    require_once ('ossim_db.inc');
    require_once ('ossim_conf.inc');
    
    require_once ('classes/Host_qualification.inc');
    require_once ('classes/Host.inc');
    require_once ('classes/Host_os.inc');


    $server = $_SERVER["SERVER_ADDR"];
    $file   = $_SERVER["REQUEST_URI"];
    print "<p align=\"center\">
        <a href=\"topdf.php?url=http://$server/$file\">Generate Report</a>
        </p>";

    /* database connect */
    $db = new ossim_db();
    $conn = $db->connect();


    /* Number of hosts to show */
    $NUM_HOSTS = 10;


    ##############################
    #  Top C & A Risk 
    ##############################
    if ($_GET["section"] == 'risk') {
        ip_max_risk();
    }
    
    ##############################
    # Top attacked hosts
    ##############################
    elseif ($_GET["section"] == 'attacked') 
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
        ip_max_risk();
        ip_max_occurrences("ip_dst");
        ip_max_occurrences("ip_src");
        alert_max_occurrences();
        alert_max_risk();
        port_max_occurrences();
        less_stable_services();
    }

    $db->close($conn);
?>
   
</body>
</html>


<?php

    function ip_max_risk() {

        global $conn;
        global $NUM_HOSTS;
    
        if ($risk_list = Host_qualification::get_list
            ($conn, "", "ORDER BY compromise + attack DESC LIMIT $NUM_HOSTS"))
        {
?>
            <h2>Top <?php echo $NUM_HOSTS ?> Risk Metrics</h2>
            <table align="center">
                <tr>
                  <th>Host</th>
                  <th>Compromise</th>
                  <th>Attack</th>
                </tr>
<?php
            foreach ($risk_list as $host) 
            {
                $ip = $host->get_host_ip();
                $hostname = Host::ip2hostname($conn, $ip);
                $link = "../report/metrics.php?host=$ip";
                $os = Host_os::get_os_pixmap($conn, $ip);
?>
                <tr>
                  <td><?php echo "<a href=\"$link\">$hostname</a> $os" ?></td>
                  <td><?php echo $host->get_compromise() ?></td>
                  <td><?php echo $host->get_attack() ?></td>
                </tr>
<?php
            }
            echo "</table><br/>\n";
        }
    }

    /* 
     * return the list of host with max occurrences 
     * as dest or source
     * pre: type is "ip_src" or "ip_dst"
     */
    function ip_max_occurrences($target)
    {
        global $NUM_HOSTS;
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");

        /* snort db connect */
        $snort_db = new ossim_db();
        $snort_conn = $snort_db->snort_connect();
        
        $query = "SELECT count($target) AS occurrences, inet_ntoa($target) 
            FROM acid_event GROUP BY $target
            ORDER BY occurrences DESC LIMIT $NUM_HOSTS;";

        if (!$rs = &$snort_conn->CacheExecute($query)) {
            print $snort_conn->ErrorMsg();
        } else {
        
            if (!strcmp($target, "ip_src"))
                $title = "Attacker hosts";
            elseif (!strcmp($target, "ip_dst"))
                $title = "Attacked hosts";
?>
        <h2>Top <?php echo "$NUM_HOSTS $title" ?></h2>
        <table align="center">
          <tr>
            <th>Host</th>
            <th>Occurrences</th>
          </tr>
<?php
            while (!$rs->EOF) {

                $ip = $rs->fields["inet_ntoa($target)"];
                $hostname = ip2hostname($ip);
                $os_pixmap = get_os_pixmap($ip);
                $occurrences = $rs->fields["occurrences"];
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
              <a href="<?php echo $link ?>"><?php echo $hostname ?></a>
              <?php echo $os_pixmap ?>
            </td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
                $rs->MoveNext();
            }
        }
        $snort_db->close($snort_conn);
        echo "</table><br/>\n";
    }

    /* 
     * return the alert with max occurrences
     */
    function alert_max_occurrences()
    {
        global $NUM_HOSTS;
    
        /* snort db connect */
        $snort_db = new ossim_db();
        $snort_conn = $snort_db->snort_connect();
        
        $query = "SELECT count(sig_name) AS occurrences, sig_name
            FROM acid_event GROUP BY sig_name
            ORDER BY occurrences DESC LIMIT $NUM_HOSTS;";

        if (!$rs = &$snort_conn->CacheExecute($query)) {
            print $snort_conn->ErrorMsg();
        } else {
        
?>
        <h2>Top <?php echo "$NUM_HOSTS Alerts" ?></h2>
        <table align="center">
          <tr>
            <th>Alert</th>
            <th>Occurrences</th>
          </tr>
<?php
            while (!$rs->EOF) {
                $alert = $rs->fields["sig_name"];
                $occurrences = $rs->fields["occurrences"];
?>
          <tr>
            <td><?php echo $alert ?></a></td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
                $rs->MoveNext();
            }
        }
        $snort_db->close($snort_conn);
        echo "</table><br/>\n";
    }

    
    /* 
     * return a list of alerts ordered by risk
     */
    function alert_max_risk()
    {
        global $NUM_HOSTS;
    
        /* snort db connect */
        $snort_db = new ossim_db();
        $snort_conn = $snort_db->snort_connect();
        
        $query = "SELECT sig_name, ossim_risk_a 
            FROM acid_event 
            GROUP BY sig_name 
            ORDER BY ossim_risk_a DESC LIMIT $NUM_HOSTS;";

        if (!$rs = &$snort_conn->CacheExecute($query)) {
            print $snort_conn->ErrorMsg();
        } else {
        
?>
        <h2>Top <?php echo "$NUM_HOSTS Alerts by Risk" ?></h2>
        <table align="center">
          <tr>
            <th>Alert</th>
            <th>Risk</th>
          </tr>
<?php
            while (!$rs->EOF) {
                $alert = $rs->fields["sig_name"];
                $risk  = $rs->fields["ossim_risk_a"];
?>
          <tr>
            <td><?php echo $alert ?></a></td>
            <td><?php echo $risk ?></td>
          </tr>
<?php
                $rs->MoveNext();
            }
        }
        $snort_db->close($snort_conn);
        echo "</table><br/>\n";
    }

    /* 
     * return the list of ports with max occurrences 
     */
    function port_max_occurrences()
    {
        global $NUM_HOSTS;
    
        /* ossim framework conf */
        $conf = new ossim_conf();
        $acid_link = $conf->get_conf("acid_link");

        /* snort db connect */
        $snort_db = new ossim_db();
        $snort_conn = $snort_db->snort_connect();
        
        $query = "SELECT count(layer4_dport) AS occurrences, layer4_dport 
            FROM acid_event GROUP BY layer4_dport
            ORDER BY occurrences DESC LIMIT $NUM_HOSTS;";

        if (!$rs = &$snort_conn->CacheExecute($query)) {
            print $snort_conn->ErrorMsg();
        } else {
        
?>
        <h2>Top <?php echo "$NUM_HOSTS" ?> Used Ports</h2>
        <table align="center">
          <tr>
            <th>Port</th>
            <th>Occurrences</th>
          </tr>
<?php
            while (!$rs->EOF) {

                $port = $rs->fields["layer4_dport"];
                $occurrences = $rs->fields["occurrences"];
?>
          <tr>
            <td><?php echo $port ?></td>
            <td><?php echo $occurrences ?></td>
          </tr>
<?php
                $rs->MoveNext();
            }
        }
        $snort_db->close($snort_conn);
        echo "</table><br/>\n";
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
            <th>Service</th>
            <th>Ocurrences</th>
          </tr>
<?php
            while (!$rs->EOF) {

                $service = $rs->fields["servicename"];
                $occurrences = $rs->fields["count"];
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


    function ip2hostname($ip) {

        $db = new ossim_db();
        $conn = $db->connect();
        $hostname = Host::ip2hostname($conn, $ip);
        $db->close($conn);

        return $hostname;
    }

    function get_os_pixmap($ip) {
        $db = new ossim_db();
        $conn = $db->connect();
        $os = Host_os::get_os_pixmap($conn, $ip);
        $db->close($conn);
        
        return $os;
    }
?>
