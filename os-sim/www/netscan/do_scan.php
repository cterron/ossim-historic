<?php
    // menu authentication
    require_once ('classes/Session.inc');
    Session::logcheck("MenuTools", "ToolsScan");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

    if (!$net = $_GET["net"]) {
        if (!$net = $_GET["net_input"]) {
            echo gettext ("Missing net argument..");
            exit;
        }
    }

    require_once ('classes/Scan.inc');
    $scan = new Scan($net);

    echo "Scanning network ($net), please wait..<br/>"; flush();
    $scan->do_scan();

    echo "Scan completed.<br/><br/>";
    echo "<a href=\"index.php\">Click here to show the results</a>";
    $scan->save_scan();

?>

</body>
</html>


