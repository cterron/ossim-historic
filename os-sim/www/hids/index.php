<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelHids");
?>

<html>
<head>
  <title><?= _("OSSIM Framework") ?></title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1><?= _("Host IDS") ?></h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('ossim_sql.inc');
    require_once ('ossim_error.inc');
    require_once ('classes/Host_ids.inc');
    
    $db = new ossim_db();
    $conn = $db->connect();
?>
<div align="center">
<img src="hids_graph.php?limit=10">
</div>
<br>
<hr noshade>
<br>
<table align="center" width="80%">
<tr>
<th> <?= _("Host") ?> </th><th> <?= _("Event date") ?> </th><th> <?= _("Events"); ?> </th>
</tr>
<?php 
        if($host_ids_list = Host_ids::get_list_reduced($conn, "", "group by ip order by 'count(sid)' desc ")){
            foreach($host_ids_list as $host) {
        $ip = $host->get_ip();
        $date = $host->get_date();
        $count = $host->get_count();
        printf("<TR><TH>
        <A HREF=\"host_detail.php?ip=$ip&date=$date\">$ip</A></TH>
        <TD>$date</TD><TD>$count</TD></TR>");
            }
        }
?>
</table>
</body>
</html>
<?php

$db->close($conn);
exit();
?>
