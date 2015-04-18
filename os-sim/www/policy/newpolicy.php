<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

<?php
    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["sourcenips"] || !$_POST["destnips"] ||
         !$_POST["sourcengrps"] || !$_POST["destngrps"] || !$_POST["nprts"] ||
         !$_POST["nsens"] || !$_POST["nsigs"] || !$_POST["descr"]))
{
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into DB */
} elseif($_POST["insert"]) {

    $priority = $_POST["priority"];
    $descr = $_POST["descr"];

    /* source ips */
    for ($i = 1; $i <= $_POST["sourcenips"]; $i++) {
        $name = "sourcemboxi" . $i;
        if ($_POST[$name]) {
            $source_ips[] = $_POST[$name];
        }
    }
                                                                                
    /* dest ips */
    for ($i = 1; $i <= $_POST["destnips"]; $i++) {
        $name = "destmboxi" . $i;
        if ($_POST[$name]) {
            $dest_ips[] = $_POST[$name];
        }
    }
                                                                                
    /* source nets */
    for ($i = 1; $i <= $_POST["sourcengrps"]; $i++) {
        $name = "sourcemboxg" . $i;
        if ($_POST[$name]) {
            $source_nets[] = $_POST[$name];
        }
    }
                                                                                
    /* dest nets */
    for ($i = 1; $i <= $_POST["destngrps"]; $i++) {
        $name = "destmboxg" . $i;
        if ($_POST[$name]) {
            $dest_nets[] = $_POST[$name];
        }
    }
                                                                                
    /* ports */
    for ($i = 1; $i <= $_POST["nprts"]; $i++) {
        $name = "mboxp" . $i;
        if ($_POST[$name]) {
            $ports[] = $_POST[$name];
        }
    }

    /* signatures */
    for ($i = 1; $i <= $_POST["nsigs"]; $i++) {
        $name = "mboxsg" . $i;
        if ($_POST[$name]) {
            $sigs[] = $_POST[$name];
        }
    }
    
    /* sensors */
    for ($i = 1; $i <= $_POST["nsens"]; $i++) {
        $name = "mboxs" . $i;
        if ($_POST[$name]) {
            $sensors[] = $_POST[$name];
        }
    }

    require_once ('classes/Policy.inc');
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    Policy::insert($conn, $priority, $descr,
                   $source_ips, $dest_ips, $source_nets, $dest_nets,
                   $ports, $sigs, $sensors);
?>
    <p>Policy succesfully inserted</p>
    <p><a href="policy.php">Back</a></p>
<?php
    $db->close($conn);
}
?>

</body>
</html>

