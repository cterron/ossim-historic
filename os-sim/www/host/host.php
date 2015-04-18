<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

  <h2>Hosts</h2>

  <table align="center">
    <tr>
      <th>Hostname</th>
      <th>Ip</th>
      <th>Asset</th>
      <th>Threshold C</th>
      <th>Threshold A</th>
      <th>Description</th>
      <th>Action</th>
    </tr>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($host_list = Host::get_list($conn, "", "ORDER BY hostname")) {
        foreach($host_list as $host) {
            $ip = $host->get_ip();
?>

    <tr>
      <td><?php echo $host->get_hostname(); ?></td>
      <td><?php echo $host->get_ip(); ?></td>
      <td><?php echo $host->get_asset(); ?></td>
      <td><?php echo $host->get_threshold_c(); ?></td>
      <td><?php echo $host->get_threshold_a(); ?></td>
      <td><?php echo $host->get_descr(); ?></td>
      <td><a href="modifyhostform.php?ip=<?php echo $ip ?>">Modify</a>
          <a href="deletehost.php?ip=<?php echo $ip ?>">Delete</a></td>
    </tr>

<?php
        } /* host_list */
    } /* foreach */

    $db->close($conn);
?>
    <tr>
      <td colspan="7"><a href="newhostform.php">Insert new host</a></td>
    </tr>
  </table>
    
</body>
</html>

