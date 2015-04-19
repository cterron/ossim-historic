<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Insert new host scan configuration"); ?> </h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('classes/Plugin.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    $plugin_list = Plugin::get_list($conn, "WHERE id >= 3000 AND id < 4000");
?>

  <table align="center">
  <form method="post" action="newhostscan.php">

    <input type="hidden" name="insert" value="insert">
    <tr>
      <th><?php echo gettext("host IP"); ?></th>
      <td class="left"><input type="text" name="host_ip"></td>
    </tr>
    <tr>
      <th><?php echo gettext("Plugin id"); ?></th>
      <td class="left">
        <select name="plugin_id">
<?php
    if ($plugin_list) {
        foreach($plugin_list as $plugin) {
            echo "<option value=\"". $plugin->get_id() . 
                "\">" . $plugin->get_name() . "</option>";
        }
    }
?>
        </select>
      </td>
    </tr>
    <tr>
      <td colspan="2"><input type="submit" value="OK"/></td>
    </tr>
  
  </form>
  </table>
  
<?php
    $db->close($conn);
?>

</body>
</html>

