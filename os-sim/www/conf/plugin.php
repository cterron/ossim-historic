<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationPlugins");
require_once ('classes/Security.inc');
?>

<html>
<head>
  <title> <?php echo gettext("Priority and Reliability configuration"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Priority and Reliability configuration"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();
   

    $order = GET('order');
    ossim_valid($order, OSS_NULLABLE, OSS_SPACE,  OSS_SCORE, OSS_ALPHA, 'illegal:'._("order"));
    
    if (ossim_error()) {
        die(ossim_error());
    }
    
    if (empty($order)) $order = "id";    
?>

    <table align="center">
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("id", $order); ?>"> 
		<?php echo gettext("Id"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("name", $order); ?>"> 
		<?php echo gettext("Name"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("type", $order); ?>"> 
		<?php echo gettext("Type"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
                echo ossim_db::get_order("type", $order); ?>"> 
		<?php echo gettext("Description"); ?> </a></th>
      </tr>

<?php
    require_once 'classes/Plugin.inc';
    
    if ($plugin_list = Plugin::get_list($conn, "ORDER BY $order")) {
        foreach ($plugin_list as $plugin) {
        
            $id = $plugin->get_id();
           
            # 1505 => OSSIM directives
            # 2000 - 3000 => Monitors
#            if (($id != 1505) && (($id < 2000) || ($id >= 3000))) 
            if ($id != 1505) 
            {
                $name = $plugin->get_name();
                $type = $plugin->get_type();
?>
      <tr>
        <td>
        <a href="pluginsid.php?id=<?php echo $id ?>&name=<?php echo $name ?>">
            <?php echo $id; ?></a>
        </td>
        <td bgcolor="#eeeee"><b><?php echo $name; ?></b></td>
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
            } /* if 1505 */
        } /* foreach */
    } /* if plugin_list */
?>
    </table>

</body>

<?php
    $db->close($conn);
?>

</html>
