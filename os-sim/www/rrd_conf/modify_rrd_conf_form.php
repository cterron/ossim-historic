<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Modify RRD Config</h1>
    
  <h3>Hints</h3>
  <ul>
  <li> Threshold: Absolute value above which is being alerted.
  <li> Priority: Resulting impact if threshold is being exceeded.
  <li> Alpha: Intercept adaption parameter.
  <li> Beta: Slope adaption parameter.
  <li> Persistence: How long has this event to last before we alert. (20 mins)
  </ul>


<?php
    require_once 'classes/RRD_config.inc';
    require_once 'classes/Host.inc';
    require_once 'ossim_db.inc';

  
    if (!$order = $_GET["order"]) $order = "rrd_attrib";
    
    $ip = $_REQUEST["ip"];

    $db = new ossim_db();
    $conn = $db->connect();


    if (($_POST["ip"]) && ($_POST["insert"])) 
    {
        $rrd_list = RRD_Config::get_list($conn, 
            "WHERE ip = inet_aton('$ip')");
        
        if ($rrd_list) 
        {
            foreach ($rrd_list as $rrd) 
            {
                $attrib = $rrd->get_rrd_attrib();
            
                if (isset($_POST["$attrib#rrd_attrib"]))
                {
                    RRD_Config::update ($conn, 
                                        $_POST["ip"], 
                                        $_POST["$attrib#rrd_attrib"], 
                                        $_POST["$attrib#threshold"], 
                                        $_POST["$attrib#priority"], 
                                        $_POST["$attrib#alpha"], 
                                        $_POST["$attrib#beta"], 
                                        $_POST["$attrib#persistence"]);
                }
            }
        }
    }
    
    /* 
     * title: 
     *  ip -> hostname | 0.0.0.0 -> GLOBAL
     */
    $host = Host::ip2hostname($conn, $ip);
    echo "<h2>";
    if (!strcmp($host, "0.0.0.0")) echo "GLOBAL";
    else echo $host;
    echo "</h2>";

    
    $rrd_list = RRD_Config::get_list($conn, 
        "WHERE ip = inet_aton('$ip') ORDER BY $order");    
    
    $db->close($conn);
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("rrd_attrib", $order); ?>&ip=<?php
                echo $ip ?>">Attribute</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("threshold", $order); ?>&ip=<?php
                echo $ip ?>">Threshold</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("priority", $order); ?>&ip=<?php
                echo $ip ?>">Priority</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("alpha", $order); ?>&ip=<?php
                echo $ip ?>">Alpha</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("beta", $order); ?>&ip=<?php
                echo $ip ?>">Beta</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("persistence", $order); ?>&ip=<?php
                echo $ip ?>">Persistence</a></th>
      <td></td>
    </tr>
      <form method="post" action="<?php echo $_SERVER["PHP_SELF"]?>">
        <input type="hidden" name="insert" value="1" />
        <input type="hidden" name="ip" value="<?php echo $ip ?>"/>
<?php
    if ($rrd_list) {
        foreach ($rrd_list as $rrd) {

            $rrd_attrib     = $rrd->get_rrd_attrib();
            $threshold      = $rrd->get_threshold();
            $priority       = $rrd->get_priority();
            $alpha          = $rrd->get_alpha();
            $beta           = $rrd->get_beta();
            $persistence    = $rrd->get_persistence();
?>
    <tr>
        <td bgcolor="#eeeeee"><?php echo $rrd->get_rrd_attrib(); ?></td>
        <input type="hidden" name="<?php echo $rrd_attrib ?>#rrd_attrib" 
            value="<?php echo $rrd_attrib ?>"/>
        <td><input type="text" name="<?php echo $rrd_attrib ?>#threshold" 
            size="8" value="<?php echo $threshold ?>"/></td>
        <td><input type="text" name="<?php echo $rrd_attrib ?>#priority" 
            size="2" value="<?php echo $priority ?>"/></td>
        <td><input type="text" name="<?php echo $rrd_attrib ?>#alpha" 
            size="8" value="<?php echo $alpha ?>"/></td>
        <td><input type="text" name="<?php echo $rrd_attrib ?>#beta" 
            size="8" value="<?php echo $beta ?>"/></td>
        <td><input type="text" name="<?php echo $rrd_attrib ?>#persistence" 
            size="2" value="<?php echo $persistence ?>"/></td>
    </tr>
<?php
        }
    }
?>
    <tr>
        <td colspan="6"><input type="submit" value="Modify"/></td>
    </tr>
    </form>
  </table>


</body>
</html>

