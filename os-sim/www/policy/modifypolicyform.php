<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Insert new policy</h2>

<?php
    
    require_once ('classes/Policy.inc');
    require_once ('classes/Host.inc');
    require_once ('classes/Net.inc');
    require_once ('classes/Port_group.inc');
    require_once ('classes/Signature_group.inc');
    require_once ('classes/Sensor.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();


    if (!$id = $_GET["id"]) {
        echo "<p>Wrong policy id</p>";
        exit;
    }

    settype($id, "int");
    
    if ($policy_list = Policy::get_list($conn, "WHERE id = $id")) {
        $policy = $policy_list[0];
    }
?>

</p>

<form method="post" action="modifypolicy.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <input type="hidden" name="id" value="<?php echo $id ?>">
  <tr>
    <th>Source<br/>
        <font size="-2">
          <a href="../host/newhostform.php">Insert new host?</a>
        </font><br/>
        <font size="-2">
          <a href="../net/newnetform.php">Insert new net?</a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== source hosts ===== */
    $i = 1;
    if ($host_list = Host::get_list($conn)) {
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
        <input type="checkbox" 
<?php
            if (Policy_host_reference::in_policy_host_reference
                                                    ($conn, $id, $ip, "source"))
            {
                echo " CHECKED ";
            }
?>
               name="<?php echo $name; ?>"
               value="<?php echo $ip ?>">
            <?php echo $ip . ' (' .$hostname.")<br>"; ?>
        </input>
<?php
            $i++;
        }
    }
    $name = "sourcemboxi".$i;
?>
    <input type="checkbox" 
<?php
            if (Policy_host_reference::in_policy_host_reference
                                                    ($conn, $id, 'any',
                                                    'source'))
            {
                echo " CHECKED ";
            }
?>
           name="<?php echo $name; ?>"
           value="any"><b>ANY</b><br></input>


<?php

    /* ===== source nets =====*/
    $j = 1;
    if ($net_list = Net::get_list($conn)) {
        foreach ($net_list as $net) {
            $net_name = $net->get_name();
            if ($j == 1) {
?>
        <input type="hidden" name="<?php echo "sourcengrps"; ?>"
            value="<?php echo count($net_list); ?>">
<?php
            } $name = "sourcemboxg" . $j;
?>
        <input type="checkbox" 
<?php
            if (Policy_net_reference::in_policy_net_reference
                                                    ($conn, $id, $net_name,
                                                    'source'))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $net_name; ?>">
            <?php echo $net_name . "<br>";?>
        </input>
<?php
            $j++;
        }
    }
?>

    </td>
  </tr>
  <tr>
    <th>Dest<br/>
        <font size="-2">
          <a href="../host/newhostform.php">Insert new host?</a>
        </font><br/>
        <font size="-2">
          <a href="../net/newnetform.php">Insert new net?</a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== dest hosts ===== */
    $i = 1;
    if ($host_list = Host::get_list($conn)) {
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
        <input type="checkbox" 
<?php
            if (Policy_host_reference::in_policy_host_reference
                                                    ($conn, $id, $ip,
                                                    "dest"))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name; ?>"
            value="<?php echo $ip ?>">
            <?php echo $ip . ' (' .$hostname.")<br>"; ?>
        </input>
<?php
            $i++;
        }
    }
    $name = "destmboxi".$i;
?>
    <input type="checkbox" 
<?php
            if (Policy_host_reference::in_policy_host_reference
                                                    ($conn, $id, 'any',
                                                    'source'))
            {
                echo " CHECKED ";
            }
?>
           name="<?php echo $name; ?>"
           value="any"><b>ANY</b><br></input>

<?php

    /* ===== dest nets =====*/
    $j = 1;
    if ($net_list = Net::get_list($conn)) {
        foreach ($net_list as $net) {
            $net_name = $net->get_name();
            if ($j == 1) {
?>
        <input type="hidden" name="<?php echo "destngrps"; ?>"
            value="<?php echo count($net_list); ?>">
<?php
            } $name = "destmboxg" . $j;
?>
        <input type="checkbox" 
<?php
            if (Policy_net_reference::in_policy_net_reference
                                                    ($conn, $id, $net_name,
                                                    'dest'))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $net_name; ?>">
            <?php echo $net_name . "<br>";?>
        </input>
<?php
            $j++;
        }
    }
?>

    </td>
  </tr>

  <tr>
    <th>Ports<br/>
        <font size="-2">
          <a href="../port/newportform.php">Insert new port group?</a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== ports ==== */
    $i = 1;
    if ($port_group_list = Port_group::get_list($conn)) {
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
        <input type="checkbox" 
<?php
            if (Policy_port_reference::in_policy_port_reference
                                            ($conn, $id, $port_group_name))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
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
    <th>Priority</th>
    <td class="left">
      <select name="priority">
        <option
        <?php if ($policy->get_priority() == 0) echo " SELECTED "; ?>
            value="0">0</option>
        <option
        <?php if ($policy->get_priority() == 1) echo " SELECTED "; ?>
            value="1">1</option>
        <option
        <?php if ($policy->get_priority() == 2) echo " SELECTED "; ?>
            value="2">2</option>
        <option
        <?php if ($policy->get_priority() == 3) echo " SELECTED "; ?>
            value="3">3</option>
        <option
        <?php if ($policy->get_priority() == 4) echo " SELECTED "; ?>
            value="4">4</option>
        <option
        <?php if ($policy->get_priority() == 5) echo " SELECTED "; ?>
            value="5">5</option>
        <option
        <?php if ($policy->get_priority() == 6) echo " SELECTED "; ?>
            value="6">7</option>
        <option
        <?php if ($policy->get_priority() == 7) echo " SELECTED "; ?>
            value="7">7</option>
        <option
        <?php if ($policy->get_priority() == 8) echo " SELECTED "; ?>
            value="8">8</option>
        <option
        <?php if ($policy->get_priority() == 9) echo " SELECTED "; ?>
            value="9">9</option>
        <option
        <?php if ($policy->get_priority() == 10) echo " SELECTED "; ?>
            value="10">10</option>
      </select>
    </td>
  </tr>

  <tr>
    <th>Signatures<br/>
        <font size="-2">
          <a href="../signature/newsignatureform.php">Insert new signature
          group?</a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== signatures ==== */
    $i = 1;
    if ($sig_group_list = Signature_group::get_list($conn)) {
        foreach($sig_group_list as $sig_group) {
            $sig_group_name = $sig_group->get_name();
            if ($i == 1) {
?>
        <input type="hidden" name="<?php echo "nsigs"; ?>"
            value="<?php echo count($sig_group_list); ?>">
<?php
            }
            $name = "mboxsg" . $i;
?>
        <input type="checkbox" 
<?php
            if (Policy_sig_reference::in_policy_sig_reference
                                         ($conn, $id, $sig_group_name))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $sig_group_name; ?>">
            <?php echo $sig_group_name . "<br>";?>
        </input>
<?php
            $i++;
        }
    }
?>
    </td>
  </tr>

  <tr>
    <th>Sensors<br/>
        <font size="-2">
          <a href="../sensor/newsensorform.php">Insert new sensor?</a>
        </font><br/>
    </th>
    <td class="left">
<?php

    /* ===== sensors ==== */
    $i = 1;
    if ($sensor_list = Sensor::get_list($conn)) {
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
        <input type="checkbox" 
<?php
            if (Policy_sensor_reference::in_policy_sensor_reference
                                         ($conn, $id, $sensor_name))
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
    <th>Description</th>
    <td class="left">
        <textarea name="descr" rows="2" 
            cols="20"><?php echo $policy->get_descr(); ?></textarea>
    </td>
  </tr>

<?php
    $db->close($conn);
?>
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

