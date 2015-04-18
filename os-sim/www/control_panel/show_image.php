<HTML>
<HEAD>
<META HTTP-EQUIV="Pragma" CONTENT="no-cache">
</HEAD>
<BODY>

<?php
require_once ('ossim_conf.inc');

$conf = new ossim_conf();


 $ip = mysql_escape_string($_GET["ip"]);
 $what = mysql_escape_string($_GET["what"]);
 $start = mysql_escape_string($_GET["start"]);
 $type = mysql_escape_string($_GET["type"]);
 $zoom = mysql_escape_string($_GET["zoom"]);
 $graph_link = $conf->get_conf("graph_link");

 ?>

<table align="center" width="100%">
    <tr><td align="center">
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=day&ip=<?php echo
      "$ip&what=$what&start=N-1D&type=$type&zoom=$zoom"?>">Last Day</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=month&ip=<?php echo
      "$ip&what=$what&start=N-1M&type=$type&zoom=$zoom"?>">Last Month</a>]
      [<a href="<?php echo $_SERVER["PHP_SELF"] ?>?range=year&ip=<?php echo
      "$ip&what=$what&start=N-1Y&type=$type&zoom=$zoom"?>">Last Year</a>]
    </td><td>Show [<a href="<?php echo $_SERVER["PHP_SELF"]?><?php echo
    "?range=$range&ip=$ip&what=compromise&start=$start&type=$type&zoom=$zoom"?>">
    Compromise</a>]<BR>
    Show [<a href="<?php echo $_SERVER["PHP_SELF"]?><?php echo
    "?range=$range&ip=$ip&what=attack&start=$start&type=$type&zoom=$zoom"?>">
    Attack</a>]</td></tr>
    <tr><td colspan="2"><HR noshade></td></tr>
    <tr><td colspan="2" align="center">
      <img src="<?php echo "$graph_link?ip=$ip&what=$what&start=$start&end=N&type=$type"; ?>">
    </td></tr>
    </table>
</HTML>
<BODY>
