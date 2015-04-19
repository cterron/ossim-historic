<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsDownloads");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Tool Downloads"); ?> </h1>

<?php 
require_once ('classes/Downloads.inc');
?>
<center>
<div align="left" style="margin-left:30px">
<dl>
<?php
foreach($downloads as $download){
print "<li><a href=\"" . $download["URL"] . "\">" . $download["Name"] . "</a> (" . $download["Version"] . ")</li>\n";
print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"" . $download["Homepage"] . "\"><small>" . $download["Homepage"] . "</small></a><br/>\n";
print "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<small>" . $download["Description"] . "</small><br/>\n";
print "<br/>&nbsp;";
}
?>
</dl>
</div>
</center>
    
</body>
</html>

