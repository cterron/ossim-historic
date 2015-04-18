<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Modify sensor</h2>

<?php
    require_once 'classes/Sensor.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$name = mysql_escape_string($_GET["name"])) {
        echo "<p>Wrong sensor</p>";
        exit;
    }
    
    if ($sensor_list = Sensor::get_list($conn, "WHERE name = '$name'")) {
        $sensor = $sensor_list[0];
    }

    $db->close($conn);
?>

<form method="post" action="modifysensor.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Hostname</th>
      <input type="hidden" name="name"
             value="<?php echo $sensor->get_name(); ?>">
    <td class="left">
      <b><?php echo $sensor->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th>IP</th>
    <td class="left">
        <input type="text" name="ip" 
               value="<?php echo $sensor->get_ip(); ?>"></td>
  </tr>
  <tr>
    <th>Description</th>
    <td class="left">
      <textarea name="descr" 
        rows="2" cols="20"><?php echo $sensor->get_descr(); ?></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>

