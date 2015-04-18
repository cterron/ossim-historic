<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

  <h2>Networks</h2>

  <table align="center">
    <tr>
      <th>Name</th>
      <th>Ips</th>
      <th>Asset</th>
      <th>Threshold C</th>
      <th>Threshold A</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($net_list = Net::get_list($conn, "", "ORDER BY name")) {
        foreach($net_list as $net) {
            $name = $net->get_name();
?>

    <tr>
      <td><?php echo $net->get_name(); ?></td>
      <td><?php echo $net->get_ips(); ?></td>
      <td><?php echo $net->get_priority(); ?></td>
      <td><?php echo $net->get_threshold_c(); ?></td>
      <td><?php echo $net->get_threshold_a(); ?></td>
      <td><?php echo $net->get_descr(); ?></td>
      <td><a href="modifynetform.php?name=<?php echo $name ?>">Modify</a>
          <a href="deletenet.php?name=<?php echo $name ?>">Delete</a></td>
    </tr>

<?php
        } /* net_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="7"><a href="newnetform.php">Insert new network</a></td>
    </tr>
  </table>
    
</body>
</html>

