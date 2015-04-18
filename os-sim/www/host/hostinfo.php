<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Framework</h1>
  <h2>Host Info</h2>

<?php 
    if (!$ip = $_GET["ip"]) { 
?>
    <p>Wrong ip</p>
<?php 
        exit;
    }


    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    
    require_once 'classes/Host_os.inc';
    require_once 'classes/Host_services.inc';
    require_once 'classes/Host_netbios.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();

?>
    <table align="center">
      <tr>
        <th>Ip</th>
        <th>Name</th>
      </tr>
<?php

    $host_list = Host::get_list($conn, "WHERE ip = '$ip'");
    $host = $host_list[0];
?>
      <tr>
        <td><?php echo $host->ip ?></td>
        <td><?php echo $host->hostname ?></td>
      </tr>
    </table>

    <h2>Operative System</h2>
    <table align="center">
      <tr>
        <th colspan="2">Operative System</th>
      </tr>
<?php

    if ($os_list = Host_os::get_list($conn, "WHERE ip = '$ip'")) {
        $os = $os_list[0];
?>
      <tr>
        <td><?php echo $os->os ?></td>
      </tr>
<?php
    }
?>
    </table>
    
    <h2>Active services and aplications names/versions</h2>
    <table align="center">
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
    
    <h2>Samba Info</h2>
    <table align="center">
      <tr>
        <th>Name</th>
        <th>Work Group</th>
      </tr>
      
<?php    
    if ($netbios_list = Host_netbios::get_list($conn, "WHERE ip = '$ip'")) {
        $netbios = $netbios_list[0];
?>
      <tr>
        <td><?php echo $netbios->name ?></td>
        <td><?php echo $netbios->wgroup ?></td>
      </tr>
<?php
    }
?>
    </table>
<?php
    $db->close($conn);
?>


</body>
</html>

