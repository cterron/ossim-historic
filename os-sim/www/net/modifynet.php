<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyNetworks");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Update net"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$net_name = POST('name');
$threshold_a = POST('threshold_a');
$threshold_c = POST('threshold_c');
$asset = POST('asset');
$descr = POST('descr');
$nsens = POST('nsens');
$ips = POST('ips');
$alert = POST('alert');
$persistence = POST('persistence');
$rrd_profile = POST('rrd_profile');

ossim_valid($net_name, OSS_NET_NAME, 'illegal:'._("Net name"));
ossim_valid($ips, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("Ips"));
ossim_valid($asset, OSS_DIGIT, 'illegal:'._("Asset"));
ossim_valid($threshold_a, OSS_DIGIT, 'illegal:'._("threshold_a"));
ossim_valid($threshold_c, OSS_DIGIT, 'illegal:'._("threshold_c"));
ossim_valid($nsens, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("nnets"));
ossim_valid($alert, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Alert"));
ossim_valid($persistence, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Persistence"));
ossim_valid($rrd_profile, OSS_ALPHA, OSS_NULLABLE, OSS_SPACE, OSS_PUNC,'illegal:'._("Net name"));
ossim_valid($descr, OSS_ALPHA, OSS_NULLABLE, OSS_SPACE, OSS_PUNC, OSS_AT, 'illegal:'._("Description"));

if (ossim_error()) {
        die(ossim_error());
}

if (POST('insert')) {
   
   $sensors = array();
    for ($i = 1; $i <= $nsens; $i++) {
        $name = "mboxs" . $i;
        ossim_valid(POST($name), OSS_NULLABLE, OSS_ALPHA, OSS_PUNC, OSS_SPACE);
        if (ossim_error()) {
            die(ossim_error());
        }
        $name_aux = POST($name);
        if (!empty($name_aux))
            $sensors[] = POST($name);
    }
    if (!count($sensors)) {
        die(ossim_error(_("At least one sensor is required")));
    }
    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    require_once 'classes/Net_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();
   
    Net::update ($conn, $net_name, $ips, $asset, $threshold_c, $threshold_a, 
                 $rrd_profile, $alert, $persistence, $sensors, $descr);
    Net_scan::delete ($conn, $net_name, 3001);
    Net_scan::delete ($conn, $net_name, 2007);
    if(POST('nessus')){
        Net_scan::insert ($conn, $net_name, 3001, 0);
    }
    if(POST('nagios')){
        Net_scan::insert ($conn, $net_name, 2007, 0);
    }

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Net succesfully updated"); ?> </p>
    <p><a href="net.php">
    <?php echo gettext("Back"); ?> </a></p>
<?php
// update indicators on top frame
$OssimWebIndicator->update_display();
?>

</body>
</html>

