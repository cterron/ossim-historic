<HTML>
<BODY bgcolor="#FFFFFF">

<?php

//include charts.php to access the InsertChart function
include "charts.php";

if(isset($_GET['width'])&&is_numeric($_GET['width']))
	$width=$_GET['width'];
else $width="450";

if(isset($_GET['height'])&&is_numeric($_GET['height']))
	$height=$_GET['height'];
else
	$height="350";

if(preg_match("/^[a-zA-Z0-9]+[a-zA-Z0-9_\-\.]+\.php$/", $_GET['source_graph']))
	echo InsertChart ( "charts.swf", "charts_library", $_GET['source_graph']."?".$_SERVER['QUERY_STRING'], $width, $height, "#FFF" );
else
	echo "<br>Invalid source file!! (It should be a php generating xml for chart.swf).<br>";

?>

</BODY>
</HTML>
