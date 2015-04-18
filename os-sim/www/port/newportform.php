<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Insert new port group</h1>

<form method="post" action="newport.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Name</th>
    <td><input type="text" name="name"></td>
  </tr>
  <tr>
    <th>Ports</th>
    <td class="left">
<?php
    require_once 'classes/Port.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

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
        <input type="checkbox" name="<?php echo $name;?>"
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
    <th>Description</th>
    <td class="left">
      <textarea name="descr" rows="2" cols="20"></textarea>
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

