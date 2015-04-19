<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelHids");
?>

<html>
<head>
  <title> <?= _("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?= _("Host IDS"); ?> </h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('ossim_sql.inc');
    require_once ('ossim_error.inc');
    require_once ('classes/Host_ids.inc');
    require_once ('classes/Security.inc');
    
    $ip = GET('ip');
    $date = GET('date');
    
    ossim_valid($ip, OSS_NULLABLE, OSS_IP_ADDR, 'illegal:'._("ip"));
    ossim_valid($date, OSS_NULLABLE, OSS_PUNC, OSS_SPACE, OSS_ALPHA, OSS_SCORE, 'illegal:'._("date"));

    if (ossim_error()) {
        die(ossim_error());
    }
                            
   
    $db = new ossim_db();
    $conn = $db->connect();
?>
<h2><?php echo "$ip";?></h2>
<hr noshade>
<table align="center" width="80%">
<tr>
<th> <?= _("Sid"); ?> </th>
<th> <?= _("What"); ?> </th>
<th> <?= _("Event Type"); ?> </th>
<th> <?= _("Target"); ?> </th>
<th> <?= _("Extra Data"); ?> </th>
</tr>
<?php 
        $host_ids_list = Host_ids::get_list($conn, "WHERE ip = inet_aton('$ip') and date = '$date'");
        if($host_ids_list){
            foreach($host_ids_list as $host){
            $sid = $host->get_sid();
            $sid = Host_ids::get_desc($conn, $sid);
            $what = Host_ids::beautify_what($host->get_what());
            $event_type = $host->get_event_type();
            $target = $host->get_target();
            $extra_data = $host->get_extra_data();
            if (preg_match('/^\[(.*)\]\[(.*)\]$/', $extra_data, $m)){
                $extra_data = "<font color=\"blue\">" . $m[1] . "</font> -> <font color=\"red\">" . $m[2] . "</font>";
            }
            printf("<TR><TD>
            $sid</TD><TD>
            $what</TD><TD>
            $event_type</TD><TD class=\"left\">
            <b>$target</b></TD><TD>
            $extra_data</TD><TD></TR>");
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
