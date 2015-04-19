<?php
require_once ('classes/Session.inc');
Session::logcheck("MainMenu", "Index", "session/login.php");

require_once ('ossim_conf.inc');
$conf = $GLOBALS["CONF"];
$ossim_link = $conf->get_conf("ossim_link");
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM"); ?> </title>
  <link rel="alternate" title="OSSIM Alarm Console"
        href="<?php echo "$ossim_link/feed/alarm_console.php" ?>"
        type="application/rss+xml">
</head>
<frameset rows="130,*" border="0" frameborder="0">
<frame src="top.php?menu=main">
<frame src="#" name="main">
</frameset>
</html>
