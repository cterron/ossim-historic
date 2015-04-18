<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<?php
   require_once ('ossim_acl.inc');
   require_once ('ossim_db.inc');
   require_once ('classes/Net.inc');
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Insert new user</h1>


<form method="post" action="newuser.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <tr>
    <th>User login</th>
    <td class="left"><input type="text" name="user" size="30" /></td>
  </tr>
  <tr>
    <th>User full name</th>
    <td class="left"><input type="text" name="name" size="30" /></td>
  </tr>
  <tr>
    <th>Enter password</th>
    <td class="left"><input type="password" name="pass1" size="30" /></td>
  </tr>
  <tr>
    <th>Re-enter password</th>
    <td class="left"><input type="password" name="pass2" size="30" /></td>
  </tr>
  <tr>
    <th>Allowed nets<br/>
    </th>
    <td class="left">
<?php
    $db = new ossim_db();
    $conn = $db->connect();
    $i = 0;
    if ($nets = Net::get_list($conn)) {
        foreach ($nets as $net) {
?>
        <input type="checkbox" name="<?php echo "net" . $i ?>"
                value="<?php echo $net->get_name(); ?>" />
        <?php echo $net->get_name(); ?><br/>
<?php
            $i++;
        }
    }
?>
        <input type="hidden" name="nnets" value="<?php echo $i ?>" />
<?php
    $db->close($conn);
?>
    <i>NOTE: No selection allows ALL nets</i>
    </td>
  </tr>
</table>

<br/>
<table align="center">
  <tr>
    <th colspan="2">Permissions</th>
  </tr>
  <tr>
    <td colspan="2" class="left">
<?php
    foreach ($ACL_MAIN_MENU as $menus) {
        foreach ($menus as $key => $menu) {
?>
            <input type="checkbox" name="<?php echo $key ?>"
            <?php if ($menu["default_perm"]) echo "checked" ?>>
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

