<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuPolicy", "PolicyPolicy");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("New policy"); ?> </h1>

<?php
require_once 'classes/Security.inc';

$priority   = POST('priority');
$begin_hour = POST('begin_hour');
$end_hour   = POST('end_hour');
$begin_day  = POST('begin_day');
$end_day    = POST('end_day');
$descr      = POST('descr');
$store      = POST('store');

ossim_valid($priority, OSS_NULLABLE, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Priority"));
ossim_valid($begin_hour, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Begin hour"));
ossim_valid($begin_day, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Begin day"));
ossim_valid($end_day, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("End day"));
ossim_valid($end_hour, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("End hour"));
ossim_valid($descr, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("Description"));
ossim_valid($store, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("Store"));

if (ossim_error()) {
    die(ossim_error());
}

if (POST('insert')) {

    /*
     *  Check correct range of dates
     *
     *  Fri 21h = ((5 - 1) * 7) + 21 = 49
     *  Sat 14h = ((6 - 1) * 7) + 14 = 56
     */
    $begin_expr = (($begin_day -1) * 7) + $begin_hour;
    $end_expr = (($end_day -1) * 7) + $end_hour;
    if ($begin_expr >= $end_expr) {
      require_once("ossim_error.inc");
      $error = new OssimError();
      $error->display("INCORRECT_DATE_RANGE");
    }

    /* source ips */
    $source_ips = array();
    for ($i = 1; $i <= POST('sourcenips'); $i++) {
        $name = "sourcemboxi" . $i;
        
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $source_ips[] = POST("$name");
        }
    }
    
    /* source nets */
    $source_nets = array();
    for ($i = 1; $i <= POST('sourcengrps'); $i++) {
        $name = "sourcemboxg" . $i;
        
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $source_nets[] = POST("$name");
        }

    }
    if (!count($source_ips) && !count($source_nets)) {
        die(ossim_error(_("At least one Source IP or Net required")));
    }
    
    /* dest ips */
    $dest_ips = array();
    for ($i = 1; $i <= POST('destnips'); $i++) {
        $name = "destmboxi" . $i;
        
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $dest_ips[] = POST("$name");
        }
    }
                                                                                                                                                                
    /* dest nets */
    $dest_nets = array();
    for ($i = 1; $i <= POST('destngrps'); $i++) {
        $name = "destmboxg" . $i;
        
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $dest_nets[] = POST("$name");
        }
    }
    if (!count($dest_ips) && !count($dest_nets)) {
        die(ossim_error(_("At least one Destination IP or Net required")));
    }
                                                                                
    /* ports */
    $ports = array();
    for ($i = 1; $i <= POST('nprts'); $i++) {
        $name = "mboxp" . $i;
    
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $ports[] = POST("$name");
        }
    }
    if (!count($ports)) {
        die(ossim_error(_("At least one Port required")));
    }

    /* plugin groups */
    $plug_groups = array();
    $plugins = POST('plugins');
    if ($plugins) {
        foreach ($plugins as $group_id => $on) {
            ossim_valid($group_id, OSS_DIGIT, 'illegal:'._("Plugin Group ID"));
            $plug_groups[] = $group_id;
        }
    }
    if (!count($plug_groups)) {
        die(ossim_error(_("At least one plugin group required")));
    }
    if (ossim_error()) {
        die(ossim_error());
    }
    
    /* sensors */
    $sensors = array();
    for ($i = 1; $i <= POST('nsens'); $i++) {
        $name = "mboxs" . $i;
        
        $aux_name = POST("$name");
        ossim_valid(POST("$name"), OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_AT, OSS_NULLABLE, 'illegal:'._("$name"));

        if (ossim_error()) {
            die(ossim_error());
        }

        if (!empty($aux_name)) {
            $sensors[] = POST("$name");
        }

    }
    if (!count($sensors)) {
        die(ossim_error("At least one Sensor required"));
    }

    require_once ('classes/Policy.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    Policy::insert($conn, $priority, 
                   $begin_hour, $end_hour, $begin_day, $end_day, $descr,
                   $source_ips, $dest_ips, $source_nets, $dest_nets,
                   $ports, $plug_groups, $sensors, $store);
?>
    <p> <?php echo gettext("Policy succesfully inserted"); ?> </p>
    <p><a href="policy.php">
    <?php echo gettext("Back"); ?> </a></p>
<?php
    $db->close($conn);
}
?>

</body>
</html>

