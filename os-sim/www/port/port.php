<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Ports"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    require_once 'classes/Security.inc';

    $order = GET('order');

    ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("Order"));

    if (ossim_error()) {
        die(ossim_error());
    }

    if (empty($order)) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">
	  <?php echo gettext("Port group"); ?> </a></th>
      <th> <?php echo gettext("Ports"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>
<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($port_list = Port_group::get_list($conn)) {
        foreach (Port_group::get_list($conn, "ORDER BY $order") 
                 as $port_group) {
            $port_group_name = $port_group->get_name();
?>
    <tr>
      <td><?php echo $port_group_name; ?></td>
      <td>
<?php
    foreach ($port_group->get_reference_ports($conn, $port_group_name) 
             as $port) {
        echo $port->get_port_number() . "-" . 
             $port->get_protocol_name() . "<br>";
    }
?>
      </td>
      <td><?php echo $port_group->get_descr(); ?></td>
      <td>
        <a href="modifyportform.php?portname=<?php 
            echo $port_group->get_name()?>">
	    <?php echo gettext("Modify"); ?> </a>
        <a href="deleteport.php?portname=<?php
            echo $port_group->get_name()?>">
	    <?php echo gettext("Delete"); ?> </a></td>
    </tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="4" align="center">
        <a href="newportform.php"> <?php echo gettext("Insert new Port Group"); ?> </a><br/>
        <a href="newsingleportform.php"> <?php echo gettext("Insert new Port"); ?> </a>
      </td>
    </tr>
</table>
</body>
</html>

<?php
    $db->close($conn);
?>

