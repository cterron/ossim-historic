<?php
require_once ('classes/Session.inc');
Session::logcheck("MainMenu", "Index", "session/login.php");
?>
<html>
<head>  <title> <?php echo gettext("OSSIM"); ?> </title>
</head>
<frameset rows="130,*" border="0" frameborder="0">
<frame src="top.php?menu=main">
<frame src="control_panel/global_score.php" name="main">
</frameset>
</html>
