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
    if ($_GET["update"] == 'services') 
    {
        $conf = new ossim_conf();
        $nmap = $conf->get_conf("nmap_path");

        $ip_sane = escapeshellcmd($ip);
        $services = shell_exec("$nmap -sV $ip");
        $lines = split("[\n\r]", $services);
        Host_services::delete($conn, $ip);
        foreach ($lines as $line) {
            preg_match ('/open\s+([\w\-\_\?]+)(\s+)?(.*)$/', $line, $regs);
            if ($regs[0]) {
                $service = $regs[1];
                $version = $regs[3];
                Host_services::insert($conn, $ip, $service, $version);
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
      <tr><th colspan="2">Active services and aplications names/versions 
      [ <a href="<?php 
        echo $_SERVER["PHP_SELF"]?>?host=<?php 
        echo $ip ?>&update=services">update</a> ]</th></h2>
      <tr>
        <th>Service</th>
        <th>Version</th>
      </tr>
    
<?php

    if ($services_list = Host_services::get_list($conn, "WHERE ip = '$ip'")) {
        foreach ($services_list as $services) {
?>
      <tr>
        <td><?php echo $services->service ?></td>
        <td><?php echo $services->version ?></td>
      </tr>
<?php
        }
    }
?>
    </table>

<?php
    $db->close($conn);
?>

</body>
</html>

