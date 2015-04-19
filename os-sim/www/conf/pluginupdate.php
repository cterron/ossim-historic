<html>
<head>
  <title> <?php echo gettext("Riskmeter"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<?php
    require_once ('ossim_db.inc');
    require_once ('classes/Security.inc');

    $id = REQUEST('id');
    $sid = REQUEST('sid');
    $priority = REQUEST('priority');
    $reliability = REQUEST('reliability');
    
    ossim_valid($id, OSS_ALPHA , 'illegal:'._("id") );
    ossim_valid($sid, OSS_ALPHA , 'illegal:'._("sid") );
    ossim_valid($priority, OSS_ALPHA , 'illegal:'._("priority") );
    ossim_valid($reliability, OSS_ALPHA , 'illegal:'._("reliability") );

    if (ossim_error()) {
        die(ossim_error());
    }

    if (($priority < 0) or ($priority > 5)) {
        echo "<p align=\"center\"> " . gettext("Priority must be between 0 and 5") . " </p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\"> " . gettext("Back") . " </a></p>";
        exit();
    }

    if (($reliability < 0) or ($reliability > 10)) {
        echo "<p align=\"center\"> " . gettext("Reliability must be between 0 and 10") . " </p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\"> " . gettext("Back") . " </a></p>";
        exit();
    }

    require_once ('classes/Plugin_sid.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    Plugin_sid::update($conn, $id, $sid, $priority, $reliability);

    $db->close($conn);

?>

    <p align="center">
    <?php echo gettext("Priority and reliability successfully updated <br/>"); ?><a href="pluginsid.php?id=<?php echo $id ?>"> 
    <?php echo gettext("Back"); ?> </a></p>
</body>
</html>

