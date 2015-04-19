<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationPlugins");
?>

<html>
<head>
  <title> <?php echo gettext("Plugin Sid"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Plugin Sid"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Security.inc';

    $order = GET('order');
    $id = GET('id');
    $name = GET('name');

    ossim_valid($order, OSS_NULLABLE, OSS_SPACE,  OSS_SCORE, OSS_ALPHA , 'illegal:'._("order"));
    ossim_valid($id, OSS_ALPHA , 'illegal:'._("id"));
    ossim_valid($name, OSS_ALPHA , OSS_SCORE, OSS_NULLABLE, 'illegal:'._("name"));

    if (ossim_error()) {
        die(ossim_error());
    }
    
    if (empty($order)) $order = "sid";

    $db = new ossim_db();
    $conn = $db->connect();
    
    $title = $name . " ($id)";
    
    require_once 'classes/Plugin_sid.inc';
    require_once 'classes/Classification.inc';
    require_once 'classes/Category.inc';
?>

    <h2><?php echo $title ?></h2>

    <table align="center">
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("plugin_id", $order); ?>&id=<?php 
            echo $id ?>">
	    <?php echo gettext("Plugin"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("sid", $order); ?>&id=<?php 
            echo $id ?>"> 
	    <?php echo gettext("Sid"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("category_id", $order); ?>&id=<?php 
            echo $id ?>">
	    <?php echo gettext("Category"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("class_id", $order); ?>&id=<?php
            echo $id ?>">
	    <?php echo gettext("Class"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order); ?>&id=<?php 
            echo $id ?>">
	    <?php echo gettext("Name"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order); ?>&id=<?php 
            echo $id ?>">
	    <?php echo gettext("Priority"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("reliability", $order); ?>&id=<?php 
            echo $id ?>">
	    <?php echo gettext("Reliability"); ?> </a></th>
        <th> <?php echo gettext("Action"); ?> </th>
      </tr>

<?php
    
    if ($plugin_list = Plugin_sid::get_list($conn, "WHERE plugin_id = $id ORDER BY $order")) {
        foreach ($plugin_list as $plugin) {

            $id   = $plugin->get_plugin_id();
            $sid  = $plugin->get_sid();
            $name = $plugin->get_name();

            # translate class id
            if ($class_id = $plugin->get_class_id()) {
                if ($class_list = Classification::get_list($conn, "WHERE id = '$class_id'")) {
                    $class_name = $class_list[0]->get_name();
                }
            }

            # translate category id
            if ($category_id = $plugin->get_category_id()) {
                if ($category_list = Category::get_list($conn, "WHERE id = '$category_id'")) {
                    $category_name = $category_list[0]->get_name();
                }
            }
?>
      <tr>
        <td><?php echo $id; ?></td>
        <td><?php echo $sid; ?></td>
        
        <!-- category id -->
        <td nowrap>
<?php 
            if (!empty($category_name)) echo $category_name . " (". $category_id .")";
            else echo "-";
?> 
        </td>
        <!-- end category id -->

        <!-- class id -->
        <td nowrap>
<?php 
            if (!empty($class_name)) echo $class_name . " (". $class_id .")";
            else echo "-"; 
?> 
        </td>
        <!-- end class id -->
        
        <td bgcolor="#eeeeee"><?php echo $name; ?></td>
        <form method="post" action="pluginupdate.php">
            <input type="hidden" name="id" 
                value="<?php echo $id ?>"/>
            <input type="hidden" name="sid" 
                value="<?php echo $sid ?>"/>
        <td><input type="text" name="priority" size="2" 
            value="<?php echo $plugin->get_priority(); ?>"/></td>
        <td><input type="text" name="reliability" size="2" 
            value="<?php echo $plugin->get_reliability(); ?>"/></td>
        <td><input type="submit" value="Modify"/></td>
        </form>
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
