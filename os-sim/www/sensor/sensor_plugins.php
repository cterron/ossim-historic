<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsSensors");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Sensors"); ?> </h1>

<?php
    require_once 'ossim_conf.inc';
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Plugin.inc';
    require_once 'get_sensors.php';
    require_once 'get_sensor_plugins.php';
    require_once 'classes/Security.inc';
    
    $ip_get = GET('sensor');
    $cmd    = GET('cmd');
    $id     = GET('id');

    ossim_valid($ip_get, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("Sensor"));
    ossim_valid($cmd, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("Cmd"));
    ossim_valid($id, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("Id"));

    if (ossim_error()) {
        die(ossim_error());
    } 

    /* connect to db */
    $db = new ossim_db();
    $conn = $db->connect();

    $db_sensor_list = array();
    $tmp_list = Sensor::get_list($conn);
    if (is_array($tmp_list)) {
        foreach ($tmp_list as $tmp) {
            $db_sensor_list[] = $tmp->get_ip();
            $db_sensor_rel[$tmp->get_ip()] = $tmp->get_name();
        }
    }

    $sensor_list = server_get_sensors($conn);

    if (!$sensor_list && empty($ip_get))
        echo "<p> " . gettext("There aren't any sensors connected to OSSIM server") . " </p>";

$ossim_conf = $GLOBALS["CONF"];
$use_munin = $ossim_conf->get_conf("use_munin");
if($use_munin == 1){
$munin_link = $ossim_conf->get_conf("munin_link");
?><center><a href="<?= $munin_link; ?>"><img src="../pixmaps/stats.gif" border="0"></a></center>
<br/>
<?php
}


    foreach ($sensor_list as $sensor)
    {
        $ip = $sensor["sensor"];

        if (isset($db_sensor_rel[$ip]))
            $name = $db_sensor_rel[$ip];
        $state = $sensor["state"];

        if ((isset($ip_get)) && ($ip_get != $ip))
            continue;

        if ( (!empty($cmd)) && (!empty($id)) ) {
           
            /*
             *  Send message to server
             *    sensor-plugin-CMD sensor="" plugin_id=""
             *  where CMD can be (start|stop|enable|disable)
             */
           
            require_once ('ossim_conf.inc');
            $ossim_conf = $GLOBALS["CONF"];

            /* get the port and IP address of the server */
            $address = $ossim_conf->get_conf("server_address");
            $port = $ossim_conf->get_conf("server_port");

            /* create socket */
            $socket = socket_create (AF_INET, SOCK_STREAM, 0);
            if ($socket < 0) {
                echo "socket_create() failed: reason: " . 
                    socket_strerror ($socket) . "\n";
                exit();
            }

            /* connect */
            $result = socket_connect ($socket, $address, $port);
            if ($result < 0) {
                echo "socket_connect() failed.\nReason: ($result) " .
                    socket_strerror($result) . "\n\n";
                exit();
            }

            /* first send a connect message to server */
            $in = 'connect id="1" type="web"' . "\n";
            $out = '';
            socket_write($socket, $in, strlen($in));
            $out = socket_read($socket, 2048, PHP_NORMAL_READ);
            if (strncmp($out, "ok id=", 4)) {
                echo "<p><b>" . gettext("Bad response from server") . "</b></p>";
                break;
            }

            /* send command */
            $msg = "sensor-plugin-$cmd sensor=\"$ip\" plugin_id=\"$id\"\n";
            socket_write($socket, $msg, strlen($msg));
            socket_close($socket);

            /* wait for 
             *   framework => server -> agent -> server => framework
             * messages */
            sleep(5);
        }

        /* 
         *  show sensor ip (and sensor name if available) 
         *  at the top of the table  
         */
        echo "<h2 align=\"center\">$ip";
        if (isset($name)) echo " [ $name ]";
        echo "</h2>";

        if (is_array($db_sensor_list)) {
            if (!in_array($ip, $db_sensor_list)) {
                echo "<p><b>Warning</b></font>:
                The sensor is being reported as enabled by
                the server but isn't configured.<br/>
                Click <a href=\"newsensorform.php?ip=$ip\">here</a>
                to configure the sensor.</p>";
                
            }
        }

        /* get plugin list for each sensor */
        $sensor_plugins_list = server_get_sensor_plugins();

?>
  <table align="center">
    <tr>
      <th> <?php echo gettext("Plugin"); ?> </th>
      <th> <?php echo gettext("Status"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
      <th> <?php echo gettext("Enabled"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>
<?php
        if ($sensor_plugins_list) {
            foreach ($sensor_plugins_list as $sensor_plugin) {
                if ($sensor_plugin["sensor"] == $ip) {
                    $id      = $sensor_plugin["plugin_id"];
                    $state   = $sensor_plugin["state"];
                    $enabled = $sensor_plugin["enabled"];
                    if ($plugin_list = 
                            Plugin::get_list($conn, "WHERE id = $id"))
                    {
                        $plugin_name = $plugin_list[0]->get_name();
                    } else {
                        $plugin_name = $id;
                    }
                        
?>
    <tr>
      <td><?php echo $plugin_name ?></td>
<?php
                    if ($state == 'start') {
?>
      <td><font color="GREEN"><b> <?php echo gettext("UP"); ?> </b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=stop&id=$id" ?>">
	    <?php echo gettext("stop"); ?> </a></td>
<?php
                    } elseif ($state == 'stop') {
?>
      <td><font color="RED"><b> <?php echo gettext("DOWN"); ?> </b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=start&id=$id" ?>">
	    <?php echo gettext("start"); ?> </a></td>
      
<?php
                    } else {
                        echo "
                          <td>Unknown</td>
                          <td>-</td>
                        ";
                    }
                    
                    if ($enabled == 'true') {
?>
      <td><font color="GREEN"><b> <?php echo gettext("ENABLED"); ?> </b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=disable&id=$id" ?>">
	    <?php echo gettext("disable"); ?> </a></td>
<?php
                    } else {
?>
      <td><font color="RED"><b> <?php echo gettext("DISABLED"); ?> </b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=enable&id=$id" ?>">
	    <?php echo gettext("enable"); ?> </a></td>
<?php
                    }
?>
    </tr>
<?php
                } # if
            } # foreach
?>
    <tr>
      <td colspan="5">
        <a href="<?PHP echo $_SERVER["PHP_SELF"] . "?sensor=$ip" ?>"> Refresh </a>
      </td>
    </tr>
<?php
        } # if
?>
        </table>
      </td>
    </tr>
  </table>
  <br/>

<?php
    }
    $db->close($conn);
?>

</body>
</html>

