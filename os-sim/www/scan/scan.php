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
  
<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Scan.inc';

    if (!$order = $_GET["order"]) $order = "ip";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("ip", $order);
          ?>">Host</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("active", $order);
          ?>">Active</a></th>
      <th>Action</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($scan_list = Scan::get_list($conn, "ORDER BY $order")) {
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

