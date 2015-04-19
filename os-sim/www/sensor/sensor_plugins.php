<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuMonitors", "MonitorsSensors");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Sensors</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Plugin.inc';
    require_once 'get_sensors.php';
    require_once 'get_sensor_plugins.php';


    /* connect to db */
    $db = new ossim_db();
    $conn = $db->connect();


    $tmp_list = Sensor::get_list($conn);
    if (is_array($tmp_list)) {
        foreach ($tmp_list as $tmp) {
            $db_sensor_list[] = $tmp->get_ip();
        }
    }

    $sensor_list = server_get_sensors();

    /* what sensor? */
    if (isset($_GET["sensor"]))
        $ip_get = $_GET["sensor"];


    if (!$sensor_list && !$ip_get)
        echo "<p>There aren't any sensors connected to OSSIM server</p>";

    foreach ($sensor_list as $sensor)
    {
        $ip = $sensor["sensor"];
        $state = $sensor["state"];

        if ((isset($ip_get)) && ($ip_get != $ip))
            continue;

        if (($cmd = $_GET["cmd"]) && ($id = $_GET["id"])) {
           
            /*
             *  Send message to server
             *    sensor-plugin-CMD sensor="" plugin_id=""
             *  where CMD can be (start|stop|enabled|disabled)
             */
           
            require_once ('ossim_conf.inc');
            $ossim_conf = new ossim_conf();

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
           
            /* send command */
            $msg = "sensor-plugin-$cmd sensor=\"$ip\" plugin_id=\"$id\"";
            socket_write($socket, $msg, strlen($msg));
            socket_close($socket);

            /* wait for 
             *   framework => server -> agent -> server => framework
             * messages */
            sleep(5);
        }

        echo "<h2 align=\"center\">$ip</h2>";
        if (!in_array($ip, $db_sensor_list)) {
            echo "<p><b>Warning</b></font>:
            The sensor is being reported as enabled by
            the server but isn't configured.<br/>
            Click <a href=\"newsensorform.php?ip=$ip\">here</a>
            to configure the sensor.</p>";
            
        }


        /* get plugin list for each sensor */
        $sensor_plugins_list = server_get_sensor_plugins();

?>
  <table align="center">
    <tr>
      <th>Plugin</th>
      <th>Status</th>
      <th>Action</th>
      <th>Enabled</th>
      <th>Action</th>
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
      <td><font color="GREEN"><b>UP</b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=stop&id=$id" ?>">stop</a></td>
<?php
                    } else {
?>
      <td><font color="RED"><b>DOWN</b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=start&id=$id" ?>">start</a></td>
      
<?php
                    }
                    if ($enabled == 'true') {
?>
      <td><font color="GREEN"><b>ENABLED</b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=disabled&id=$id" ?>">disable</a></td>
<?php
                    } else {
?>
      <td><font color="RED"><b>DISABLED</b></font></td>
      <td><a href="<?PHP echo $_SERVER["PHP_SELF"] . 
            "?sensor=$ip&ip=$ip&cmd=enabled&id=$id" ?>">enable</a></td>
<?php
                    }
?>
    </tr>
<?php
                } # if
            } # foreach
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

