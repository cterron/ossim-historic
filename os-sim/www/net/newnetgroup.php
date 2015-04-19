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
                                                                                
  <h1> <?php echo gettext("New network group"); ?> </h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["name"] ||
         !$_POST["threshold_c"] || !$_POST["threshold_a"] || 
         !$_POST["nnets"] || !$_POST["descr"])) 
    {
    require_once("ossim_error.inc");
    $error = new OssimError();
    $error->display("FORM_MISSING_FIELDS");

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $net_group_name    = validateVar($_POST["name"], OSS_ALPHA . OSS_PUNC .
    OSS_SCORE);
    $threshold_c = validateVar($_POST["threshold_c"], OSS_DIGIT);
    $threshold_a = validateVar($_POST["threshold_a"], OSS_DIGIT);
    $rrd_profile = validateVar($_POST["rrd_profile"]);
    $descr       = validateVar($_POST["descr"], OSS_ALPHA . OSS_PUNC .
    OSS_SCORE);
    
    for ($i = 1; $i <= validateVar($_POST["nnets"], OSS_DIGIT); $i++) {
        $name = "mboxs" . $i;
        if (validateVar($_POST[$name])) {
            $nets[] = validateVar($_POST[$name]);
        }
    }

    require_once 'ossim_db.inc';
    require_once 'classes/Net.inc';
    require_once 'classes/Net_group.inc';
    require_once 'classes/Net_group_scan.inc';
    $db = new ossim_db();
    $conn = $db->connect();
   
    Net_group::insert ($conn, $net_group_name, $threshold_c, $threshold_a, $rrd_profile, $nets, $descr);

    if($_POST["nessus"]){
        Net_group_scan::insert ($conn, $net_group_name, 3001, 0);
    }

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Network group succesfully inserted"); ?> </p>
    <p><a href="netgroup.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

