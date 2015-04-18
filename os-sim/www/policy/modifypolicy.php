<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Update Policy</h1>

<?php

    if (!$id = $_POST["id"]) {
        echo "No polici id specified";
        exit;
    }

    /* check params */
    if ((mysql_escape_string($_POST["insert"])) &&
        (!mysql_escape_string($_POST["sourcenips"]) ||
        !mysql_escape_string($_POST["destnips"]) ||
         !mysql_escape_string($_POST["sourcengrps"]) ||
         !mysql_escape_string($_POST["destngrps"]) ||
         !mysql_escape_string($_POST["nprts"]) ||
         !mysql_escape_string($_POST["nsens"]) ||
         !mysql_escape_string($_POST["nsigs"]) ||
         !mysql_escape_string($_POST["begin_day"]) ||
         !mysql_escape_string($_POST["end_day"]) ||
         !mysql_escape_string($_POST["descr"])))
{
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into DB */
} elseif(mysql_escape_string($_POST["insert"])) {

    $priority = mysql_escape_string($_POST["priority"]);
    $begin_hour = mysql_escape_string($_POST["begin_hour"]);
    $end_hour   = mysql_escape_string($_POST["end_hour"]);
    $begin_day  = mysql_escape_string($_POST["begin_day"]);
    $end_day    = mysql_escape_string($_POST["end_day"]);
    $descr = mysql_escape_string($_POST["descr"]);

    /*
     *  Check correct range of dates
     *
     *  Fri 21h = ((5 - 1) * 7) + 21 = 49
     *  Sat 14h = ((6 - 1) * 7) + 14 = 56
     */
    $begin_expr = (($begin_day -1) * 7) + $begin_hour;
    $end_expr = (($end_day -1) * 7) + $end_hour;
    if ($begin_expr >= $end_expr) { 
?>
        <p align="center">Error: Incorrect range of dates!</p>
<?php   
        exit;
    }

    /* source ips */
    for ($i = 1; $i <= mysql_escape_string($_POST["sourcenips"]); $i++) {
        $name = "sourcemboxi" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $source_ips[] = mysql_escape_string($_POST[$name]);
        }
    }
                                                                                
    /* dest ips */
    for ($i = 1; $i <= mysql_escape_string($_POST["destnips"]); $i++) {
        $name = "destmboxi" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $dest_ips[] = mysql_escape_string($_POST[$name]);
        }
    }
                                                                                
    /* source nets */
    for ($i = 1; $i <= mysql_escape_string($_POST["sourcengrps"]); $i++) {
        $name = "sourcemboxg" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $source_nets[] = mysql_escape_string($_POST[$name]);
        }
    }
                                                                                
    /* dest nets */
    for ($i = 1; $i <= mysql_escape_string($_POST["destngrps"]); $i++) {
        $name = "destmboxg" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $dest_nets[] = mysql_escape_string($_POST[$name]);
        }
    }
                                                                                
    /* ports */
    for ($i = 1; $i <= mysql_escape_string($_POST["nprts"]); $i++) {
        $name = "mboxp" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $ports[] = mysql_escape_string($_POST[$name]);
        }
    }

    /* signatures */
    for ($i = 1; $i <= mysql_escape_string($_POST["nsigs"]); $i++) {
        $name = "mboxsg" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $sigs[] = mysql_escape_string($_POST[$name]);
        }
    }
    
    /* sensors */
    for ($i = 1; $i <= mysql_escape_string($_POST["nsens"]); $i++) {
        $name = "mboxs" . $i;
        if (mysql_escape_string($_POST[$name])) {
            $sensors[] = mysql_escape_string($_POST[$name]);
        }
    }

    require_once ('classes/Policy.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();


    Policy::update($conn, $id, $priority, $begin_hour, $end_hour, 
                   $begin_day, $end_day, $descr,
                   $source_ips, $dest_ips, $source_nets, $dest_nets,
                   $ports, $sigs, $sensors);
?>
    <p>Policy succesfully updated</p>
    <p><a href="policy.php">Back</a></p>
<?php
    $db->close($conn);
}
?>

</body>
</html>

