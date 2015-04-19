<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Users"); ?> </h1>

<?php

    require_once ('ossim_db.inc');
    require_once ('classes/Session.inc');
    require_once ('ossim_acl.inc');
    require_once ('classes/Security.inc');

$order = GET('order');

ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));

if (ossim_error()) {
    die(ossim_error());
}

if (empty($order)) $order = "login";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("login", $order);
          ?>">
	  <?php echo gettext("Login"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>"> 
	  <?php echo gettext("Name"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("email", $order);
          ?>">
	  <?php echo gettext("Email"); ?> </a></th>
      <th> <?php echo gettext("Password"); ?> </th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("company", $order);
          ?>">
      <?php echo gettext("Company"); ?> </a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("department", $order);
          ?>">
	  <?php echo gettext("Department"); ?> </a></th>
      <th> <?php echo gettext("Actions"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();

    if ($session_list = Session::get_list($conn, "ORDER BY $order")) {
        foreach ($session_list as $session) {
            $login = $session->get_login();
            $name  = $session->get_name();
            $email = $session->get_email();
            $pass  = "...";
            $company = $session->get_company();
            $department = $session->get_department();
?>
    <tr>
      <td><?php echo $login; ?></td>
      <td><?php echo $name; ?></td>
      <td><?php echo $email; ?>&nbsp;</td>
      <td><?php echo $pass; ?></td>
      <td><?php echo $company; ?>&nbsp;</td>
      <td><?php echo $department; ?>&nbsp;</td>
       <td>
      [<a href="changepassform.php?user=<?php echo $login ?>">
      <?php echo gettext("Change Password"); ?> </a>]
<?php
    if ( Session::am_i_admin() )
    {
        if ($login != ACL_DEFAULT_OSSIM_ADMIN) {
?>
      [<a href="modifyuserform.php?user=<?php echo $login ?>"> 
      <?php echo gettext("Update"); ?> </a>]
      [<a href="deleteuser.php?user=<?php echo $login ?>"> 
      <?php echo gettext("Delete"); ?> </a>]
<?php
    } elseif ($login == ACL_DEFAULT_OSSIM_ADMIN) {
?>
      [<a href="modifyuserform.php?user=<?php echo $login ?>"> 
      <?php echo gettext("Update"); ?> </a>]
<?php
        }
    }
?>
      </td>
    </tr>

<?php

        }
    }

    if ( Session::am_i_admin() )
    {
?>
    <tr>
      <td colspan="7"><a href="newuserform.php"> <?php echo gettext("Insert new user"); ?> </a></td>
    </tr>
    <tr>
      <td colspan="7"><a href="../setup/ossim_acl.php"> <?php echo gettext("Reload ACLS"); ?> </a></td>
    </tr>
<?php
    }
?>
  </table>

<?php
    $db->close($conn);
?>

</body>
</html>

