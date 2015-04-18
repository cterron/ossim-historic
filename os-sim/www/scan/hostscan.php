<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Host scan configuration</h1>

<?php
     if (!$order = $_GET["order"]) 
        $order = "inet_aton(host_ip)";
        
    require_once ('ossim_db.inc');
    
    $db = new ossim_db();
    $conn = $db->connect();
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
        echo ossim_db::get_order("host_ip", $order);
        ?>">Host</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
        echo ossim_db::get_order("plugin_id", $order);
        ?>">Plugin id</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
        echo ossim_db::get_order("plugin_sid", $order);
        ?>">Plugin sid</a></th>
      <th>Action</th>
    </tr>
    
<?php
    require_once ('classes/Host_scan.inc');
    require_once ('classes/Plugin.inc');

    if ($host_list = Host_scan::get_list($conn, "ORDER BY $order")) {
        foreach ($host_list as $host) {
            $ip = $host->get_host_ip();

            $id = $host->get_plugin_id();
            if ($plugin_list = Plugin::get_list($conn, "WHERE id = $id")) {
                $plugin_name = $plugin_list[0]->get_name();
            }
?>
    <tr>
      <td><?php echo $ip ?></td>
      <td><?php echo $plugin_name . " (". $id .")"; ?></td>
      <td>
          <?php if ($sid = $host->get_plugin_sid()) echo $sid; 
                else echo "ANY" ?>
      </td>
      <td>
          <a href="deletehostscan.php?host_ip=<?php echo $ip
          ?>&plugin_id=<?php echo $id ?>">Delete</a>
      </td>
    </tr>
<?php
        }
    }
    $db->close($conn);
?>
    <tr>
      <td colspan="4"><a href="newhostscanform.php">New</a></td>
    </tr>
  </table>
  
</body>
</html>

