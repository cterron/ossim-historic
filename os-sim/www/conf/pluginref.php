<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationCorrelation");
?>

<html>
<head>
  <title> Riskmeter </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Plugin reference</h1>

<?php
    require_once 'ossim_db.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();
    
    if (!$order = $_GET["order"]) $order = "plugin_id";
    
    require_once 'classes/Plugin_reference.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Plugin_sid.inc';
    
    if (!$inf = $_GET["inf"])
        $inf = 0;
    if (!$sup = $_GET["sup"])
        $sup = 25;

?>

    <table align="center" width="100%">
      <tr>
        <td colspan="4">
<?php
    /* 
     * prev and next buttons 
     */
    $inf_link = $_SERVER["PHP_SELF"] . 
            "?order=$order" . 
            "&sup=" . ($sup - 25) .
            "&inf=" . ($inf - 25);
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + 25) .
        "&inf=" . ($inf + 25);
    $count = Plugin_reference::get_count($conn);
    
    if ($inf >= 25) {
        echo "<a href=\"$inf_link\">&lt;- Prev 25</a>";
    }
    echo "&nbsp;&nbsp;($inf-$sup of $count)&nbsp;&nbsp;";
    if ($sup < $count) {
        echo "<a href=\"$sup_link\">Next 25 -&gt;</a>";
    }
?>
        </td>
      </tr>
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("plugin_id", $order) . 
            "&inf=$inf&sup=$sup" ?>">Plugin id</a>
        </th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("plugin_sid", $order) .
            "&inf=$inf&sup=$sup" ?>">Plugin sid</a>
        </th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("reference_id", $order) .
            "&inf=$inf&sup=$sup" ?>">Reference id</a>
        </th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("reference_sid", $order) .
            "&inf=$inf&sup=$sup"?>">Reference sid</a>
        </th>
      </tr>

<?php
    
    if ($pluginref_list = Plugin_reference::get_list($conn, 
                                                     "ORDER BY $order",
                                                     $inf, $sup)) {
        foreach ($pluginref_list as $plugin) {

            $id   = $plugin->get_plugin_id();
            $sid  = $plugin->get_plugin_sid();
            $ref_id = $plugin->get_reference_id();
            $ref_sid = $plugin->get_reference_sid();

            # translate id
            if ($plugin_list = Plugin::get_list($conn, "WHERE id = $id")) {
                $plugin_name = $plugin_list[0]->get_name();
            }

            # translate sid
            if ($plugin_sid_list = Plugin_sid::get_list($conn, 
                "WHERE plugin_id = $id AND sid = $sid")) 
            {
                $plugin_sid_name = $plugin_sid_list[0]->get_name();
            }
            
            # translate ref id
            if ($plugin_list = Plugin::get_list($conn, "WHERE id = $ref_id")) {
                $plugin_ref_name = $plugin_list[0]->get_name();
            }

            # translate ref sid
            if ($plugin_sid_list = Plugin_sid::get_list($conn, 
                "WHERE plugin_id = $ref_id AND sid = $ref_sid")) 
            {
                $plugin_ref_sid_name = $plugin_sid_list[0]->get_name();
            }
?>
      <tr>
        <td><?php echo $plugin_name; ?></td>
        <td><?php echo $plugin_sid_name; ?></td>
        <td><?php echo $plugin_ref_name; ?></td>
        <td><?php echo $plugin_ref_sid_name; ?></td>
        
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
