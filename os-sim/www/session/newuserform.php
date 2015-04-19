<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<?php
    require_once ('ossim_acl.inc');
    require_once ('ossim_db.inc');
    require_once ('classes/Net.inc');
    require_once ('classes/Sensor.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    $net_list = Net::get_all($conn);
    $sensor_list = Sensor::get_all($conn, "ORDER BY name ASC");
    $db->close($conn);
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>

</head>
<body>

  <h1> <?= _("Insert new user") ?> </h1>

<?php

    $user  = GET('user');
    $pass1 = GET('pass1');
    $pass2 = GET('pass2');
    $name  = GET('name');
    $email = GET('email');
    $company = GET('company');
    $department = GET('department');
    $networks   = GET('networks');
    $sensors = GET('sensors');
    $perms = GET('perms');
    $copy_panels = GET('copy_panels');
    
    ossim_valid($user, OSS_USER, OSS_NULLABLE, 'illegal:'._("User name"));
    ossim_valid($copy_panels, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Copy panels"));
    ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Name"));
    ossim_valid($email, OSS_MAIL_ADDR, OSS_NULLABLE, 'illegal:'._("e-mail"));
    ossim_valid($company, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE, 'illegal:'._("Company"));
    ossim_valid($department, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE,  'illegal:'._("Department"));

    if (ossim_error()) {
            die(ossim_error());
    }
 
    $all = $defaults = array();
?>

<form method="post" action="newuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <tr>
    <th> <?= _("User login").required() ?></th>
    <td class="left">
        <input type="text" id="1" name="user" value="<?=$user?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("User full name").required() ?> </th>
    <td class="left">
        <input type="text" id="2" name="name" value="<?=$name?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("Email").required() ?> <img src="../pixmaps/email_icon.gif"></th>
    <td class="left">
        <input type="text" id="3" name="email" value="<?=$email?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("Enter password").required() ?> </th>
    <td class="left">
        <input type="password" id="4" name="pass1" value="<?=$pass1?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("Re-enter password").required() ?> </th>
    <td class="left">
        <input type="password" id="5" name="pass2" value="<?=$pass2?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("Company") ?> </th>
    <td class="left">
        <input type="text" id="6" name="company" value="<?=$company?>" size="30" />
    </td>
  </tr>
  <tr>
    <th> <?= _("Department") ?> </th>
    <td class="left">
        <input type="text" id="7" name="department" value="<?=$department?>" size="30" />
    </td>
  </tr>
<tr>
<th><?= _("Pre-set executive panels to admin panels") ?></th>
    <td align="center">
   <input type="radio" name="copy_panels" value="1" checked> <?= _("Yes"); ?>
   <input type="radio" name="copy_panels" value="0" > <?= _("No"); ?> 
    </td>
</tr>
<tr>
  <td>&nbsp;</td>
  <td align="center">
    <input type="submit" value="OK">
    <input type="reset" value="<?php echo gettext("reset"); ?>">
  </td>
</tr>
 </table>
  <br/>
  <table align="center">
  <tr>
    <th><?= _("Allowed nets") ?></th>
    <th><?= _("Allowed sensors") ?></th>
    <th colspan="2"> <?= _("Permissions") ?> </th>
</tr><tr>
    <td class="left" valign="top">


<a href="#" onClick="return selectAll('nets');"><?= _("Select / Unselect all")?></a>
<hr noshade>

<?php
    $i = 0;
    foreach ($net_list as $net) {
        $all['nets'][] = "net" . $i;
?>
        <input type="checkbox" id="<?= "net" . $i ?>" name="<?= "net" . $i ?>"
               value="<?= $net->get_name(); ?>" /><?= $net->get_name()?><br/>
<?
        $i++;
    }
?>
        <input type="hidden" name="nnets" value="<?php echo $i ?>" />
        <i><?php echo gettext("NOTE: No selection allows ALL")." ".gettext("nets"); ?></i>
    </td>
    <td class="left" valign="top">

<a href="#" onClick="return selectAll('sensors');"><?= _("Select / Unselect all");?></a>
<hr noshade>

<?php
    $i = 0;
    foreach ($sensor_list as $sensor) {
        $all['sensors'][] = "sensor".$i;
?>
        <input type="checkbox" id="<?= "sensor".$i ?>" name="<?= "sensor".$i ?>"
               value="<?= $sensor->get_ip() ?>" /><?=$sensor->get_name()?><br/> 
<?
        $i++;
    }
?>
        <input type="hidden" name="nsensors" value="<?php echo $i ?>" />
        <i><?php echo gettext("NOTE: No selection allows ALL")." ".gettext("sensors"); ?></i>
    </td>
    <td colspan="2" class="left" valign="top">

<a href="#" onClick="return selectAll('perms');"><?= _("Select / Unselect all");?></a>
&nbsp;-&nbsp;<a href="#" onClick="return selectAll('perms', true);"><?= _("Back to Defaults");?></a>
<hr noshade>

<?php
    foreach ($ACL_MAIN_MENU as $menus) {
        foreach ($menus as $key => $menu) {
            $all['perms'][] = $key;
?>
            <input type="checkbox" id="<?= $key ?>" name="<?= $key ?>"
            <? if ($menu["default_perm"]) {
                  echo " checked ";
                  $defaults['perms'][$key] = true;
               } else {
                  $defaults['perms'][$key] = false;
               }
            ?>
            ><?=$menu["name"]?><br/>
<?
        }
        echo "<hr noshade>";
    }
?>
    </td>
  </tr>
</table>

<br/>
<table align="center">
  <tr>
    <td colspan="2" align="center" valign="top">
      <input type="submit" value="OK">
      <input type="reset" value="<?php echo gettext("reset"); ?>">
    </td>
  </tr>
</table>
</form>
<script>

var check_nets    = true; // if true next click on "Select/Unselect" puts all to checked
var check_sensors = true;
var check_perms   = true;

function selectAll(category, defaults)
{
    if (category == 'perms' && !defaults) {
    <? foreach ($all['perms'] as $id) { ?>
        document.getElementById('<?=$id?>').checked = check_perms;
    <? } ?>
        check_perms = check_perms == false ? true : false;
    }
    if (category == 'perms' && defaults) {
    <? foreach ($defaults['perms'] as $id => $check) { ?>
        document.getElementById('<?=$id?>').checked = <?=$check ? 'true' : 'false'?>;
    <? } ?>
    }
    if (category == 'sensors') {
    <? foreach ($all['sensors'] as $id) { ?>
        document.getElementById('<?=$id?>').checked = check_sensors;
    <? } ?>
        check_sensors = check_sensors == false ? true : false;
    }
    if (category == 'nets') {
    <? foreach ($all['nets'] as $id) { ?>
        document.getElementById('<?=$id?>').checked = check_nets;
    <? } ?>
        check_nets = check_nets == false ? true : false;
    }    
    return false;
}
            
</script>  
</body>
</html>
