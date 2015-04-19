<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationRRDConfig");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("RRD Config"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/RRD_config.inc';
    require_once 'classes/Host.inc';

    if (!$order = $_GET["order"]) $order = "inet_aton(ip)";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("profile", $order);
          ?>">
	  <?php echo gettext("Profile"); ?> </a></th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php

    $db = new ossim_db();
    $conn = $db->connect();

    if ($rrd_list = RRD_config::get_profile_list($conn)) {
        foreach($rrd_list as $profile) {
?>
    <tr>
      <td><?php echo $profile ?></td>
      <td>
        <a href="modify_rrd_conf_form.php?profile=<?php
            echo $profile  ?>">
	    <?php echo gettext("Modify"); ?> </a>
<?php
            if (strcmp($profile, 'global')) {
?>
        &nbsp;<a href="delete_rrd_conf.php?profile=<?php
            echo $profile ?>">
	    <?php echo gettext("Delete"); ?> </a>
<?php
            }
?>
       </td>
    </tr>
<?php
        }
    }


    $db->close($conn);
?>
    <tr>
      <td colspan="2">
        <a href="new_rrd_conf_form.php"> <?php echo gettext("Insert new RRD profile"); ?> </a>
      </td>
    </tr>
  </table>

</body>
</html>

