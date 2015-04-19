<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");

?>


<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
   <script src="../js/prototype.js" type="text/javascript"></script>
   <script src="../js/scriptaculous/scriptaculous.js" type="text/javascript"></script>
   <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<body>
<?php

require_once ('classes/Security.inc');
require_once ('classes/Util.inc');
require_once ('classes/Host_vulnerability.inc');
require_once ('classes/Host.inc');

$db = new ossim_db();
$conn = $db->connect();

$user = Session::get_session_user();
$conf = $GLOBALS['CONF'];
$conf_threshold = $conf->get_conf('threshold');

$action = REQUEST('action');
$nhosts = REQUEST('nhosts');
$title = urlencode(REQUEST('title'));

ossim_valid($num, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Number"));


if (ossim_error()) {
    die(ossim_error());
}

if($action == "generate"){
// Generate report
    $hosts = "";
    for ($i = 0; $i < $nhosts; $i++) {
        if($hosts == "")
            $hosts = REQUEST("host$i");
        else
            if(REQUEST("host$i") != "")
                $hosts .= "," . REQUEST("host$i");
    }


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

    if($title == "") $title = urlencode(_("Nessus Report"));

    $in = 'nessus action="report" title="' . $title . '" list="' .  $hosts . '"' . "\n";
    socket_write ($socket, $in, strlen ($in));
    print "<center><a href=\"report.php\" target=\"main\">" .  _("Report successfully created, please reload page.") . "</a></center>";

    socket_close($socket);

} else {
// Show form
$hosts = Host_vulnerability::get_list($conn, "", "", $aggregated = true);

$num = count($hosts);

if($num > 20){
    $cols = 5;
} else {
    $cols = 3;
}

$rows = intval($num / $cols) +1 ;

$global_i = 0;

$global_i = 0;

?>

<h3><?=_("Select hosts for custom report")?></h3>
<?=_("Note: Only hosts that have been scanned are available for selection")?>.

<form action="<?= $_SERVER["PHP_SELF"]?>" method="POST">
<?= _("Report title"); ?> <input type="text" name="title" size="50">
        <table width="100%" align="left" border="0"><tr>
        <?php
        for($i=1;$i<=$rows;$i++){
        ?>
        <?php
            for($a=0;$a <$cols && $global_i < $num ;$a++){
                $host = $hosts[$global_i];
                echo "<td width=\"" . intval(100/$cols) . "%\">";
                $all['sensors'][] = "sensor".$global_i;
                ?>
                <div align="left">
                <input align="left" type="checkbox" id="<?= "host".$global_i ?>" name="<?= "host".$global_i ?>" value="<?= $host ->get_ip() ?>" /><?= Host::ip2hostname($conn, $host->get_ip()); ?></div></td>
                <?php
                $global_i++;
            }
            echo "</tr>\n";
            ?>
            <?php
        }

?>
</table>
<center>
<input type="hidden" name="action" value="generate">
<input type="hidden" name="nhosts" value="<?php echo $global_i ?>" />
<input type="submit" value="<?= _("Submit"); ?>">
</center>
</form>
<?php
}
?>

</body>
</html>
