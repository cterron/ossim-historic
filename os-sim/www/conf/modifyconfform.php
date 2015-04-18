<html>
<head>
  <title> Riskmeter </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Framework</h1>
  <h2>Configuration</h2>

<?php
    require_once 'classes/Conf.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    $conf = Conf::get_conf($conn);
    
?>

  <table align="center">
  <form method="post" action="modifyconf.php">
    <tr><th colspan="2">General Options</th></tr>
    <tr>
      <td align="right">Recovery level</td>
      <td align="left">
        <input type="text" 
               value="<?php echo $conf->get_recovery() ?>"
               size="5" name="recovery">
      </td>
    </tr>
    <tr>
      <td align="right">Default threshold</td>
      <td align="left">
        <input type="text" 
               value="<?php echo $conf->get_threshold() ?>"
               size="5" name="threshold">
      </td>
    </tr>
    <tr>
      <td align="right">Graph default threshold</td>
      <td align="left">
        <input type="text" 
               value="<?php echo $conf->get_graph_threshold() ?>"
               size="5" name="graph_threshold">
      </td>
    </tr>
    <tr><th colspan="2"></th></tr>
    <tr>
      <td colspan="2">
        <a href="reload.php">Reload All</a>
      </td>
    </tr>
    <tr><th colspan="2"></th></tr>
    <tr><td colspan="2"><a href="../sensor/editsensor.php">Edit remote sensor</a></td></tr>
    <tr><th colspan="2"></th></tr>
    <tr><th colspan="2">Appearance</th></tr>
    <tr>
      <td align="right">Left Bar length</td>
      <td align="left">
        <input type="text" 
               value="<?php echo $conf->get_bar_length_left() ?>"
               size="5" name="bar_length_left">
      </td>
    </tr>
    <tr>
      <td align="right">Right Bar length</td>
      <td align="left">
        <input type="text" 
               value="<?php echo $conf->get_bar_length_right() ?>"
               size="5" name="bar_length_right">
      </td>
    </tr>
    <tr><th colspan="2"></th></tr>
    <tr>
       <td align="center" colspan="2"><input type="submit" value="OK"></td>
    </tr>
    </form> 
  </table>

</body>

<?php
    $db->close($conn);
?>

</html>
