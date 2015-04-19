<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelVulnerabilities");

?>


<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
    <script src="../js/prototype.js" type="text/javascript"></script>
    <script src="../js/scriptaculous/scriptaculous.js" type="text/javascript"></script>
    <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<body>

<h1><?=_("Nessus custom reports")?></h1>
<?php

require_once ('classes/Security.inc');
require_once ('classes/Util.inc');

$user = Session::get_session_user();

if (ossim_error()) {
    die(ossim_error());
}


// Previous reports

$i = 0;

if ($handle = @opendir('.')) {
    while (false !== ($file = readdir($handle))) {
   // We'll be prune to the "y3k" issue but I don't care
   if ((is_dir($file)) && !(strncmp($file,"2",1)) && strstr($file, ".report") && (strlen($file) == 21)){
        // Skip broken dirs. index.html should be present at least
        if(!file_exists($file . "/index.html")) continue;
        $folders[$i] = $file;
        $i++;
        }
    }
    closedir($handle);
}

if(is_array($folders)){
rsort($folders);
}

$num = count($folders);
?>
<center>
<table border="0" width="50%">
<tr>
<td colspan="7" border="0">
<h2><?php echo _("Last") . " " . $num . " " . _("Reports"); ?> </h2>
</td>
</tr>
<?php
for($i=0;$i<$num;$i++){
$file = $folders[$i];
if($file == "") continue;
?>
<tr>
<td>
<?php
$report_name = htmlentities(urldecode(file_get_contents("$file/report_name.txt")));
if($report_name == ""){
echo "Unknown report name";
} else {
echo $report_name;
}

?>
</td>
<td border="0">* <?php echo Util::timestamp2date(substr($file,0,14));?> </td>
<td border="0"> <a href="#" onClick="dothings('<?= $file; ?>/index.html')"><?php echo _("Show");?> </a></td>
<td border="0"> <a href="handle_scan.php?action=delete&scan_date=<?php echo $file; ?>"> <?php echo gettext("Delete"); ?> </a></td>
<td border="0"> <a href="handle_scan.php?action=archive&scan_date=<?php echo $file; ?>"> <?php echo gettext("Archive"); ?> </a></td>
</tr>
<?php
}
?>
</table>
<br/>
<a href="#" onClick="dothings('report_form.php')"><?=_("Generate new report")?></a>
</center>


<div id="report_div" style="display: none;">
<iframe id="report" src="about:blank" width="100%" height="100%"></iframe>
<ilayer id="report"><ilayer>
</div>
<script language="javascript">
function dothings(src){
load_iframe(src);
Effect.Appear('report_div', {from: 0.1, to: 1});
}
function load_iframe(src) {
  if (document.getElementById) {
    document.getElementById("report").src = src;
  }
  else if (document.all) {
    document.frames["report"].src = src;
  }
  else if (document.layers) {
    document.report.load(src,0);
  }
}
</script>
</body>
</html>
