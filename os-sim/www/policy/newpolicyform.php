<?php
require_once 'classes/Security.inc';
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPolicy");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Insert new policy</h1>

<?php
    
    require_once ('classes/Host.inc');
    require_once ('classes/Net.inc');
    require_once ('classes/Port_group.inc');
    require_once ('classes/Plugingroup.inc');
    require_once ('classes/Sensor.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();
?>

</p>

<form method="post" action="newpolicy.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th><?=_("Source").required()?><br/>
        <font size="-2">
          <a href="../net/newnetform.php"><?=_("Insert new net?")?></a>
        </font><br/>
        <font size="-2">
          <a href="../host/newhostform.php"><?=_("Insert new host?")?></a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== source nets =====*/
    $j = 1;
    if ($net_list = Net::get_list($conn, "ORDER BY name")) {
        foreach ($net_list as $net) {
            $net_name = $net->get_name();
            if ($j == 1) {
?>
        <input type="hidden" name="<?php echo "sourcengrps"; ?>"
            value="<?php echo count($net_list); ?>">
<?php
            } $name = "sourcemboxg" . $j;
?>
        <input type="checkbox" name="<?php echo $name;?>"
            value="<?php echo $net_name; ?>">
            <?php echo $net_name ?><br>
        </input>
<?php
            $j++;
        }
    }
?>

<hr noshade>

<?php

    /* ===== source hosts ===== */
    $i = 1;
    if ($host_list = Host::get_list($conn, "", "ORDER BY hostname")) {
        foreach ($host_list as $host) {
            $ip       = $host->get_ip();
            $hostname = $host->get_hostname();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "sourcenips"; ?>"
            value="<?php echo count($host_list) + 1; ?>">
<?php
            }
            $name = "sourcemboxi" . $i;
?>
        <input type="checkbox" name="<?php echo $name; ?>"
            value="<?php echo $ip ?>">
            <?php echo $ip . ' (' .$hostname.")<br>"; ?>
        </input>
<?php
            $i++;
        }
    }
    $name = "sourcemboxi".$i;
?>
    <input type="checkbox" name="<?php echo $name; ?>"
           value="any">&nbsp;<b><?=_("ANY")?></b><br></input>



    </td>
  </tr>
  <tr>
    <th><?=_("Dest").required()?><br/>
        <font size="-2">
          <a href="../net/newnetform.php"><?=_("Insert new net?")?></a>
        </font><br/>
        <font size="-2">
          <a href="../host/newhostform.php"><?=_("Insert new host?")?></a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== dest nets =====*/
    $j = 1;
    if ($net_list = Net::get_list($conn, "ORDER BY name")) {
        foreach ($net_list as $net) {
            $net_name = $net->get_name();
            if ($j == 1) {
?>
        <input type="hidden" name="<?php echo "destngrps"; ?>"
            value="<?php echo count($net_list); ?>">
<?php
            } $name = "destmboxg" . $j;
?>
        <input type="checkbox" name="<?php echo $name;?>"
            value="<?php echo $net_name; ?>">
            <?php echo $net_name ?><br>
        </input>
<?php
            $j++;
        }
    }
?>

<hr noshade>

<?php

    /* ===== dest hosts ===== */
    $i = 1;
    if ($host_list =  Host::get_list($conn, "", "ORDER BY hostname")) {
        foreach ($host_list as $host) {
            $ip       = $host->get_ip();
            $hostname = $host->get_hostname();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "destnips"; ?>"
            value="<?php echo count($host_list) + 1; ?>">
<?php
            }
            $name = "destmboxi" . $i;
?>
        <input type="checkbox" name="<?php echo $name; ?>"
            value="<?php echo $ip ?>">
            <?php echo $ip . ' (' .$hostname.")<br>"; ?>
        </input>
<?php
            $i++;
        }
    }
    $name = "destmboxi".$i;
?>
    <input type="checkbox" name="<?php echo $name; ?>"
           value="any">&nbsp;<b><?=_("ANY")?></b><br></input>


    </td>
  </tr>

  <tr>
    <th><?=_("Ports").required()?><br/>
        <font size="-2">
          <a href="../port/newportform.php"><?=_("Insert new port group?")?></a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== ports ==== */
    $i = 1;
    if ($port_group_list = Port_group::get_list($conn, "ORDER BY name")) {
        foreach($port_group_list as $port_group) {
            $port_group_name = $port_group->get_name();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "nprts"; ?>"
            value="<?php echo count($port_group_list); ?>">
<?php
            }
            $name = "mboxp" . $i;
?>
        <input type="checkbox" name="<?php echo $name;?>"
            value="<?php echo $port_group_name; ?>">
            <?php echo $port_group_name . "<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>

  <tr>
    <th><?=_("Priority").required()?></th>
    <td class="left">
      <select name="priority">
        <option value="-1"><?= _("Do not change"); ?></option>
        <option value="0">0</option>
        <option value="1">1</option>
        <option selected value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
      </select>
    </td>
  </tr>

  <tr>
    <th> <?= _("Plugin Groups").required() ?> <br/>
        <font size="-2">
          <a href="../policy/modifyplugingroups.php">
      <?php echo gettext("Insert new plugin group"); ?>?
        </font><br/>
    </th>
    <td class="left">
<?
    /* ===== plugin groups ==== */
    foreach (Plugingroup::get_list($conn) as $g) {
?>
    <input type="checkbox" name="plugins[<?=$g->get_id()?>]"> <?=$g->get_name()?><br/>
<? } ?>

    </td>
  </tr>

  <tr>
    <th><?=_("Sensors").required()?><br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php"><?=_("Insert new sensor?")?></a>
        </font><br/>
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
            $name = "mboxs" . $i;
?>
        <input type="checkbox" name="<?php echo $name;?>"
            value="<?php echo $sensor_name; ?>">
            <?php echo $sensor_ip . " (" . $sensor_name . ")<br>";?>
        </input>
<?php
            $i++;
        }
    }

?>
    <input type="checkbox" name="<?php echo $name; ?>"
           value="any">&nbsp;<b><?=_("ANY")?></b><br></input>
    </td>
  </tr>

  <tr>
    <th><?= _("Time Range").required()?></th>
    <td>
      <table>
        <tr>
          <td><?=_("Begin")?></td><td></td><td><?=_("End")?></td>
        </tr>
        <tr>
          <td>
            <select name="begin_day">
              <option selected value="1"><?=_("Mon");?></option>
              <option value="2"><?=_("Tue");?></option>
              <option value="3"><?=_("Wed");?></option>
              <option value="4"><?=_("Thu");?></option>
              <option value="5"><?=_("Fri");?></option>
              <option value="6"><?=_("Sat");?></option>
              <option value="7"><?=_("Sun");?></option>
            </select>
            <select name="begin_hour">
              <option selected value="0">0h</option>
              <option value="1">1h</option>
              <option value="2">2h</option>
              <option value="3">3h</option>
              <option value="4">4h</option>
              <option value="5">5h</option>
              <option value="6">6h</option>
              <option value="7">7h</option>
              <option value="8">8h</option>
              <option value="9">9h</option>
              <option value="10">10h</option>
              <option value="11">11h</option>
              <option value="12">12h</option>
              <option value="13">13h</option>
              <option value="14">14h</option>
              <option value="15">15h</option>
              <option value="16">16h</option>
              <option value="17">17h</option>
              <option value="18">18h</option>
              <option value="19">19h</option>
              <option value="20">20h</option>
              <option value="21">21h</option>
              <option value="22">22h</option>
              <option value="23">23h</option>
            </select>
          </td>
          <td>-</td>
          <td>
            <select name="end_day">
              <option value="1"><?=_("Mon");?></option>
              <option value="2"><?=_("Tue");?></option>
              <option value="3"><?=_("Wed");?></option>
              <option value="4"><?=_("Thu");?></option>
              <option value="5"><?=_("Fri");?></option>
              <option value="6"><?=_("Sat");?></option>
              <option selected value="7"><?=_("Sun");?></option>
            </select>
            <select name="end_hour">
              <option value="0">0h</option>
              <option value="1">1h</option>
              <option value="2">2h</option>
              <option value="3">3h</option>
              <option value="4">4h</option>
              <option value="5">5h</option>
              <option value="6">6h</option>
              <option value="7">7h</option>
              <option value="8">8h</option>
              <option value="9">9h</option>
              <option value="10">10h</option>
              <option value="11">11h</option>
              <option value="12">12h</option>
              <option value="13">13h</option>
              <option value="14">14h</option>
              <option value="15">15h</option>
              <option value="16">16h</option>
              <option value="17">17h</option>
              <option value="18">18h</option>
              <option value="19">19h</option>
              <option value="20">20h</option>
              <option value="21">21h</option>
              <option value="22">22h</option>
              <option selected value="23">23h</option>
            </select>
          </td>
        </tr>
      </table>
    </td>
  </tr>
  <tr>
    <th> <?= _("Store events").required() ?> </th>
    <td class="left">
    <input type="radio" name="store" value="1" checked> <?= _("Yes"); ?>
    <input type="radio" name="store" value="0" > <?= _("No"); ?>
    </td>
  </tr>

  <tr>
    <th><?= _("Description").required() ?></th>
    <td class="left">
        <textarea name="descr" rows="2" cols="20"></textarea>
    </td>
  </tr>

<?php
    $db->close($conn);
?>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="<?php echo gettext('reset'); ?>">
    </td>
  </tr>
</table>
</form>

</body>
</html>

