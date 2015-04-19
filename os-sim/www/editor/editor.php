<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsRuleViewer");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
  
  <h1>Rule viewer</h1>

<?php
    require_once ('ossim_conf.inc');
    require_once ('dir.php');

    $ossim_conf = new ossim_conf();
    $snort_rules_path = $ossim_conf->get_conf("snort_rules_path");
?>

  <table align="center">
<?php
    $files = getDirFiles($snort_rules_path);

    /* local snort rule directory */
    if ($files == NULL) {
        printf(gettext("Sorry, can't locate snort rules at <b>%s</b>"), 
               $snort_rules_path);
        exit;
    }

    foreach ($files as $file) {

        /* only show .rules files */
        $f = split ("\.", $file);
        if ($f[1] == 'rules') {
?>
    <tr><td>
    <a href="rule.php?name=<?php echo $file; ?>"><?php echo $f[0]; ?></a>
    </td></tr>
<?php
        }
    }
?>
  </table>
</body>
</html>
