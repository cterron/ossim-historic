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
                                                                                
  <h1> <?php echo gettext("New Host"); ?> </h1>

<?php
    require_once('classes/Security.inc');

    $insert = POST('insert');
    $hostname = POST('hostname');
    $ip = POST('ip');
    $id = POST('id');
    $threshold_c = POST('threshold_c');
    $threshold_a = POST('threshold_a');
    $nsens = POST('nsens');
    $asset = POST('asset');
    $alert = POST('alert');
    $persistence = POST('persistence');
    $nat = POST('nat');
    $descr = POST('descr');
    $os = POST('os');
    $mac = POST('mac');
    $mac_vendor = POST('mac_vendor');
    $nessus = POST('nessus');
    $nagios = POST('nagios');
    $sensor_name = POST('name');
    $rrd_profile = POST('rrd_profile');

    ossim_valid($insert, OSS_NULLABLE, OSS_ALPHA, 'illegal:'._("insert"));
    ossim_valid($hostname, OSS_NULLABLE, OSS_SPACE,  OSS_SCORE, OSS_ALPHA, OSS_PUNC, 'illegal:'._("hostname"));
    ossim_valid($ip, OSS_IP_ADDR, 'illegal:'._("ip"));
    ossim_valid($id, OSS_NULLABLE, OSS_ALPHA, OSS_SCORE, 'illegal:'._("id"));
    ossim_valid($threshold_a, OSS_NULLABLE, OSS_DIGIT, 'illegal:'._("threshold_a"));
    ossim_valid($threshold_c, OSS_NULLABLE, OSS_DIGIT, 'illegal:'._("threshold_c"));
    ossim_valid($nsens, OSS_NULLABLE, OSS_DIGIT, 'illegal:'._("nsens"));
    ossim_valid($asset, OSS_NULLABLE, OSS_DIGIT, 'illegal:'._("asset"));
    ossim_valid($alert, OSS_NULLABLE, OSS_ALPHA, 'illegal:'._("alert"));
    ossim_valid($persistence, OSS_NULLABLE, OSS_ALPHA, 'illegal:'._("persistence"));
    ossim_valid($nat, OSS_NULLABLE, OSS_IP_ADDR, 'illegal:'._("nat"));
    ossim_valid($descr, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, OSS_AT, 'illegal:'._("descr"));
    ossim_valid($rrd_profile, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, OSS_AT, 'illegal:'._("rrd_profile"));
    ossim_valid($os, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, OSS_AT, 'illegal:'._("os"));
    ossim_valid($mac_vendor, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, OSS_AT, 'illegal:'._("mac_vendor"));
    ossim_valid($mac, OSS_NULLABLE, OSS_ALPHA, OSS_PUNC, 'illegal:'._("mac"));
    ossim_valid($nessus, OSS_NULLABLE, OSS_ALPHA , 'illegal:'._("nesus"));
    ossim_valid($nagios, OSS_NULLABLE, OSS_ALPHA , 'illegal:'._("nagios"));
    ossim_valid($sensor_name, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, 'illegal:'._("Sensor name"));

    if (ossim_error()) {
        die(ossim_error());
    }

    if(!empty($insert)) {

    $num_sens = 0;
    for ($i = 1; $i <= $nsens; $i++) {
        $name = "mboxs" . $i;
        if (POST("$name")) {
            $num_sens ++;
            ossim_valid(POST("$name"), OSS_ALPHA, OSS_SCORE, OSS_PUNC, OSS_AT);
            if (ossim_error()) {
                die(ossim_error());
            }
            $sensors[] = POST("$name");
        }
    }
    if (!isset($sensors)){
        ?>
        <p> <?php echo gettext("You Need to select at least one sensor"); ?> </p>
        <p><a href="newhostform.php">
        <?php echo gettext("Back"); ?> </a></p>
        <? 
        die();
    }

    require_once 'ossim_db.inc';
    require_once 'classes/Host.inc';
    require_once 'classes/Host_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (! Host::in_host($conn, $ip)) {
        Host::insert ($conn, $ip, $hostname, $asset, 
                      $threshold_c, $threshold_a, $rrd_profile, $alert, 
                      $persistence, $nat, $sensors, $descr, 
                      $os, $mac, $mac_vendor);
        if (!empty($nessus)) {
            Host_scan::insert ($conn, $ip, 3001, 0);
        }
        if (!empty($nessus)) {
            Host_scan::insert ($conn, $ip, 2007, 0);
        }
    } else {
        Host::update ($conn, $ip, $hostname, $asset,
                      $threshold_c, $threshold_a, $rrd_profile, $alert,
                      $persistence, $nat, $sensors, $descr,
                      $os, $mac, $mac_vendor);
    }


    $db->close($conn);
}
?>
    <p> <?php echo gettext("Host succesfully inserted"); ?> </p>
    <p><a href="host.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

