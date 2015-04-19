<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyServers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Insert new server"); ?> </h1>

<?php

require_once 'classes/Security.inc';

$ip = GET('ip');
$hostname = GET('hostname');

ossim_valid($ip, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("Server IP"));
ossim_valid($hostname, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Server Name"));

if (ossim_error()) {
    die(ossim_error());
}

?>

<form method="post" action="newserver.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Hostname"); ?> </th>
    <td class="left"><input type="text" name="name"
		value="<?php echo $hostname;?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("IP"); ?> </th>
    <td class="left"><input type="text" name="ip"
        value="<?php echo $ip;?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Port"); ?> </th>
    <td class="left"><input type="text" value="40001" name="port"></td>
  </tr>

<tr>
    <th> <?= _("Correlate events").required() ?> </th>
    <td class="left">
    <input type="radio" name="correlate" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="correlate" value="0" > <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?= _("Cross Correlate events").required() ?> </th>
    <td class="left">
    <input type="radio" name="cross_correlate" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="cross_correlate" value="0" > <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?= _("Store events").required() ?> </th>
    <td class="left">
    <input type="radio" name="store" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="store" value="0" > <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?= _("Qualify events").required() ?> </th>
    <td class="left">
    <input type="radio" name="qualify" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="qualify" value="0" > <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?= _("Resend alarms").required() ?> </th>
    <td class="left">
    <input type="radio" name="resend_alarms" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="resend_alarms" value="0" > <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?= _("Resend events").required() ?> </th>
    <td class="left">
    <input type="radio" name="resend_events" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="resend_events" value="0" > <?= _("No"); ?>
    </td>
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

