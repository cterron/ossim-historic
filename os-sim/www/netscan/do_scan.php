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
    require_once 'classes/Security.inc';
    
    $net = GET('net');
    $net_input = GET('net_input');
    
    ossim_valid($net, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Net"));
    ossim_valid($net_input, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Net"));

    if (ossim_error()) {
        die(ossim_error());
    }

    if (empty($net)) $net = $net_input;

    require_once ('classes/Scan.inc');
    $scan = new Scan($net);

    echo gettext("Scanning network")." ($net), ".gettext("please wait")."..<br/>"; flush();
    $scan->do_scan();

    echo gettext("Scan completed").".<br/><br/>";
    echo "<a href=\"index.php\">".gettext("Click here to show the results")."</a>";
    $scan->save_scan();

?>

</body>
</html>


