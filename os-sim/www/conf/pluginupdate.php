<html>
<head>
  <title> Riskmeter </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<?php

    if ((!$id = $_REQUEST["id"]) or (!$sid = $_REQUEST["sid"])) {
        echo "<p align=\"center\">Unknown plugin id - sid</p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\">Back</a></p>";
        exit();
    }

    if ((!$priority = $_REQUEST["priority"]) or 
        (!$reliability = $_REQUEST["reliability"])) {
        echo "<p align=\"center\">No values for priority or reliability</p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\">Back</a></p>";
        exit();
    }

    if (($priority < 0) or ($priority > 10)) {
        echo "<p align=\"center\">Priority must be in (0..10) range</p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\">Back</a></p>";
        exit();
    }
    if (($reliability < 0) or ($reliability > 5)) {
        echo "<p align=\"center\">Reliability must be in (0..5) range</p>";
        echo "<p align=\"center\"><a href=\"pluginsid.php?id=$id\">Back</a></p>";
        exit();
    }

    require_once ('classes/Plugin_sid.inc');
    require_once ('ossim_db.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    Plugin_sid::update($conn, $id, $sid, $priority, $reliability);

    $db->close($conn);

?>
    <p align="center"><a href="pluginsid.php?id=<?php echo $id ?>">Back</a></p>
</body>
</html>

