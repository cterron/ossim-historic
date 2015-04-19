<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyHosts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Modify host group"); ?> </h1>

<?php
    require_once 'classes/Host.inc';
    require_once 'classes/Host_group.inc';
    require_once 'classes/Host_group_scan.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Host_group_reference.inc';
    require_once 'classes/RRD_config.inc';
    require_once 'classes/Security.inc';
    require_once 'classes/Sensor.inc';

    $name = GET('name');

    ossim_valid($name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("name"));

    if (ossim_error()) {
        die(ossim_error());
    }

    $db = new ossim_db();
    $conn = $db->connect();

    if ($host_group_list = Host_group::get_list($conn, "WHERE name = '$name'")) {
        $host_group = $host_group_list[0];
    }

?>

<form method="post" action="modifyhostgroup.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th> <?php echo gettext("Group Name"); ?> </th>
      <input type="hidden" name="name"
             value="<?php echo $host_group->get_name(); ?>">
      <td class="left">
        <b><?php echo $host_group->get_name(); ?></b>
      </td>
  </tr>

    <th> <?php echo gettext("Hosts"); ?> <br/>
        <font size="-2">
          <a href="newhostform.php"> <?php echo gettext("Insert new host"); ?> ?</a>
        </font>
    </th>
    <td class="left">
<?php

    /* ===== Hosts ==== */
    $i = 1;
    if ($host_list = Host::get_list($conn)) {
        foreach($host_list as $host) {
            $host_name = $host->get_hostname();
            $host_ips =   $host->get_ip();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "hhosts"; ?>"
            value="<?php echo count($host_list); ?>">
<?php
            }
            $name = "mboxs" . $i;
?>
        <input type="checkbox"
<?php
            if (Host_group_reference::in_host_group_reference
                                       ($conn, $host_group->get_name(), $host_ips))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $host_ips; ?>">
            <?php echo $host_name . " (" . $host_ips . ")<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>

  <tr>
    <th> <?php echo gettext("Threshold C"); ?> </th>
    <td class="left">
      <input type="text" name="threshold_c" size="4"
             value="<?php echo $host->get_threshold_c(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Threshold A"); ?> </th>
    <td class="left">
      <input type="text" name="threshold_a" size="4"
             value="<?php echo $host->get_threshold_a(); ?>"></td>
  </tr>
  <tr>
    <th> <?php echo gettext("RRD Profile"); ?> <br/>
        <font size="-2">
          <a href="../rrd_conf/new_rrd_conf_form.php"> <?php echo gettext("Insert new profile"); ?> ?</a>
        </font>
    </th>
    <td class="left">
      <select name="rrd_profile">
<?php
    foreach (RRD_Config::get_profile_list($conn) as $profile)
    {
        $host_profile = $host_group->get_rrd_profile();
        if (strcmp($profile, "global"))
        {
            $option = "<option value=\"$profile\"";
            if (0 == strcmp($host_profile, $profile))
                $option .= " SELECTED ";
            $option .= ">$profile</option>\n";
            echo $option;
        }
    }
?>
        <option value=""
            <?php if (!$host_profile) echo " SELECTED " ?>>
	    <?php echo gettext("None"); ?> </option>
      </select>
    </td>
  </tr>
  <tr>

  <tr>
    <th> <?php echo gettext("Scan options"); ?> </th>
    <td class="left">
      <input type="checkbox"
      <?php
      $name = $host_group->get_name();
      if(Host_group_scan::in_host_group_scan($conn, $name, 3001)){
          echo " CHECKED ";
      }
      ?>
      name="nessus" value="1"> Enable nessus scan </input>
    </td>
  </tr>

  <tr>
    <th> <?php echo gettext("Sensors"); ?> (*)<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">
          <?php echo gettext("Insert new sensor"); ?> ?</a>
        </font>
    </th>
    <td class="left">
<?php

    /* ===== sensors ==== */
    $i = 1;
    if ($sensor_list = Sensor::get_list($conn, "ORDER BY name")) {
        foreach($sensor_list as $sensor) {
            $sensor_name = $sensor->get_name();
            $sensor_ip =   $sensor->get_ip();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "nsens"; ?>"
            value="<?php echo count($sensor_list); ?>">
<?php
            }
            $name = "sboxs" . $i;
?>
        <input type="checkbox"
<?php
            if (Host_group_sensor_reference::in_host_group_sensor_reference
                                       ($conn, $host_group->get_name(), $sensor_name))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $sensor_name; ?>">
            <?php echo $sensor_ip . " (" . $sensor_name . ")<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>

  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr"
        rows="2" cols="20"><?php echo $host_group->get_descr(); ?></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>
<?php
    $db->close($conn);
?>

</body>
</html>

