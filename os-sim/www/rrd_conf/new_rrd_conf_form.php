<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>New RRD Config</h1>

<h3>Hints</h3>
<ul>
<li> Threshold: Absolute value above which is being alerted.
<li> Priority: Resulting impact if threshold is being exceeded.
<li> Alpha: Intercept adaption parameter.
<li> Beta: Slope adaption parameter.
<li> Persistence: How long has this event to last before we alert. (Hours)
</ul>

<?php
    require_once 'classes/RRD_config.inc';
    require_once 'ossim_db.inc';

    $db = new ossim_db();
    $conn = $db->connect();
?>

    <form method="post" action="new_rrd_conf.php">
    
    <table align="center">
      <tr><th>Ip</th></tr>
      <tr><td><input type="text" name="ip"></td></tr>
    </table>
    <br/>
    <table align="center">
    
<?php

    if ($rrd_global_list = RRD_Config::get_list($conn, "WHERE ip = 0"))
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
      </tr>
<?php
        }
    }
    
    $db->close($conn);
?>

      <tr>
        <td colspan="6"><input type="submit" value="Insert"/></td>
      </tr>
    </table>
    </form>

</body>
</html>

