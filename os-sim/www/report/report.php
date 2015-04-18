<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsHostReport");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Host report</h1>

<?php 
    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_os.inc';

    if (!$order = $_GET["order"]) $order = "hostname"; 
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("hostname", $order);
          ?>">Hostname</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">Ip</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php 
            echo ossim_db::get_order("asset", $order);
          ?>">Asset</a></th>
      <th>OS</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($host_list = Host::get_list($conn, "$search", "ORDER BY $order")) {
        foreach($host_list as $host) {
            $ip = $host->get_ip();

            if ($os_list = Host_os::get_list($conn, 
                                             "WHERE ip = inet_aton('$ip')")) {
                $os = $os_list[0]->get_os();
                $os_prev = $os_list[0]->get_previous();
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
        if (strcmp($os, $os_prev) && ($os)) {
            echo "&nbsp;<img src=\"../pixmaps/major.gif\"/>";
        }
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

