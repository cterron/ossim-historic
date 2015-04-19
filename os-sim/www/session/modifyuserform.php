<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<?php
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
    /* check user arg */
    if (!$user = $_GET["user"]) {
        echo "Wrong User!";
        exit;
    }

    function check_perms($user, $mainmenu, $submenu)
    {
        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");
        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl = new gacl();

        return $gacl->acl_check($mainmenu,
                                $submenu,
                                ACL_DEFAULT_USER_SECTION,
                                $user);
    }

    require_once ('classes/Session.inc');
    require_once ('classes/Net.inc');
    require_once ('ossim_db.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    if ($user_list = Session::get_list($conn, "WHERE login = '$user'")) {
        $user = $user_list[0];
    }
?>

<form method="post" action="modifyuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <input type="hidden" name="user" value="<?php echo $user->get_login() ?>" />
  <tr>
    <th> <?php echo gettext("User login"); ?> </th>
    <th class="left"><?php echo $user->get_login(); ?></th>
  </tr>
  <tr>
    <th> <?php echo gettext("User name"); ?> </th>
    <td class="left"><input type="text" name="name"
        value="<?php echo $user->get_name(); ?>" /></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Allowed nets"); ?> </th>
    <td class="left">
<?php
    $i = 0;
    if ($nets = Net::get_list($conn)) {
        foreach ($nets as $net) {
            $net_name = $net->get_name();
            $input = "
      <input type=\"checkbox\" name=\"net$i\" value=\"" .
        $net_name ."\"";
            if (false !== strpos(Session::allowedNets($user->get_login()),
                                 $net->get_ips()))
            {
                $input .= " checked ";
            }
            $input .= "/>$net_name<br/>";
            echo $input;
            $i++;
        }
    }
?>

    <input type="hidden" name="nnets" value="<?php echo $i ?>" />
    <i>NOTE: No selection allows ALL nets</i>
  </tr>
</table>

<br/>
<table align="center">
  <tr>
    <th colspan="2"> <?php echo gettext("Permissions"); ?> </th>
  </tr>
  <tr>
    <td colspan="2" class="left">
<?php
    foreach ($ACL_MAIN_MENU as $mainmenu => $menus) {
        foreach ($menus as $key => $menu) {
?>
            <input type="checkbox" name="<?php echo $key ?>"
            <?php if (check_perms($user->get_login(), $mainmenu, $key)) 
                echo "checked" 
            ?>>
<?php
            echo $menu["name"] . "<br/>\n";
        }
        echo "</td></tr><tr><td class=\"left\" colspan=\"2\">";
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

