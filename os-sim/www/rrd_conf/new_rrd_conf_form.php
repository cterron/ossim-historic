<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationRRDConfig");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1><?php echo gettext("New RRD Profile"); ?></h1>

<h3><?php echo gettext("Hints"); ?></h3>
<ul>
<li> <?php echo gettext("Threshold: Absolute value above which is being alerted"); ?>.
<li> <?php echo gettext("Priority: Resulting impact if threshold is being exceeded"); ?>.
<li> <?php echo gettext("Alpha: Intercept adaption parameter"); ?>.
<li> <?php echo gettext("Beta: Slope adaption parameter"); ?>.
<li> <?php echo gettext("Persistence: How long has this event to last before we alert.")." (".gettext("Hours").")"; ?>
</ul>

<?php
    require_once 'classes/RRD_config.inc';
    require_once 'ossim_db.inc';

    $db = new ossim_db();
    $conn = $db->connect();
?>

    <form method="post" action="new_rrd_conf.php">

    <table align="center">
      <tr><th><?php echo gettext("Enter a profile name"); ?></th></tr>
      <tr><td><input type="text" name="profile"></td></tr>
    </table>
    <br/>
    <table align="center">
      <tr>
        <th><?php echo gettext("Attribute"); ?></th>
        <th><?php echo gettext("Threshold"); ?></th>
        <th><?php echo gettext("Priority"); ?></th>
        <th><?php echo gettext("Alpha"); ?></th>
        <th><?php echo gettext("Beta"); ?></th>
        <th><?php echo gettext("Persistence"); ?></th>
        <th><?php echo gettext("Enable"); ?></th>
      </tr>

<?php

    if ($rrd_global_list = RRD_Config::get_list($conn,
                                                "WHERE profile = 'Default'"))
    {
        foreach ($rrd_global_list as $global)
        {
            $attrib         = $global->get_rrd_attrib();
            $threshold      = $global->get_threshold();
            $priority       = $global->get_priority();
            $alpha          = $global->get_alpha();
            $beta           = $global->get_beta();
            $persistence    = $global->get_persistence();
?>
      <tr>
        <th><?php echo $attrib ?></th>
        <input type="hidden" name="<?php echo $attrib ?>#rrd_attrib"
            value="<?php echo $attrib ?>"/>
        <td><input type="text" name="<?php echo $attrib ?>#threshold"
            size="8" value="<?php echo $threshold ?>"></td>
        <td><input type="text" name="<?php echo $attrib ?>#priority"
            size="2" value="<?php echo $priority ?>"/></td>
        <td><input type="text" name="<?php echo $attrib ?>#alpha"
            size="8" value="<?php echo $alpha ?>"/></td>
        <td><input type="text" name="<?php echo $attrib ?>#beta"
            size="8" value="<?php echo $beta ?>"/></td>
        <td><input type="text" name="<?php echo $attrib ?>#persistence"
            size="2" value="<?php echo $persistence ?>"/></td>
        <td><input type="checkbox" name="<?php echo $attrib ?>#enable" checked/>
        </td>
      </tr>
<?php
        }
    }
    
    $db->close($conn);
?>

      <tr>
        <td colspan="7"><input type="submit" value="<?php echo gettext("Insert"); ?>"/></td>
      </tr>
    </table>
    </form>

</body>
</html>

