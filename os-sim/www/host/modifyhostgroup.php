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

  <h1> <?php echo gettext("Update host group"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$host_group_name = POST('name');
$threshold_a = POST('threshold_a');
$threshold_c = POST('threshold_c');
$hhosts = POST('hhosts');
$rrd_profile = POST('rrd_profile');
$descr = POST('descr');
$nsens = POST('nsens');

ossim_valid($host_group_name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_SPACE, 'illegal:'._("Host name"));
ossim_valid($threshold_a, OSS_DIGIT, 'illegal:'._("threshold_a"));
ossim_valid($threshold_c, OSS_DIGIT, 'illegal:'._("threshold_c"));
ossim_valid($hhosts, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("hhosts"));
ossim_valid($rrd_profile, OSS_ALPHA, OSS_NULLABLE, OSS_SPACE, OSS_PUNC, 'illegal:'._("Host name"));
ossim_valid($descr, OSS_ALPHA, OSS_NULLABLE, OSS_SPACE, OSS_PUNC, OSS_AT, 'illegal:'._("Description"));
ossim_valid($nsens, OSS_NULLABLE, OSS_DIGIT, 'illegal:'._("nsens"));


if (ossim_error()) {
    die(ossim_error());
}

if(POST('insert')) {

    $sensors = array();
    $num_sens = 0;
    for ($i = 1; $i <= $nsens; $i++) {
        $name = "sboxs" . $i;
        if (POST("$name")) {
            $num_sens ++;
            ossim_valid(POST("$name"), OSS_ALPHA, OSS_SCORE, OSS_PUNC, OSS_AT);
            if (ossim_error()) {
                die(ossim_error());
            }
            $sensors[] = POST("$name");
        }
    }
    if (count($sensors)==0){
        ?>
        <p> <?php echo gettext("You Need to select at least one sensor"); ?> </p>
        <p><a href="hostgroup.php">
        <?php echo gettext("Back"); ?> </a></p>
        <?
        die();
    }


    $hosts = array();
    for ($i = 1; $i <= $hhosts; $i++) {
        $name = "mboxs" . $i;
	
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_NULLABLE, OSS_PUNC, OSS_SPACE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        $name_aux = POST("$name");

        if (!empty($name_aux))
            $hosts[] = POST("$name");
    }

    require_once 'ossim_db.inc';
    require_once 'classes/Host_group.inc';
    require_once 'classes/Host_group_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    Host_group::update ($conn, $host_group_name, $threshold_c, $threshold_a, $rrd_profile, $sensors, $hosts, $descr);
    Host_group_scan::delete ($conn, $host_group_name, 3001);
    if(POST('nessus')){
        Host_group_scan::insert ($conn, $host_group_name, 3001, 0);
    }

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Host group succesfully updated"); ?> </p>
    <p><a href="hostgroup.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

