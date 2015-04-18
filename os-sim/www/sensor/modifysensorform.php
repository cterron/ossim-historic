<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify sensor</h1>

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
    <th>Priority</th>
    <td class="left">
      <select name="priority">
        <option
        <?php if ($sensor->get_priority() == 0) echo " SELECTED "; ?>
          value="0">0</option>
        <option
        <?php if ($sensor->get_priority() == 1) echo " SELECTED "; ?>
          value="1">1</option>
        <option
        <?php if ($sensor->get_priority() == 2) echo " SELECTED "; ?>
          value="2">2</option>
        <option
        <?php if ($sensor->get_priority() == 3) echo " SELECTED "; ?>
          value="3">3</option>
        <option
        <?php if ($sensor->get_priority() == 4) echo " SELECTED "; ?>
          value="4">4</option>
        <option
        <?php if ($sensor->get_priority() == 5) echo " SELECTED "; ?>
          value="5">5</option>
        <option
        <?php if ($sensor->get_priority() == 6) echo " SELECTED "; ?>
          value="6">6</option>
        <option
        <?php if ($sensor->get_priority() == 7) echo " SELECTED "; ?>
          value="7">7</option>
        <option
        <?php if ($sensor->get_priority() == 8) echo " SELECTED "; ?>
          value="8">8</option>
        <option
        <?php if ($sensor->get_priority() == 9) echo " SELECTED "; ?>
          value="9">9</option>
        <option
        <?php if ($sensor->get_priority() == 10) echo " SELECTED "; ?>
          value="10">10</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Port</th>
    <td class="left">
        <input type="text" name="port" 
               value="<?php echo $sensor->get_port(); ?>"></td>
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

