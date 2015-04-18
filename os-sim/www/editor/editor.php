<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
                                                                                
  <h2>Rule viewer</h2>

<?php
    require_once ('ossim_conf.inc');
    require_once ('dir.php');

    $ossim_conf = new ossim_conf();
    $snort_rules_path = $ossim_conf->get_conf("snort_rules_path");
?>

  <p align="center">
<?php
    $files = getDirFiles($snort_rules_path);
    foreach ($files as $file) {

        /* only show .rules files */
        $f = split ("\.", $file);
        if ($f[1] == 'rules') {
?>
    <a href="rule.php?name=<?php echo $file; ?>"><?php echo $f[0]; ?></a>
    <br/>
<?php
        }
    }
?>
  </p>
</body>
</html>
