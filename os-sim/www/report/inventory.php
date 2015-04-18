<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php 
    if (!$ip = $_GET["host"]) { 
        echo "<p>Wrong ip</p>";
        exit;
    }
?>

<h1>Inventory - <?php echo $ip ?></h1>

<?php

    require_once 'ossim_db.inc';
    require_once 'ossim_conf.inc';
    require_once 'classes/Host.inc';
    
    require_once 'classes/Host_os.inc';
    require_once 'classes/Host_mac.inc';
    require_once 'classes/Host_services.inc';
    require_once 'classes/Host_netbios.inc';
    require_once 'classes/Net.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();

    /* services update */
    if ($_GET["origin"] == 'active' && $_GET["update"] == 'services') 
    {
        $conf = new ossim_conf();
        $nmap = $conf->get_conf("nmap_path");

        $ip = escapeshellcmd($ip);
        $services = shell_exec("$nmap -sV -P0 $ip");
        $lines = split("[\n\r]", $services);
        Host_services::delete($conn, $ip);
        foreach ($lines as $line) {
            preg_match ('/(\S+)\s+open\s+([\w\-\_\?]+)(\s+)?(.*)$/', $line, $regs);
            if ($regs[0]) {
                list($port, $protocol) = explode("/", $regs[1]);
                $protocol = getprotobyname($protocol);
                if ($protocol == -1) {
                $protocol = 0; 
                } else {
                }
                $service = $regs[2];
                $service_type = $regs[2];
                $version = $regs[4];
                $origin = 1;
                $date_formatted = strftime("%Y-%m-%d %H:%M:%S");
                Host_services::insert($conn, $ip, $port, $protocol, $service, $service_type, $version, $date_formatted, $origin); // origin = 0 (pads), origin = 1 (nmap)
            }
        }
    }
?>
    <table align="center">
      <tr><td colspan="2"></td></tr>
      <tr><th colspan="2">Host Info</th></tr>
<?php

    if ($host_list = Host::get_list($conn, "WHERE ip = '$ip'")) {
        $host = $host_list[0];

        $sensor_list = $host->get_sensors($conn);
?>
      <tr>
        <th>Name</th>
        <td><?php echo $host->hostname ?></td>
      </tr>

<?php
    }
?>
      <tr>
        <th>Ip</th>
        <td><b><?php echo $ip ?></b></td>
      </tr>
<?php
    if ($os_list = Host_os::get_list($conn, "WHERE ip = inet_aton('$ip')")) {
        $os = $os_list[0];
?>
      <tr>
        <th>Operating System</th>
        <td>
<?php 
            echo $os->os . " ";
            echo Host_os::get_os_pixmap($conn, $os->get_ip()); 
?>
        </td>
      </tr>
<?php
    }
?>

<?php

    if ($mac_list = Host_mac::get_list($conn, "WHERE ip = inet_aton('$ip')")) {
        $mac = $mac_list[0];
?>
      <tr>
        <th>MAC</th>
        <td><?php echo $mac->get_mac() ?></td>
      </tr>
<?php
    }
?>
    
      
<?php    
    if ($netbios_list = Host_netbios::get_list($conn, "WHERE ip = '$ip'")) {
        $netbios = $netbios_list[0];
?>
      <tr>
        <th>Netbios Name</th>
        <td><?php echo $netbios->name ?></td>
      </tr>
      <tr>
        <th>Netbios Work Group</th>
        <td><?php echo $netbios->wgroup ?></td>
      </tr>
<?php
    }
?>
      <tr><td colspan="2"></td></tr>
      <tr><td colspan="2"></td></tr>
      <tr><th colspan="2">Host belongs to:</td></tr>

<?php
    if ($net_list = Net::get_list($conn))
    {
        foreach ($net_list as $net) {
            if (Net::isIpInNet($ip, $net->get_ips())) {
?>
      <tr>
        <th>Net</th>
        <td><?php echo $net->get_name() ?></td>
      </tr>
<?php
            }
        }
    }

    if ($sensor_list) {
        foreach ($sensor_list as $sensor) {
?>
      <tr>
        <th>Sensor</th>
        <td><?php echo $sensor->get_sensor_name() ?></td>
      </tr>
<?php
        }
    }
?>


      <tr><td colspan="2"></td></tr>
      <tr><td colspan="2"></td></tr>
      <tr><th colspan="2">Port / Service information
      <?php if($_GET["origin"] == 'active'){
      ?>
      (<A HREF="<?php echo $_SERVER["PHP_SELF"]?>?host=<?php echo $ip?>&origin=passive">Active</A>)
      [ <a href="<?php 
        echo $_SERVER["PHP_SELF"]?>?host=<?php 
        echo $ip ?>&update=services&origin=active">update</a> ]
        </th></h2>
      </tr>
        <?php } else { ?>
      (<A HREF="<?php echo $_SERVER["PHP_SELF"]?>?host=<?php echo $ip?>&origin=active">Passive</A>)
        </th></h2>
        <?php } ?>
      <tr>
      <td colspan="2">
      <table>
      <tr>
        <th>Service</th>
        <th>Version</th>
        <th>Date</th>
      </tr>
<?php
    if($_GET["origin"] == 'active'){
    if ($services_list = Host_services::get_list($conn, "WHERE ip = inet_aton('$ip') and origin = 1")) {
        foreach ($services_list as $services) {
?>
      <tr>
        <td><?php echo $services->get_service() . " (" .
            $services->get_port(). "/" .
            getprotobynumber($services->get_protocol()) . ")" ?></td>
        <td><?php echo $services->get_version() ?></td>
        <td><?php echo $services->get_date() ?></td>
      </tr>
<?php
        }
    }
    } else {
    if ($services_list = Host_services::get_list($conn, "WHERE ip = inet_aton('$ip') and origin = 0")) {
        foreach ($services_list as $services) {
?>
      <tr>
        <td><?php echo $services->get_service() . " (" .
            $services->get_port(). "/" .
            getprotobynumber($services->get_protocol()) . ")" ?></td>
        <td><?php echo $services->get_version() ?></td>
        <td><?php echo $services->get_date() ?></td>
      </tr>
<?php
        }
    }
    }
?>
      </table>
      </td>
      </tr>
    </table>

<?php
    $db->close($conn);
?>

</body>
</html>

