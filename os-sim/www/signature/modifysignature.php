<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Modify signature group</h2>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["name"] || !$_POST["nsigs"] || !$_POST["descr"]))
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $name  = mysql_escape_string($_POST["name"]);
    $nsigs = mysql_escape_string($_POST["nsigs"]);
    $descr = mysql_escape_string($_POST["descr"]);

    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    for ($i = 1; $i <= $_POST["nsigs"]; $i++) {
        $mboxname = "mbox" . $i;
        if ($_POST[$mboxname]) {
            $signature_list[] = mysql_escape_string($_POST[$mboxname]);
        }
    }
   
    Signature_group::update ($conn, $name, $signature_list, $descr);

    $db->close($conn);
}
?>
    <p>Signature succesfully updated</p>
    <p><a href="signature.php">Back</a></p>

</body>
</html>

