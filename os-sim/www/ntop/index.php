<html>
<head>  <title> OSSIM </title>
</head>

<?php

    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();

?>

<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php">
<frame src="<?php echo $conf->get_conf("ntop_link")?>/trafficStats.html" name="ntop">
<body>
</body>
</html>

