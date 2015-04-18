<html>
<head>
  <title> Riskmeter </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Priority and Reliability configuration</h1>

<?php
    require_once 'ossim_db.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();
    
    if (!$order = $_GET["order"]) $order = "id";    
?>

    <table align="center">
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("id", $order); ?>">Id</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("name", $order); ?>">Name</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("type", $order); ?>">Type</a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("type", $order); ?>">Description</a></th>
      </tr>

<?php
    require_once 'classes/Plugin.inc';
    
    if ($plugin_list = Plugin::get_list($conn, "ORDER BY $order")) {
        foreach ($plugin_list as $plugin) {
            $id = $plugin->get_id();
            $type = $plugin->get_type();
?>
      <tr>
        <td><a href="pluginsid.php?id=<?php echo $id ?>">
            <?php echo $plugin->get_id(); ?></a></td>
        <td bgcolor="#eeeee"><b><?php echo $plugin->get_name(); ?></b></td>
        <td>
<?php
        if ($type == '1') {
            echo "Detector ($type)"; 
        } elseif ($type == '2') {
            echo "Monitor ($type)";
        } else {
            echo "Other ($type)";
        }
?>
        </td>
        <td><?php echo $plugin->get_description(); ?></td>
      </tr>
<?php
        }
    }
?>
    </table>

</body>

<?php
    $db->close($conn);
?>

</html>
