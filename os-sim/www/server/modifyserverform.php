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
                                                                                
  <h1> <?php echo gettext("Modify server"); ?> </h1>

<?php
    require_once 'classes/Server.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Security.inc';

    $name = GET('name');

    ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_SCORE, 'illegal:'._("Server name"));

    if (ossim_error()) {
        die(ossim_error());
    }

    $db = new ossim_db();
    $conn = $db->connect();

    if ($server_list = Server::get_list($conn, "WHERE name = '$name'")) {
        $server = $server_list[0];
    }

    if ($role_list = Role::get_list($conn, $name)) {
        $role = $role_list[0];
    }

    $db->close($conn);
?>

<form method="post" action="modifyserver.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Hostname"); ?> </th>
      <input type="hidden" name="name"
             value="<?php echo $server->get_name(); ?>">
    <td class="left">
      <b><?php echo $server->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("IP"); ?> </th>
    <td class="left">
        <input type="text" name="ip" 
               value="<?php echo $server->get_ip(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Port"); ?> </th>
    <td class="left">
        <input type="text" name="port" 
               value="<?php echo $server->get_port(); ?>"></td>
  </tr>
<?php
?>
  <tr>
    <th> <?php echo gettext("Correlate events"); ?> </th>
    <td class="left">
    <input type="radio" name="correlate" value="1" <?php if($role->get_correlate() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="correlate" value="0" <?php if($role->get_correlate() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Cross Correlate events"); ?> </th>
    <td class="left">
    <input type="radio" name="cross_correlate" value="1" <?php if($role->get_cross_correlate() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="cross_correlate" value="0" <?php if($role->get_cross_correlate() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Store events"); ?> </th>
    <td class="left">
    <input type="radio" name="store" value="1" <?php if($role->get_store() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="store" value="0" <?php if($role->get_store() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Qualify events"); ?> </th>
    <td class="left">
    <input type="radio" name="qualify" value="1" <?php if($role->get_qualify() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="qualify" value="0" <?php if($role->get_qualify() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Resend alarms"); ?> </th>
    <td class="left">
    <input type="radio" name="resend_alarms" value="1" <?php if($role->get_resend_alarm() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="resend_alarms" value="0" <?php if($role->get_resend_alarm() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("Resend events"); ?> </th>
    <td class="left">
    <input type="radio" name="resend_events" value="1" <?php if($role->get_resend_event() == 1) echo " checked "; ?>> <?= _("Yes"); ?>
    <input type="radio" name="resend_events" value="0" <?php if($role->get_resend_event() == 0) echo " checked "; ?>> <?= _("No"); ?>
    </td>
  </tr>
  <?php
  

?>

  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
        <textarea name="descr" rows="2"
            cols="20"><?php echo $server->get_descr(); ?></textarea>
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

