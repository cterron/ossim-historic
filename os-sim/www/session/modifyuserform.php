<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");

require_once ('ossim_acl.inc');
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Modify user"); ?> </h1>

<?php
    require_once ("classes/Security.inc");
    
    $user  = GET('user');
    $networks = GET('networks'); 
    $sensors = GET('sensors');
    $perms = GET('perms');


    ossim_valid($user, OSS_USER, 'illegal:'._("User name"));
    ossim_valid($networks, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("networks"));
    ossim_valid($sensors, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("sensors"));
    ossim_valid($perms, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("perms"));
   
    if (ossim_error()) {
        die(ossim_error());
    }
                                
    

    function check_perms($user, $mainmenu, $submenu)
    {
        $gacl = $GLOBALS['ACL'];
        return $gacl->acl_check($mainmenu,
                                $submenu,
                                ACL_DEFAULT_USER_SECTION,
                                $user);
    }

    require_once ('classes/Session.inc');
    require_once ('classes/Net.inc');
    require_once ('classes/Sensor.inc');
    require_once ('ossim_db.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    if ($user_list = Session::get_list($conn, "WHERE login = '$user'")) {
        $user = $user_list[0];
    }

    $net_list = Net::get_all($conn);
    $sensor_list = Sensor::get_all($conn, "ORDER BY name ASC");
?>

<form method="post" action="modifyuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <input type="hidden" name="user" value="<?php echo $user->get_login() ?>" />
  <tr>
    <th> <?php echo gettext("User login"); ?> </th>
    <th class="left"><?php echo $user->get_login(); ?></th>
    <th>&nbsp;</th>
  </tr>
  <tr>
    <th> <?php echo gettext("User name"); ?> </th>
    <td class="left"><input type="text" name="name"
        value="<?php echo $user->get_name(); ?>" /></td>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
  <tr>
    <th> <?php echo gettext("User email"); ?> </th>
    <td class="left"><input type="text" name="email"
        value="<?php echo $user->get_email(); ?>" /></td>
    <th>&nbsp;</th>
  </tr>
  <tr>
    <th> <?php echo gettext("Company"); ?> </th>
    <td class="left"><input type="text" name="company"
        value="<?php echo $user->get_company(); ?>" /></td>
    <th>&nbsp;</th>
  </tr>
  <tr>
    <th> <?php echo gettext("Department"); ?> </th>
    <td class="left"><input type="text" name="department"
        value="<?php echo $user->get_department(); ?>" /></td>
    <th>&nbsp;</th>
  </tr>
</table>
  <br/>
  <table align="center">
  <tr>
    <th> <?php echo gettext("Allowed nets"); ?> </th>
    <th> <?php echo gettext("Allowed sensors"); ?> </th>
    <th colspan="2"> <?php echo gettext("Permissions"); ?> </th>
  </tr><tr>
    <td class="left" valign="top">
<?php
if($networks){
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&networks=0" .  "&sensors=" .  $sensors . "&perms=" . $perms; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&networks=1" .  "&sensors=" .  $sensors . "&perms=" . $perms; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
}
?>
<?php
    $i = 0;
    foreach ($net_list as $net) {
        $net_name = $net->get_name();
        $input = "<input type=\"checkbox\" name=\"net$i\" value=\"" .
                 $net_name ."\"";
        
        if (false !== strpos(Session::allowedNets($user->get_login()), $net->get_ips()))
        {
            $input .= " checked ";
        }
        if($networks) {
    	  $input .= " checked ";
        }
        $input .= "/>$net_name<br/>";
        echo $input;
        $i++;
    }
?>
      <input type="hidden" name="nnets" value="<?php echo $i ?>" />
      <i>NOTE: No selection allows ALL nets</i>
    </td>
    <td class="left" valign="top">
<?php
if($sensors){
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&sensors=0" .  "&networks=" .  $networks . "&perms=" . $perms; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&sensors=1" .  "&networks=" .  $networks. "&perms=" . $perms; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
}
?>

<?php
    $i = 0;
    foreach ($sensor_list as $sensor) {
        $sensor_name = $sensor->get_name();
        $sensor_ip = $sensor->get_ip();
        $input = "<input type=\"checkbox\" name=\"sensor$i\" value=\"" .
                 $sensor_ip ."\"";
        if (false !== strpos(Session::allowedSensors($user->get_login()),
                             $sensor_ip))
        {
            $input .= " checked ";
        }
        if ($sensors) {
        	$input .= " checked ";
        }
        $input .= "/>$sensor_name<br/>";
        echo $input;
        $i++;
    }
?>
      <input type="hidden" name="nsensors" value="<?php echo $i ?>" />
      <i>NOTE: No selection allows ALL sensors</i>
    </td>
    <td colspan="2" class="left" valign="top">
<?php
if($perms){
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&perms=0" .  "&networks=" .  $networks . "&sensors=" . $sensors; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
} else {
?>
<a href="<?php echo $_SERVER["PHP_SELF"] . "?user=" . $user->get_login() .  "&perms=1" .  "&networks=" .  $networks. "&sensors=" . $sensors; ?>"><?php echo gettext("Select / Unselect all");?></a>
<hr noshade>
<?php
}
?>

<?php
    foreach ($ACL_MAIN_MENU as $mainmenu => $menus) {
        foreach ($menus as $key => $menu) {
?>
            <input type="checkbox" name="<?php echo $key ?>"
            <?php if (check_perms($user->get_login(), $mainmenu, $key)) 
                echo " checked ";
		if($perms) echo " checked ";
            ?>>
<?php
            echo $menu["name"] . "<br/>\n";
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
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>

