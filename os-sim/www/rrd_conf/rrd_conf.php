<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>RRD Config</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/RRD_config.inc';
    require_once 'classes/Host.inc';

    if (!$order = $_GET["order"]) $order = "inet_aton(ip)";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">Ip</a></th>
      <th>Action</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($rrd_list = RRD_config::get_ip_list($conn)) {
        foreach($rrd_list as $ip) {
?>
    <tr>
      <td>
<?php 
            if (!strcmp($ip, '0.0.0.0')) echo 'GLOBAL';
            else echo Host::ip2hostname($conn, $ip);
?>
      </td>
      <td>
        <a href="modify_rrd_conf_form.php?ip=<?php echo $ip  ?>">Modify</a>
<?php
            if (strcmp($ip, '0.0.0.0')) {
?>
        &nbsp;<a href="delete_rrd_conf.php?ip=<?php echo $ip  ?>">Delete</a>
<?php
            }
?>
       </td>
    </tr>
<?php
        }
    }
    

    $db->close($conn);
?>
    <tr>
      <td colspan="2"><a href="new_rrd_conf_form.php">Insert new rrd_conf</a></td>
    </tr>
  </table>
    
</body>
</html>

