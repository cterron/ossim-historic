<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

  <h2>Sensors</h2>

  <table align="center">
    <tr>
      <th>Hostname</ith>
      <th>Ip</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($sensor_list = Sensor::get_list($conn)) {
        foreach($sensor_list as $sensor) {
            $name = $sensor->get_name();
?>

    <tr>
      <td><?php echo $sensor->get_name(); ?></td>
      <td><?php echo $sensor->get_ip(); ?></td>
      <td><?php echo $sensor->get_descr(); ?></td>
      <td><a href="modifysensorform.php?name=<?php echo $name ?>">Modify</a>
          <a href="deletesensor.php?name=<?php echo $name ?>">Delete</a></td>
    </tr>

<?php
        } /* sensor_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="7"><a href="newsensorform.php">Insert new sensor</a></td>
    </tr>
  </table>
    
</body>
</html>

