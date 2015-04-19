<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Change password"); ?> </h1>

<?php

    if (!$user = $_GET["user"]) {
        echo "<p align=\"center\">Wrong user</p>";
        exit;
    }

?>

<form method="post" action="changepass.php">
<table align="center">
  <input type="hidden" name="update" value="update" />
  <input type="hidden" name="user" value="<?php echo $user ?>" />
  <tr>
    <th> <?php echo gettext("User name"); ?> </th>
    <td><?php echo $user; ?></td>
  </tr>
  <tr>
    <td> <?php echo gettext("Current password"); ?> </td>
    <td class="left"><input type="password" name="oldpass" /></td>
  </tr>
  <tr>
    <td> <?php echo gettext("Enter new password"); ?> </td>
    <td class="left"><input type="password" name="pass1" /></td>
  </tr>
  <tr>
    <td> <?php echo gettext("Retype new password"); ?> </td>
    <td class="left"><input type="password" name="pass2" /></td>
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

