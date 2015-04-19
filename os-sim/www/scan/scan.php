<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsScan");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
  
  <h1>Scan</h1>

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
    require_once 'classes/Security.inc';

    $order = GET('order');
    
    ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
    
    if (ossim_error()) {
        die(ossim_error());
    }

    if (empty($order))  $order = "inet_aton(ip)";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">
	  <?php echo gettext("Host"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("active", $order);
          ?>">
	  <?php echo gettext("Active"); ?> </a></th>
      <th> <?php echo gettext("Action"); ?> </th>
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

