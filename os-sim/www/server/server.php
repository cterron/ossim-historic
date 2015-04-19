<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyServers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Servers"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'ossim_conf.inc';
    require_once 'classes/Server.inc';
    require_once 'classes/Plugin.inc';
    require_once 'classes/Security.inc';
    require_once 'server_get_servers.php';
    
    $order = GET('order');
    
    ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
  
    if (ossim_error()) {
        die(ossim_error());
    }
  
    if (empty($order))
         $order = "name";

    $ossim_conf = $GLOBALS["CONF"];
    $db = new ossim_db();
    $conn = $db->connect();
    
    /* get the port and IP address of the server */
    $address = $ossim_conf->get_conf("server_address");
    $port = $ossim_conf->get_conf("server_port");

    echo _("Master server at") . " <b>" . $address . ":" . $port . "</b> " . _("is") . " ";
    if(check_server($conn) == true){
    echo "<font color=\"green\">";
    echo _("UP");
    echo "</font>";
    // Server up
    } else {
    echo "<font color=\"red\">";
    echo _("DOWN");
    echo "</font>";
    // Server down
    }
    echo ".";

?>

  <table align="center">
  <tr>
  <th><?php echo gettext("Active Children Servers");?></th>
  <th><?php echo gettext("Total Children Servers");?></th>
  </tr><tr>
  <td><div id="active">0</div></td>
  <td><b><div id="total">0</div></b></td>
  </tr>
  </table>
  <br/>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php //you can re-order the rows
            echo ossim_db::get_order("inet_aton(ip)", $order);
          ?>">
	  <?php echo gettext("Ip"); ?> </a></th>

      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">
	  <?php echo gettext("Hostname"); ?> </a></th>

      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("port", $order);
          ?>">
	  <?php echo gettext("Port"); ?> </a></th>

      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("connect", $order);
          ?>">
	  <?php echo gettext("Active"); ?> </a></th>

      <th> <?php echo gettext("Correlate"); ?> </th>
      <th> <?php echo gettext("Cross Correlate"); ?> </th>
      <th> <?php echo gettext("Store"); ?> </th>
      <th> <?php echo gettext("Qualify"); ?> </th>
      <th> <?php echo gettext("Resend Alarms"); ?> </th>
      <th> <?php echo gettext("Resend Events"); ?> </th>

      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>

<?php
    //first, get the servers connected; all this servers are "actived"
    $server_list = server_get_servers($conn);
    $server_list_aux = $server_list; //here are stored the connected servers
    $server_stack = array(); //here will be stored the servers wich are in DDBB
    $server_configured_stack = array();
    if($server_list){
        foreach ($server_list as $server_status){
            if(in_array($server_status["servername"],$server_stack)) continue;
            array_push($server_stack,$server_status["servername"]);            
        }
    }

    $active_servers = 0;
    $total_servers = 0;
    
    if ($server_list = Server::get_list($conn, "ORDER BY $order")) {
        foreach($server_list as $server) {
            $ip = $server->get_ip();
            $name = $server->get_name();
            $total_servers++;

?>

    <tr>
      <td><?php echo $server->get_ip(); ?></td>
<!--      <td><a href="server_get_servers.php?name=<?php echo $name ?>"><?php echo $server->get_ip(); ?></a></td>-->
      <td><?php echo $server->get_name(); ?></td>
      <td><?php echo $server->get_port(); ?></td>
      <td><?php 
        if (in_array($server->get_name(),$server_stack)){
            echo "<font color=\"green\"><b>YES</b></font>";
            $active_servers++;
            array_push($server_configured_stack,$server->get_name());
        } else {
            echo "<font color=\"red\"><b>NO</b></font>";
        }
      ?></td>
      <?php
      $aux = $server->get_name();
      if ($role_list = Server::get_role($conn, "WHERE name = '$aux'")) {
        $role = $role_list[0];
    }
               ?><td><?php
                    if ($role->get_correlate() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_correlate() == 0)
                      echo _("No");
                ?></td>
               <td><?php
                    if ($role->get_cross_correlate() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_cross_correlate() == 0)
                      echo _("No");
                ?></td>
               <td><?php
                    if ($role->get_store() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_store() == 0)
                      echo _("No");
                ?></td>
               <td><?php
                    if ($role->get_qualify() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_qualify() == 0)
                      echo _("No");
                ?></td>
               <td><?php
                    if ($role->get_resend_alarm() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_resend_alarm() == 0)
                      echo _("No");
                ?></td>
               <td><?php
                    if ($role->get_resend_event() == 1){
                      echo _("Yes");
                    }
                    elseif ($role->get_resend_event() == 0)
                      echo _("No");
                ?></td><?php
       ?>



      <td><?php echo $server->get_descr(); ?></td>
      <td>
        [ <a href="modifyserverform.php?name=<?php echo $name ?>">
	<?php echo gettext("Modify"); ?> </a> |
        <a href="deleteserver.php?name=<?php echo $name ?>">
	<?php echo gettext("Delete"); ?> </a> ]</td>
    </tr>

<?php
        } /* foreach */
    } /* server_list */

    $db->close($conn);
?>

<?php
    $diff_arr = array_diff($server_stack,$server_configured_stack);

    if($diff_arr) {
			
?>
    <tr><td colspan="7"></td></tr>
    <tr>
      <td colspan="7"><font color="red"><b> <?php echo gettext("Warning"); ?> </b></font>:
        <?php echo gettext("the following children server(s) are being reported as enabled by the server but aren't configured"); ?> .
      </td>
    </tr>
<?php
        foreach($diff_arr as $name_diff) { 
              foreach ($server_list_aux as $server_name){
                if($name_diff == $server_name["servername"])
                { 
                  $aux = $server_name["host"];
                  $aux2 = $server_name["servername"];
                  break 1;
                }
              }
        
?>
    <tr>
      <td><?php echo $aux ?></td>
      <td><?php echo $aux2 ?></td>
      <td>-</td>
      <td><font color="green"><b> <?php echo gettext("YES"); ?> </b></font></td>
      <td>-</td>
       
      <td><a href="newserverform.php?ip=<?php echo $aux ?>&hostname=<?php echo $aux2?>"> 
      <?php echo gettext("Insert"); ?> </a></td>
    </tr>
    <tr><td colspan="7"></td></tr>
<?php
        }
   } 
?>
    <tr>
      <td colspan="12"><a href="newserverform.php"> <?php echo gettext("Insert new server"); ?> </a></td>
    </tr>
<!--    <tr>
      <td colspan="10"><a href="../conf/reload.php?what=servers"> <?php echo gettext("Reload"); ?> </a></td>
    </tr>-->
</table>

<script language="javascript">
active_servers_div = document.getElementById("active");
total_servers_div = document.getElementById("total");

<?php
if($active_servers == 0){
?>
active_servers_div.innerHTML = "<font color=\"red\">" + <?php echo $active_servers; ?> + "</font>"; 
<?php
} else {
?>
active_servers_div.innerHTML = "<font color=\"green\">" + <?php echo $active_servers; ?> + "</font>"; 
<?php
}
?>
total_servers_div.innerHTML = <?php echo $total_servers; ?>;
</script>

</body>
</html>

