<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");
?>

<?php
// Testing some padding here for different browsers, see php flush() man page.
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Update Scan"); ?> </h1>

<?php
    require_once 'classes/Security.inc';

    $status = GET('status');
    ossim_valid($status, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Status"));

    if (ossim_error()) {
        die(ossim_error());
    }    
    
    require_once ('ossim_conf.inc');
    $conf = $GLOBALS["CONF"];
    $data_dir = $conf->get_conf("data_dir");

    /* Frameworkd's address & port */
    $address = $conf->get_conf("frameworkd_address");
    $port = $conf->get_conf("frameworkd_port");

   /* create socket */
    $socket = socket_create (AF_INET, SOCK_STREAM, 0);
    if ($socket < 0) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("CRE_SOCKET", array(socket_strerror ($socket)));
    }

    /* connect */
    $result = @socket_connect ($socket, $address, $port);
    if (!$result) {
            require_once("ossim_error.inc");
            $error = new OssimError();
            $error->display("FRAMW_NOTRUN", array($address.":".$port));
    }

    if($status == "reset"){
        $in = 'nessus reset now' . "\n";
        socket_write ($socket, $in, strlen ($in));
    }

    $in = 'nessus start now' . "\n";
    $out = '';
    socket_write ($socket, $in, strlen ($in));
 
    echo str_pad('',1024);  // minimum start for Safari
?>
<center> 
<?php echo gettext("Nessus scan started, depending on number of hosts to be scanned this may take a while"); ?>.
</center>
<center>
<?= _("Scan status:") . " " ?>
<div id="percentage">
<?= "0% " . _("completed.") ?>
</div>
</center>
<?php flush(); ?>
<?php
    $in = 'nessus status get' . "\n";

    while (socket_write($socket, $in, strlen ($in)) && ($out = socket_read ($socket, 255, PHP_BINARY_READ)))
    {
        if($out >0 && $out <100){
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML = '"' . <?php echo rtrim($out); ?> + "<?= "% " . _("completed.") ?>";
</script>
<?php
flush();
        } elseif ( $out < 0 ) {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =  "<?= '"' . _("Error! return was:") . " " ?>" + <?php echo rtrim($out); ?> + "<?= " " . _("Please check your frameworkd logs.") . '"'?>" ;
</script>
<?php
flush();
break;
        } elseif ( $out == 100 ) {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =  "<?= '"' . _("Scan succesfully completed.") . '"' ?>" ; 
</script>
<?php
flush();
break;
        } else {
            if(preg_match("/Error/",$out)){
        ?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =   <?= '"<BR>' . _("An error ocurred, please check your frameworkd & web server logs:") . '<BR><BR><b>' . rtrim($out) . '</b><BR>"' ?>; 
percentage_div.innerHTML += "<BR><a href=\"<?= $_SERVER["PHP_SELF"]?>?status=reset\" > <?= _("Reset") . "<BR>&nbsp;<BR>";?>"; 
</script>
<?php
flush();
break;
            } else {
?>
<script language="javascript">
percentage_div = document.getElementById("percentage");
percentage_div.innerHTML =   <?= '"<BR>' . _("Frameworkd said:") . '<BR><BR><b>' . rtrim($out) . '</b><BR>&nbsp;<BR>"' ?>; 
</script>

<?php
flush();
break;
            }
        }
        sleep(5);
    }

?>
<center><a href="index.php"> <?php echo gettext("Back"); ?> </a></center>
 
</body>
</html>

