<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Host report"); ?> </h1>

<?php 
    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_os.inc';

    $order = GET('order') ? GET('order') : 'hostname';
    if (!ossim_valid($order, OSS_ALPHA . OSS_SPACE . OSS_PUNC, 'ilegal:'._("Order"))) {
        die(ossim_error());
    }
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("hostname", $order);
          ?>">
	  <?php echo gettext("Hostname"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">
	  <?php echo gettext("Ip"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("asset", $order);
          ?>">
	  <?php echo gettext("Asset"); ?> </a></th>
      <th> <?php echo gettext("OS"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($host_list = Host::get_list($conn, '', "ORDER BY $order")) {
        foreach($host_list as $host) {
            $ip = $host->get_ip();

            if ($os_data = Host_os::get_ip_data($conn,$ip)) {
                $os = $os_data["os"];
            } else {
                $os = "";
            }
?>

    <tr>
      <td><a href="../report/index.php?host=<?php 
        echo $ip ?>"><?php echo $host->get_hostname(); ?></a></td>
      <td><?php echo $host->get_ip(); ?></td>
      <td><?php echo $host->get_asset(); ?></td>
      <td>
        <?php 
        echo "$os ";
        echo Host_os::get_os_pixmap($conn, $host->get_ip());
        ?>
      </td>

    </tr>

<?php
        } /* host_list */
    } /* foreach */

    $db->close($conn);
?>
  </table>
    
</body>
</html>

