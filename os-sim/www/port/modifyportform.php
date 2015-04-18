<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Modify port group</h2>

<?php
    require_once 'classes/Port_group.inc';
    require_once 'classes/Port.inc';
    require_once 'classes/Port_group_reference.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$port_name = mysql_escape_string($_GET["portname"])) {
        echo "<p>Wrong port name</p>";
        exit;
    }

    if ($port_group_list = Port_group::get_list
            ($conn, "WHERE name = '$port_name'")) {
        $port_group = $port_group_list[0];
    }
?>
<form method="post" action="modifyport.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Name</th>
      <input type="hidden" name="name"
             value="<?php echo $port_group->get_name(); ?>">
    <td class="left">
      <b><?php echo $port_group->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th>Ports</th>
    <td class="left">
<?php

    $i = 1;
    if ($port_list = Port::get_list($conn)) {
        foreach ($port_list as $port) {
            if ($i == 1) {
?>
        <input type="hidden" name="nports"
            value="<?php echo count(Port::get_list($conn)); ?>">
<?php
            } $name = "mbox" . $i;
?>
        <input type="checkbox" 
<?php
            if (Port_group_reference::in_port_group_reference
                    ($conn, $port_group->get_name(), 
                            $port->get_port_number(),
                            $port->get_protocol_name()))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo
            $port->get_port_number() . "-" . $port->get_protocol_name(); ?>">
            <?php echo
            $port->get_port_number() . "-" . $port->get_protocol_name(); ?><br>
        </input>
<?php
            $i++;
        }
    }
    $db->close($conn);
?>
    </td>
  </tr>
  <tr>
    <th>Description:&nbsp;</th>
    <td class="left">
      <textarea name="descr" rows="2" 
        cols="20"><?php echo $port_group->get_descr();?></textarea>
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

