<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Insert new sensor"); ?> </h1>

<?php

require_once 'classes/Security.inc';

$ip = GET('ip');

ossim_valid($ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("Sensor name"));

if (ossim_error()) {
    die(ossim_error());
}

?>

<form method="post" action="newsensor.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Hostname"); ?> </th>
    <td class="left"><input type="text" name="name"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("IP"); ?> </th>
    <td class="left"><input type="text" name="ip"
        value="<?php echo $ip;?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Priority"); ?> </th>
    <td class="left">
      <select name="priority">
        <option value="1">1</option>
        <option value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option selected value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
      </select>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Port"); ?> </th>
    <td class="left"><input type="text" value="40001" name="port"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
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

