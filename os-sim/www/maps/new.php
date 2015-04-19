<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMaps");

if (GET("engine") == 'openlayers_ve') {
    header("Location: openlayers.php?layer=ve"); exit;
}
if (GET("engine") == 'openlayers_op') {
    header("Location: openlayers.php?layer=op"); exit;
}
if (GET("engine") == 'openlayers_image') {
    header("Location: openlayers_image.php"); exit;
}

?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<h3><?=_("Select map engine")?></h3>

<form action="<?=$_SERVER['PHP_SELF']?>" method="get">  
<table align="center" widht="60%">
<tr>
    <td style="text-align: left"><input type="radio" name="engine" value="openlayers_ve">Openlayers Virtual Earth</td>
</tr><tr>
    <td style="text-align: left"><input type="radio" name="engine" value="openlayers_op">Openlayers Native</td>
</tr><tr>
    <td style="text-align: left"><input type="radio" name="engine" value="openlayers_image">Openlayers Image</td>
</tr></table>
<br>
<center><input type="submit" name="submit" value="<?=_("Next")?> &gt;"></center>
</form>
</body></html>