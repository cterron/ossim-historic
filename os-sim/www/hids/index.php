<?php
//require_once ('classes/Session.inc');
//Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Host IDS</h1>

<?php
    require_once ('ossim_db.inc');
    
    $db = new ossim_db();
    $conn = $db->connect();
?>
<table>
<?php 

        $query = "SELECT count(sid), inet_ntoa(ip), date FROM host_ids group by ip,date";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
        $ip = $rs->fields["inet_ntoa(ip)"];
        $date = $rs->fields["date"];
        $sid = $rs->fields["count(sid)"];
        printf("<TR><TH>
        <A HREF=\"host_detail.php?ip=$ip&date=$date\">$ip</A></TH>
        <TD>$date</TD><TD>$sid</TD></TR>");
                $rs->MoveNext();            
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
