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
  
  <h1><?php echo gettext("Rule viewer"); ?></h1>

<?php
    require_once ('ossim_conf.inc');
    require_once ('dir.php');

    $ossim_conf = $GLOBALS["CONF"];
    $snort_rules_path = $ossim_conf->get_conf("snort_rules_path");
?>

  <table align="center">
<?php
    $files = getDirFiles($snort_rules_path);

    /* local snort rule directory */
    if ($files == NULL) {
          require_once("ossim_error.inc");
          $error = new OssimError();
          $error->display("RULES_NOT_FOUND", array($snort_rules_path));
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
