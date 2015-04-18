<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Users</h1>

<?php

    require_once ('ossim_db.inc');
    require_once ('classes/Session.inc');
    require_once ('ossim_acl.inc');

    if (!$order = $_GET["order"]) $order = "login";

?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("login", $order);
          ?>">Login</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">Name</a></th>
      <th>Password</th>
      <th>Actions</th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();

    if ($session_list = Session::get_list($conn, "ORDER BY $order")) {
        foreach ($session_list as $session) {
            $login = $session->get_login();
            $name  = $session->get_name();
            $pass  = "XXX";
?>
    <tr>
      <td><?php echo $login; ?></td>
      <td><?php echo $name; ?></td>
      <td><?php echo $pass; ?></td>
      <td>
      [<a href="changepassform.php?user=<?php echo $login ?>">Change Password</a>]
<?php
    if ($login != ACL_DEFAULT_OSSIM_ADMIN) {
?>
      [<a href="modifyuserform.php?user=<?php echo $login ?>">Update</a>]
      [<a href="deleteuser.php?user=<?php echo $login ?>">Delete</a>]
<?php
    }
?>
      </td>
    </tr>

<?php

        }
    }

?>
    <tr>
      <td colspan="4"><a href="newuserform.php">Insert new user</a></td>
    </tr>
  </table>

<?php
    $db->close($conn);
?>

</body>
</html>

