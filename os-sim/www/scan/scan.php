<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

  <h2>Scan</h2>

  <table align="center">
    <tr><th>Update Scan</th></tr>
    <tr><td><?php include('makescan.php'); ?></td></tr>
    <tr><th>Delete Scan</th></tr>
    <tr><td><?php include('deletescan.php'); ?></td></tr>
  </table>
  <br/>
  
  <table align="center">
    <tr>
      <th>Host</th>
      <th>Active</th>
      <th>Action</th>
    </tr>

<?php

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Scan.inc';

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($scan_list = Scan::get_list($conn)) {
        foreach($scan_list as $scan) {
            $ip = $scan->get_ip();
            $active = $scan->get_active(); 
?>

    <tr>
      <td><?php echo Host::ip2hostname($conn, $ip); ?></td>
      <td>
<?php 
        if ($active == 1) echo '<font color="green">Yes</font>';
        else echo '<font color="red">No</font>'
?>
      </td>
      <td>
<?php
        if (Host::in_host($conn, $ip)) {
          echo "Host is in DB";
        } else {
          echo "<a href=\"../host/newhostform.php?ip=$ip\">Insert in DB</a>";
        }
?>
      </td>
    </tr>

<?php
        } /* scan_list */
    } /* foreach */

    $db->close($conn);
?>
  </table>
    
</body>
</html>

