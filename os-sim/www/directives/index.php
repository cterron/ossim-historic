<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuCorrelation", "CorrelationDirectives");
?>

<html>
<frameset cols="260,100%" border="0" frameborder="0">
<frame src="directiveslist.php" name="directives_list">
<frame src="directive.php" name="directives">
</frameset>
</html>
